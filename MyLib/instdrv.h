#pragma once
#include <windows.h>
#include <winsvc.h>

//安装驱动
BOOL scmInstallDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName,
    _In_opt_ LPCTSTR ServiceExe
);
//启动驱动
BOOL scmStartDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
);
//打开驱动
BOOL scmOpenDevice(
    _In_ LPCTSTR DriverName,
    _Inout_opt_ PHANDLE lphDevice
);
//停止驱动
BOOL scmStopDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
);
//删除驱动
BOOL scmRemoveDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
);
//卸载驱动
BOOL scmUnloadDeviceDriver(
    _In_ LPCTSTR Name
);
//载入驱动
BOOL scmLoadDeviceDriver(
    _In_		LPCTSTR Name,
    _In_opt_	LPCTSTR Path,
    _Inout_		PHANDLE lphDevice
);
