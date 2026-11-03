"""Embed the same process-local UTF-8 manifest in private Yosys/ABC copies.

No system locale, PATH, or installed program is changed. Windows 10 1903+
is required for activeCodePage; the manifest contains no elevation request.
Equivalent source builds can link resource 1/RT_MANIFEST using windres.
"""
import ctypes
from ctypes import wintypes as W
from pathlib import Path
import hashlib,json,shutil,sys

ROOT=Path(__file__).resolve().parent
K=ctypes.WinDLL('kernel32',use_last_error=True)
K.BeginUpdateResourceW.argtypes=[W.LPCWSTR,W.BOOL];K.BeginUpdateResourceW.restype=W.HANDLE
K.UpdateResourceW.argtypes=[W.HANDLE,W.LPCWSTR,W.LPCWSTR,W.WORD,W.LPVOID,W.DWORD];K.UpdateResourceW.restype=W.BOOL
K.EndUpdateResourceW.argtypes=[W.HANDLE,W.BOOL];K.EndUpdateResourceW.restype=W.BOOL
def embed(path):
    data=(ROOT/'windows-utf8.manifest').read_bytes()
    h=K.BeginUpdateResourceW(str(path.resolve()),False)
    if not h:raise ctypes.WinError(ctypes.get_last_error())
    buf=ctypes.create_string_buffer(data)
    # Integer resource names use MAKEINTRESOURCE, including RT_MANIFEST=24.
    if not K.UpdateResourceW(h,ctypes.cast(24,W.LPCWSTR),ctypes.cast(1,W.LPCWSTR),0,buf,len(data)):
        code=ctypes.get_last_error();K.EndUpdateResourceW(h,True);raise ctypes.WinError(code)
    if not K.EndUpdateResourceW(h,False):raise ctypes.WinError(ctypes.get_last_error())
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
if __name__=='__main__':
    for value in sys.argv[1:]:
        p=Path(value);before=sha(p);embed(p);print(json.dumps({'path':str(p),'original_sha256':before,'utf8_sha256':sha(p)}))
