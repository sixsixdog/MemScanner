
#include "ProcManager.h"
#include <TlHelp32.h>
#include <tchar.h>
#include "Define.h"
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")

ProcManager::ProcManager()
{
}


ProcManager::~ProcManager()
{
}

DWORD_PTR ProcManager::GetModuleBase(DWORD dwPid,LPCTSTR szModName)
{
	HANDLE        hSnap;
	MODULEENTRY32 me;
	me.dwSize = sizeof(me);
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);

	if (hSnap == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	BOOL bRet = Module32First(hSnap, &me);

	while (bRet)
	{
		if (_tcsicmp(me.szModule,szModName)==0)
		{
			CloseHandle(hSnap);
			return DWORD_PTR(me.modBaseAddr);
		}
		bRet = Module32Next(hSnap, &me);
	}
	CloseHandle(hSnap);
	return false;
}




int ProcManager::GetProcessIdByName(LPCTSTR szProcess)//注意要加exe后缀
{
	int dwRet = -1;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe32);
	do
	{
		if (_tcsicmp(pe32.szExeFile, szProcess) == 0)
		{
			dwRet = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapshot, &pe32));
	CloseHandle(hSnapshot);
	return dwRet;
}

BOOL ProcManager::EnableDebugPriv()
{
	HANDLE   hToken;
	LUID   sedebugnameValue;
	TOKEN_PRIVILEGES   tkp;
	//打开进程令牌环
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return   FALSE;
	}
	//获取进程本地唯一ID
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return   FALSE;
	}
	//调整权限
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//禁用所有权限标志->false
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		return   FALSE;
	}
	//关闭内核对象
	CloseHandle(hToken);
	return TRUE;

}
//根据pid获取线程组
int ProcManager::GetProcessThreadNumByID(DWORD dwPID)
{
	//获取进程快照列表
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//判断快照是否成功
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return 0;
	
	//32位程序信息结构体
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	//取出快照中第一个程序快照信息
	BOOL bRet = ::Process32First(hProcessSnap, &pe32);;
	//循环获取下一个程序直到取到指定pid的进程或者快照列表遍历完毕
	while(bRet)
	{
		//与目标pid对比
		if (pe32.th32ProcessID == dwPID)
		{
			//返回线程组
			::CloseHandle(hProcessSnap);
			return pe32.cntThreads;
		}
		bRet = ::Process32Next(hProcessSnap, &pe32);
	}
	return 0;
}

//获取程序pid
int ProcManager::GetAowProcId()
{
	DWORD dwRet = 0;
	DWORD dwThreadCountMax = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &pe32);
	do
	{
		if (_tcsicmp(pe32.szExeFile, EXE_NAME) == 0)
		{
			//根据pid获取线程号
			DWORD dwTmpThreadCount = GetProcessThreadNumByID(pe32.th32ProcessID);
			if (dwTmpThreadCount > dwThreadCountMax)
			{
				dwThreadCountMax = dwTmpThreadCount;
				dwRet = pe32.th32ProcessID;
			}
		}
	} while (Process32Next(hSnapshot, &pe32));
	CloseHandle(hSnapshot);
	return dwRet;
}