#pragma once

// Windows-only temporary-directory cleanup. No shell and no process-global chdir.
// Reparse points are leaves; intermediate reparse components are rejected.
// This is not a defence against a hostile process replacing directories while
// cleanup is in progress. ABC has already exited before its temp tree is cleaned.
#ifdef _WIN32
#include <windows.h>
#include <algorithm>
#include <climits>
#include <cstddef>
#include <string>
#include <vector>

namespace FpgaNativeCleanup {

struct Result {
    DWORD error = ERROR_SUCCESS;
    size_t removed = 0;
    bool missing = false;
    std::wstring absolute_path;
};

inline bool is_missing(DWORD error)
{
    return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
}

inline void remember_error(Result &result, DWORD error)
{
    if (result.error == ERROR_SUCCESS) result.error = error;
}

inline bool same_prefix(const std::wstring &left, const std::wstring &right, size_t count)
{
    return count <= left.size() && count <= right.size() && count <= INT_MAX &&
        CompareStringOrdinal(left.c_str(), int(count), right.c_str(), int(count), TRUE) == CSTR_EQUAL;
}

inline bool utf8_to_wide(const std::string &value, std::wstring &wide, DWORD &error)
{
    if (value.empty() || value.size() > INT_MAX || value.find('\0') != std::string::npos) {
        error = ERROR_INVALID_NAME;
        return false;
    }
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), int(value.size()), nullptr, 0);
    if (!count) { error = GetLastError(); return false; }
    wide.resize(count);
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), int(value.size()), wide.data(), count)) {
        error = GetLastError();
        return false;
    }
    std::replace(wide.begin(), wide.end(), L'/', L'\\');
    return true;
}

inline bool absolute_lexical(const std::wstring &input, std::wstring &path, size_t &root_length, DWORD &error)
{
    std::wstring value = input;
    if (value.compare(0, 4, L"\\\\.\\") == 0) { error = ERROR_INVALID_NAME; return false; }
    if (value.compare(0, 4, L"\\\\?\\") == 0) {
        if (value.compare(4, 4, L"UNC\\") == 0) value = L"\\\\" + value.substr(8);
        else if (value.size() >= 7 && value[5] == L':' && value[6] == L'\\') value.erase(0, 4);
        else { error = ERROR_INVALID_NAME; return false; }
    }
    DWORD count = GetFullPathNameW(value.c_str(), 0, nullptr, nullptr);
    if (!count) { error = GetLastError(); return false; }
    std::vector<wchar_t> buffer(count);
    DWORD length = GetFullPathNameW(value.c_str(), count, buffer.data(), nullptr);
    if (!length) { error = GetLastError(); return false; }
    if (length >= count) { error = ERROR_FILENAME_EXCED_RANGE; return false; }
    path.assign(buffer.data(), length);
    root_length = 0;
    if (path.size() >= 3 && path[1] == L':' && path[2] == L'\\') root_length = 3;
    else if (path.compare(0, 2, L"\\\\") == 0) {
        size_t server_end = path.find(L'\\', 2);
        if (server_end == std::wstring::npos || server_end == 2) { error = ERROR_INVALID_NAME; return false; }
        size_t share_end = path.find(L'\\', server_end + 1);
        if (share_end == std::wstring::npos) root_length = path.size();
        else {
            if (share_end == server_end + 1) { error = ERROR_INVALID_NAME; return false; }
            root_length = share_end + 1;
        }
    } else { error = ERROR_INVALID_NAME; return false; }
    while (path.size() > root_length && path.back() == L'\\') path.pop_back();
    if (path.size() <= root_length) { error = ERROR_ACCESS_DENIED; return false; }
    return true;
}

inline std::wstring extended_path(const std::wstring &path)
{
    if (path.compare(0, 2, L"\\\\") == 0) return L"\\\\?\\UNC\\" + path.substr(2);
    return L"\\\\?\\" + path;
}

inline bool valid_parent_components(const std::wstring &path, size_t root_length, Result &result)
{
    for (size_t end = path.find(L'\\', root_length); end != std::wstring::npos; end = path.find(L'\\', end + 1)) {
        DWORD attrs = GetFileAttributesW(extended_path(path.substr(0, end)).c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES) {
            DWORD error = GetLastError();
            if (is_missing(error)) result.missing = true;
            else remember_error(result, error);
            return false;
        }
        if (!(attrs & FILE_ATTRIBUTE_DIRECTORY) || (attrs & FILE_ATTRIBUTE_REPARSE_POINT)) {
            remember_error(result, ERROR_CANT_ACCESS_FILE);
            return false;
        }
    }
    return true;
}

inline void remove_node(const std::wstring &path, DWORD attrs, Result &result)
{
    if (!(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        if (DeleteFileW(path.c_str())) result.removed++;
        else { DWORD error = GetLastError(); if (!is_missing(error)) remember_error(result, error); }
        return;
    }
    if (attrs & FILE_ATTRIBUTE_REPARSE_POINT) {
        if (RemoveDirectoryW(path.c_str())) result.removed++;
        else { DWORD error = GetLastError(); if (!is_missing(error)) remember_error(result, error); }
        return;
    }
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileW((path + L"\\*").c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (error != ERROR_FILE_NOT_FOUND) { remember_error(result, error); return; }
    } else {
        do {
            if (!wcscmp(data.cFileName, L".") || !wcscmp(data.cFileName, L"..")) continue;
            std::wstring child = path + L"\\" + data.cFileName;
            DWORD child_attrs = GetFileAttributesW(child.c_str());
            if (child_attrs == INVALID_FILE_ATTRIBUTES) {
                DWORD error = GetLastError();
                if (!is_missing(error)) remember_error(result, error);
                continue;
            }
            remove_node(child, child_attrs, result);
        } while (FindNextFileW(find, &data));
        DWORD error = GetLastError();
        FindClose(find);
        if (error != ERROR_NO_MORE_FILES) remember_error(result, error);
    }
    if (RemoveDirectoryW(path.c_str())) result.removed++;
    else { DWORD error = GetLastError(); if (!is_missing(error)) remember_error(result, error); }
}

inline Result remove_tree(const std::string &dirname)
{
    Result result;
    std::wstring input;
    if (!utf8_to_wide(dirname, input, result.error)) return result;
    size_t root_length;
    if (!absolute_lexical(input, result.absolute_path, root_length, result.error)) return result;

    // Refuse deleting the current directory or an ancestor of it.
    DWORD cwd_size = GetCurrentDirectoryW(0, nullptr);
    if (!cwd_size) { result.error = GetLastError(); return result; }
    std::vector<wchar_t> cwd_buffer(cwd_size);
    DWORD cwd_length = GetCurrentDirectoryW(cwd_size, cwd_buffer.data());
    if (!cwd_length) { result.error = GetLastError(); return result; }
    if (cwd_length >= cwd_size) { result.error = ERROR_FILENAME_EXCED_RANGE; return result; }
    std::wstring cwd(cwd_buffer.data(), cwd_length);
    size_t length = result.absolute_path.size();
    if (same_prefix(result.absolute_path, cwd, length) &&
        (cwd.size() == length || cwd[length] == L'\\')) {
        result.error = ERROR_ACCESS_DENIED;
        return result;
    }
    if (!valid_parent_components(result.absolute_path, root_length, result)) return result;
    std::wstring path = extended_path(result.absolute_path);
    DWORD attrs = GetFileAttributesW(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        if (is_missing(error)) result.missing = true;
        else result.error = error;
        return result;
    }
    if (!(attrs & FILE_ATTRIBUTE_DIRECTORY)) { result.error = ERROR_DIRECTORY; return result; }
    remove_node(path, attrs, result);
    return result;
}

}
#endif
