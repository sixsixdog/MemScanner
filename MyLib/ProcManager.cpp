
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




int ProcManager::GetProcessIdByName(LPCTSTR szProcess)//ע��Ҫ��exe��׺
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
	//�򿪽������ƻ�
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return   FALSE;
	}
	//��ȡ���̱���ΨһID
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		return   FALSE;
	}
	//����Ȩ��
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//��������Ȩ�ޱ�־->false
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		return   FALSE;
	}
	//�ر��ں˶���
	CloseHandle(hToken);
	return TRUE;

}
//����pid��ȡ�߳���
int ProcManager::GetProcessThreadNumByID(DWORD dwPID)
{
	//��ȡ���̿����б�
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//�жϿ����Ƿ�ɹ�
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return 0;
	
	//32λ������Ϣ�ṹ��
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	//ȡ�������е�һ�����������Ϣ
	BOOL bRet = ::Process32First(hProcessSnap, &pe32);;
	//ѭ����ȡ��һ������ֱ��ȡ��ָ��pid�Ľ��̻��߿����б�������
	while(bRet)
	{
		//��Ŀ��pid�Ա�
		if (pe32.th32ProcessID == dwPID)
		{
			//�����߳���
			::CloseHandle(hProcessSnap);
			return pe32.cntThreads;
		}
		bRet = ::Process32Next(hProcessSnap, &pe32);
	}
	return 0;
}

//��ȡ����pid
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
			//����pid��ȡ�̺߳�
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