#include "DxManager.h"
#include "WRand.h"
#include <time.h>
#include "GameESP.h"
#include "Misc.h"

DxManager::DxManager()
{
	game_hWnd = NULL;
	g_pIndexBuffer = NULL;
	g_pVertexBuffer = NULL;
}

DxManager::~DxManager()
{
	SAFE_RELEASE(pD3D);
	SAFE_RELEASE(g_pIndexBuffer)
	SAFE_RELEASE(g_pVertexBuffer)
	SAFE_RELEASE(pFont)
	SAFE_RELEASE(pFontMenu)
	SAFE_RELEASE(pDevice)
	SAFE_RELEASE(pD3DLine)
		
}

//render function
void DxManager::render()
{
	// clear the window alpha
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET| D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);//D3DCLEAR_ZBUFFER ���ӵĻ��任��ʾЧ��û��
	pDevice->BeginScene();    // begins the 3D scene
							 //calculate and and draw esp stuff
	ESPWork();
	pDevice->EndScene();    // ends the 3D scene
	pDevice->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	switch (uMsg)
	{
	case WM_PAINT:
	{
		DxManager* dm = (DxManager*)GetWindowLong(hWnd, GWL_USERDATA);
		if (!dm) return -1;
		dm->render();
		ValidateRect(hWnd, NULL);   // ���¿ͻ�������ʾ
	}	
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void DxManager::SetupWindow(LPCTSTR szWndName)
{

	RECT rc;
	while (!game_hWnd)
	{
		Sleep(500);
		game_hWnd = ::FindWindow(0, szWndName);

	}
	if (game_hWnd != NULL)
	{
		game_hWnd = ::GetWindow(game_hWnd, GW_CHILD);
		
		Sleep(1000);
		GetWindowRect(game_hWnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;
	}
	else
	{
	}
	WNDCLASSEXA wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	//trunk�ķ���������Get / SetWindowLong����GWL_USERDATA����,��� ��̬������ʹ��thisָ�������
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);

	srand((unsigned)time(NULL));
	WRand wr;
	int nNameLen = rand() % 20 + 1;
	std::string className = wr.Generate(nNameLen, TRUE, TRUE, FALSE);
	wc.lpszClassName = className.c_str();//�����������
	RegisterClassExA(&wc);
	nNameLen = rand() % 15 + 1;
	std::string wndName = wr.Generate(nNameLen, TRUE, TRUE, FALSE);

	//͸������ WS_EX_LAYERED | WS_EX_TRANSPARENT
	over_hWnd = CreateWindowExA(WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, wndName.c_str(), WS_POPUP, rc.left, rc.top, s_width, s_height, NULL, NULL, wc.hInstance, NULL);

//	SetWindowLong(over_hWnd, GWL_USERDATA, (LONG)this);
	InitD3D();

	SetLayeredWindowAttributes(over_hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(over_hWnd, 0, 255, LWA_ALPHA);
	MARGINS  margin = { -1, -1, -1, -1 }; //���4��ֵ��Ϊ - 1������չ�������ͻ�����
	//MARGINS  margin = { 0,0,s_width,s_height };
	DwmExtendFrameIntoClientArea(over_hWnd, &margin);
	ShowWindow(over_hWnd, SW_SHOW);
	UpdateWindow(over_hWnd);
	
		
}

void DxManager::InitD3D()
{
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);    // ��ȡD3D����ָ��
	if (pD3D == NULL)
	{
		MyOutputDebugStringA("Direct3DCreate9 failed");
		return;
	}
	D3DPRESENT_PARAMETERS d3dpp;    // D3D�豸��Ϣ�ṹ��

	D3DCAPS9 caps; int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		MyOutputDebugStringA("GetDeviceCaps failed");
		return;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //֧��Ӳ���������㣬�Ҳ���Ӳ����������
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //��֧��Ӳ���������㣬���������������

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // ��ʼ��
	d3dpp.Windowed = TRUE;    // ����
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // ��̨���潻����ʽ    ��ת
	d3dpp.hDeviceWindow = over_hWnd;    // D3D���󴰿�
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;     // ��̨�����ʽ  32λ
	d3dpp.BackBufferWidth = s_width;    // ��̨������
	d3dpp.BackBufferHeight = s_height;    // ��̨����߶�   ���ڵ�ǰ�豸�ĸ߶ȿ��

	d3dpp.EnableAutoDepthStencil = TRUE;            //�Զ���Ȼ���
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;       //�Զ���Ȼ����ʽ

	/*d3dpp.BackBufferWidth = s_width;
	d3dpp.BackBufferHeight = s_height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 2;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = over_hWnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;*/

	//����D3D�豸����
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, over_hWnd, vp, &d3dpp, &pDevice)))
	{
		MyOutputDebugStringA("CreateDevice failed");
		return;
	}

	//D3DXCreateFontA(d3ddev, 13, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
	D3DXCreateFontA(pDevice, 13, 0, FW_SEMIBOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "simsun", &pFont);
	D3DXCreateFontA(pDevice, 15, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "simsun", &pFontMenu);
	D3DXCreateLine(pDevice, &pD3DLine);

	
	
}

void DxManager::DrawString(int x, int y, DWORD D3DColor, LPD3DXFONT pUseFont, const char *fmt, ...)
{
	//OutputDebugStringA("DrawString\n");
	char szText[1024] = { 0 };
	va_list va_alist;
	RECT rFontPos = { x, y, x + 120, y + 16 };
	va_start(va_alist, fmt);
	vsprintf_s(szText, fmt, va_alist);
	va_end(va_alist);
	pUseFont->DrawTextA(NULL, szText, -1, &rFontPos, DT_NOCLIP, D3DColor);
}

void DxManager::FillRGB(int x, int y, int w, int h, D3DCOLOR color)
{
	D3DRECT rec = { x, y, x + w, y + h };
	pDevice->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}

void DxManager::DrawPoint(int x, int y, int w, int h, DWORD color)
{
	FillRGB((int)x, (int)y, (int)w, (int)h, color);
}
void DxManager::DrawBox(float x, float y, float width, float height, D3DCOLOR color)
{
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	pD3DLine->SetWidth(1);
	pD3DLine->Draw(points, 5, color);
}

void DxManager::DrawCrosshair()
{
	
	FillRGB(s_width / 2 - 13, s_height / 2, 10, 1, TextYellow);//�����
	FillRGB(s_width / 2 + 4, s_height / 2, 10, 1, TextYellow);//�ұ���
	FillRGB(s_width / 2, s_height / 2 - 13, 1, 10, TextYellow);//�ϱ���
	FillRGB(s_width / 2, s_height / 2 + 4, 1, 10, TextYellow);//�±���

}

void DxManager::DrawBlood(float x, float y, float h, float w, float fBlood)
{
	if (fBlood >70.0)
	{
		FillRGB(x,y,5,h,TextBlack);
		FillRGB(x, y, 5, h*fBlood/100.0, TextGreen);
	}
	if (fBlood >30.0 && fBlood<=70.0)
	{
		FillRGB(x, y, 5, h, TextBlack);
		FillRGB(x, y, 5, h*fBlood / 100.0, TextYellow);
	}
	if (fBlood > 0.0 && fBlood <= 30.0)
	{
		FillRGB(x, y, 5, h, TextBlack);
		FillRGB(x, y, 5, h*fBlood / 100.0, TextRed);
	}
}

void DxManager::DrawCircle(int X, int Y, int radius, int numSides, DWORD Color)
{
	D3DXVECTOR2 Line[128];
	float Step = M_PI * 2.0 / numSides;
	int Count = 0;
	for (float a = 0; a < M_PI*2.0; a += Step)
	{
		float X1 = radius * cos(a) + X;
		float Y1 = radius * sin(a) + Y;
		float X2 = radius * cos(a + Step) + X;
		float Y2 = radius * sin(a + Step) + Y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
	}
	//pD3DLine->Begin();
	pD3DLine->Draw(Line, Count, Color);
	//pD3DLine->End();
}

void DxManager::DrawFPS(int x, int y, DWORD D3DColor, LPD3DXFONT pUseFont)
{
	
	static float  fps = 0; 
	static int    frameCount = 0;
	static float  currentTime = 0.0f;
	static float  lastTime = 0.0f;

	frameCount++;
	currentTime = timeGetTime()*0.001f;


	if (currentTime - lastTime > 1.0f) 
	{
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}
	char szFPS[30] = { 0 };
	sprintf_s(szFPS, "FPS:%0.3f", fps);
	RECT rFontPos = { x, y, x + 120, y + 16 };
	pUseFont->DrawTextA(NULL, szFPS, -1, &rFontPos, DT_NOCLIP, D3DColor);
}

void DxManager::TestDD()
{

	pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	pDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

}

void DxManager::Matrix_Set()
{
	
}
void DxManager::TestBH()
{
	
}