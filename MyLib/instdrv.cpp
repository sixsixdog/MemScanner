//驱动 未启用
#include "instdrv.h"
#include <stdio.h>

BOOL scmInstallDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName,
    _In_opt_ LPCTSTR ServiceExe
)
{
    SC_HANDLE  schService=NULL;
	for (int i = 0; i < 3 && (NULL == schService); i++)
	{
		schService = CreateService(SchSCManager, // 服务控制管理程序维护的登记数据库的句柄
			DriverName,            // 服务名称
			DriverName,            // 显示名称
			SC_MANAGER_ALL_ACCESS, // 返回类型
			SERVICE_KERNEL_DRIVER, // 服务类型
			SERVICE_DEMAND_START,  // 何时开始
			SERVICE_ERROR_IGNORE,  // 启动失败的严重程度
			ServiceExe,            // 指定服务程序二进制文件的路径
			NULL,                  // 顺序装入的服务组名
			NULL,                  // 忽略(标志符)
			NULL,                  // 依赖的服务
			NULL,                  // 服务帐号。如是NULL,则表示使用LocalSystem
			NULL                   // 密码。NULL表示无密码
			);
		if (schService == NULL) {
			printf("s1%s\n", DriverName);
			char szOut[100] = { 0 };
			wsprintfA(szOut, "%x weeor:%d", (DWORD_PTR)SchSCManager, GetLastError());
			OutputDebugStringA(szOut);
		}
		else
		{
			break;
		}
	}
	
    CloseServiceHandle(schService);
    return TRUE;
}

//启动驱动
BOOL scmStartDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
)
{
    SC_HANDLE  schService;
    BOOL       ret;
    //打开服务
    schService = OpenService(SchSCManager,
        DriverName,
		SC_MANAGER_ALL_ACCESS
    );
	if (schService == NULL)
	{
		return FALSE;
	}
        
    //开启服务
    ret = StartService(schService, 0, NULL)
        || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;

    CloseServiceHandle(schService);

    return ret;
}

//打开驱动
BOOL scmOpenDevice(
    _In_ LPCTSTR DriverName,
    _Inout_opt_ PHANDLE lphDevice
)
{
    TCHAR    completeDeviceName[64];
    HANDLE   hDevice;

    //初始化内存同ZeroMemory 比ZeroMemory安全
    RtlSecureZeroMemory(completeDeviceName, sizeof(completeDeviceName));
    wsprintf(completeDeviceName, TEXT("\\\\.\\%s"), DriverName);
    //创建文件
    hDevice = CreateFile(completeDeviceName,
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0
    );
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
        

    if (lphDevice) {
        *lphDevice = hDevice;
    }
    else {
        CloseHandle(hDevice);
    }

    return TRUE;
}

//停止驱动
BOOL scmStopDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
)
{
    BOOL            ret;
    INT             iRetryCount;
    SC_HANDLE       schService;
    SERVICE_STATUS  serviceStatus;

    ret = FALSE;
    //打开服务
	schService = OpenService(SchSCManager, DriverName, SC_MANAGER_ALL_ACCESS);
    if (schService == NULL) {
        return ret;
    }

    iRetryCount = 5;
    do {
        SetLastError(0);
        //服务管理器
        ret = ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus);
        if (ret != FALSE)
            break;

        if (GetLastError() != ERROR_DEPENDENT_SERVICES_RUNNING)
            break;
        Sleep(1000);
        iRetryCount--;
    } while (iRetryCount);
    CloseServiceHandle(schService);

    return ret;
}
//删除驱动
BOOL scmRemoveDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
)
{
    SC_HANDLE  schService;
    BOOL       bResult = FALSE;
    //打开服务
	schService = OpenService(SchSCManager, DriverName, SC_MANAGER_ALL_ACCESS);
    if (schService) {
        //删除服务
        bResult = DeleteService(schService);
        CloseServiceHandle(schService);
    }
    return bResult;
}

//卸载设备驱动
BOOL scmUnloadDeviceDriver(
    _In_ LPCTSTR Name
)
{
    SC_HANDLE schSCManager;
    BOOL      bResult = FALSE;

    if (Name == NULL) {
        return bResult;
    }
    //打开服务管理器
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager) {
        scmStopDriver(schSCManager, Name);
        bResult = scmRemoveDriver(schSCManager, Name);
        CloseServiceHandle(schSCManager);
    }
    return bResult;
}

//载入设备驱动
BOOL scmLoadDeviceDriver(
    _In_		LPCTSTR Name,
    _In_opt_	LPCTSTR Path,
    _Inout_		PHANDLE lphDevice
)
{
    SC_HANDLE schSCManager;
    BOOL      bResult = FALSE;

    if (Name == NULL) {
        return bResult;
    }
    //打开服务管理器
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager) {
        //scmRemoveDriver(schSCManager, Name);
		scmUnloadDeviceDriver(Name);
        scmInstallDriver(schSCManager, Name, Path);
        scmStartDriver(schSCManager, Name);
        bResult = scmOpenDevice(Name, lphDevice);
        CloseServiceHandle(schSCManager);
		
    }
	
    return bResult;
}
