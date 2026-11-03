if(-not ('FpgaNativePeak' -as [type])) {
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class FpgaNativePeak {
    [StructLayout(LayoutKind.Sequential)]
    struct Counters {
        public uint cb, faults;
        public UIntPtr peak, current, peakPaged, paged, peakNonPaged, nonPaged, pagefile, peakPagefile;
    }
    [DllImport("psapi.dll", SetLastError=true)]
    static extern bool GetProcessMemoryInfo(IntPtr process, ref Counters counters, uint size);
    public static ulong Read(IntPtr process) {
        var c = new Counters();
        c.cb = (uint)Marshal.SizeOf(c);
        if(!GetProcessMemoryInfo(process, ref c, c.cb))
            throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
        return c.peak.ToUInt64();
    }
}
'@
}
