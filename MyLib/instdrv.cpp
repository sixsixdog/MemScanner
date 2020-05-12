//���� δ����
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
		schService = CreateService(SchSCManager, // SCManager database
			DriverName,           // name of service
			DriverName,           // name to display
			SC_MANAGER_ALL_ACCESS,    // desired access
			SERVICE_KERNEL_DRIVER, // service type
			SERVICE_DEMAND_START,  // start type
			SERVICE_ERROR_IGNORE,  // error control type
			ServiceExe,            // service's binary
			NULL,                  // no load ordering group
			NULL,                  // no tag identifier
			NULL,                  // no dependencies
			NULL,                  // LocalSystem account
			NULL                   // no password
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

/*
* scmStartDriver
*
* Purpose:
*
* Start service, resulting in SCM drvier load.
*
*/
BOOL scmStartDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
)
{
    SC_HANDLE  schService;
    BOOL       ret;

    schService = OpenService(SchSCManager,
        DriverName,
		SC_MANAGER_ALL_ACCESS
    );
	if (schService == NULL)
	{
		return FALSE;
	}
        

    ret = StartService(schService, 0, NULL)
        || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;

    CloseServiceHandle(schService);

    return ret;
}

/*
* scmOpenDevice
*
* Purpose:
*
* Open driver device by symbolic link.
*
*/
BOOL scmOpenDevice(
    _In_ LPCTSTR DriverName,
    _Inout_opt_ PHANDLE lphDevice
)
{
    TCHAR    completeDeviceName[64];
    HANDLE   hDevice;

    RtlSecureZeroMemory(completeDeviceName, sizeof(completeDeviceName));
    wsprintf(completeDeviceName, TEXT("\\\\.\\%s"), DriverName);

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

/*
* scmStopDriver
*
* Purpose:
*
* Command SCM to stop service, resulting in driver unload.
*
*/
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
	schService = OpenService(SchSCManager, DriverName, SC_MANAGER_ALL_ACCESS);
    if (schService == NULL) {
        return ret;
    }

    iRetryCount = 5;
    do {
        SetLastError(0);
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

/*
* scmRemoveDriver
*
* Purpose:
*
* Remove service entry from SCM database.
*
*/
BOOL scmRemoveDriver(
    _In_ SC_HANDLE SchSCManager,
    _In_ LPCTSTR DriverName
)
{
    SC_HANDLE  schService;
    BOOL       bResult = FALSE;

	schService = OpenService(SchSCManager, DriverName, SC_MANAGER_ALL_ACCESS);
    if (schService) {
        bResult = DeleteService(schService);
        CloseServiceHandle(schService);
    }
    return bResult;
}

/*
* scmUnloadDeviceDriver
*
* Purpose:
*
* Combines scmStopDriver and scmRemoveDriver.
*
*/
BOOL scmUnloadDeviceDriver(
    _In_ LPCTSTR Name
)
{
    SC_HANDLE schSCManager;
    BOOL      bResult = FALSE;

    if (Name == NULL) {
        return bResult;
    }
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager) {
        scmStopDriver(schSCManager, Name);
        bResult = scmRemoveDriver(schSCManager, Name);
        CloseServiceHandle(schSCManager);
    }
    return bResult;
}

/*
* scmLoadDeviceDriver
*
* Purpose:
*
* Unload if already exists, Create, Load and Open driver instance.
*
*/
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
