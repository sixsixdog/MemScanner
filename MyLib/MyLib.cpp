// MyLib.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <tchar.h>
#include "GameESP.h"
#include "Define.h"

#include <TlHelp32.h>
#include <Psapi.h>
#include <math.h>
#include "instdrv.h"

#pragma comment(lib,"Psapi.lib")


const TCHAR *szCaption = _T("雷电模拟器");



int main()
{

	//提升权限
	ProcManager::EnableDebugPriv();
	

// 	HANDLE hDevice = NULL;
// 	TCHAR szDrvPath[MAX_PATH] = { 0 };
// 	GetModuleFileName(NULL, szDrvPath, MAX_PATH);
// 	*_tcsrchr(szDrvPath, '\\') = '\0';
// 	_tcscat(szDrvPath, _T("\\wsex.sys"));
// 	BOOL bRet = scmLoadDeviceDriver(_T("CyberPeaceA"), szDrvPath, &hDevice);
// 	if (!bRet || hDevice == NULL)
// 	{
// 		printf("初始化失败!\n");
// 		system("pause");
// 		return 1;
// 	}

	//获取pid
	dwGamePid = ProcManager::GetAowProcId();
	if (!dwGamePid)
	{
		return GET_THID_FAILED;
	}
	//打开进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE, dwGamePid);
	
	//内存扫描器
	pMM = new MemoryManager(hProcess);
	
	//创建线程并立即销毁内核对象
	CloseHandle(CreateThread(0, 0, ThreadUpdateData, 0, 0, 0));
// 	CloseHandle(CreateThread(0, 0, ThreadUpdateData2, 0, 0, 0));
//	CloseHandle(CreateThread(0, 0, ThreadUpdateData3, 0, 0, 0));
//   失效 未更新
	while (1)
	{
		//窗口菜单
		pDxm = new DxManager();
		//动态追随
		pDxm->SetupWindow(szCaption);

		MSG msg;
		while (1)
		{
			ZeroMemory(&msg, sizeof(MSG));
			//非阻塞式获取消息
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				//翻译消息
				TranslateMessage(&msg);
				//分发消息
				DispatchMessage(&msg);
			}

			HWND hWnd = NULL;
			//获取宿主窗口
			hWnd = FindWindow(0, szCaption);
			
			if (!hWnd)
			{
				//宿主销毁时销毁自身
				DestroyWindow(pDxm->over_hWnd);
				delete pDxm;
				break;
			}
			//获取
			hWnd = ::GetWindow(hWnd, GW_CHILD);
			//窗口位置信息
			RECT rc;
			//初始化
			ZeroMemory(&rc, sizeof(RECT));
			
			//获取窗口位置
			GetWindowRect(hWnd, &rc);
			
			//根据宿主窗口位置设置覆盖窗口的位置信息
			pDxm->s_width = rc.right - rc.left;
			pDxm->s_height = rc.bottom - rc.top;

			//移动到宿主窗口上方
			MoveWindow(pDxm->over_hWnd, rc.left, rc.top, pDxm->s_width, pDxm->s_height, true);
			
			//移动窗口
			HWND hwnd2 = GetForegroundWindow();
			HWND hwnd3 = GetWindow(hwnd2, GW_HWNDPREV);
			//设置窗口位置
			SetWindowPos(pDxm->over_hWnd, hwnd3, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			//立即更新窗口
			UpdateWindow(pDxm->over_hWnd);
			//绘制
			pDxm->render();
		}
	}
	
}
