#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 
#pragma comment(lib,"dwmapi.lib")
#pragma comment(lib,"d3d9.lib") 
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "winmm.lib")

//����Բ����
#define M_PI	3.14159265358979323846264338327950288419716939937510

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }      //�Զ���һ��SAFE_RELEASE()��,������Դ���ͷ�

class DxManager
{
public:
	DxManager();
	~DxManager();

	
	HWND game_hWnd;//��Ϸ���ھ��
	HWND over_hWnd;//͸�����Ǵ��ھ��
	int s_width;//���ڴ�С
	int s_height;
	void SetupWindow(LPCTSTR szWndName);

	LPDIRECT3D9 pD3D;
	LPDIRECT3DDEVICE9 pDevice;
	LPD3DXFONT pFont;
	LPD3DXFONT pFontMenu;
	ID3DXLine* pD3DLine;
	void InitD3D();
	void render();
	void DrawString(int x, int y, DWORD D3DColor, LPD3DXFONT pUseFont, const char *fmt, ...);
	void DrawBox(float x, float y, float width, float height, D3DCOLOR color);
	void DrawPoint(int x, int y, int w, int h, DWORD color);
	void DrawCircle(int X, int Y, int radius, int numSides, DWORD Color);
	void FillRGB(int x, int y, int w, int h, D3DCOLOR color);


	void DrawBlood(float x, float y, float h, float w, float fBlood);
	void DrawCrosshair();

	//fps֡��
	void DrawFPS(int x, int y, DWORD D3DColor, LPD3DXFONT pUseFont);
	//���㻺�����
	LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer;    //���㻺��������
	LPDIRECT3DINDEXBUFFER9  g_pIndexBuffer;    // �����������
	void TestDD();
	//�任����
	void Matrix_Set();
	void TestBH();
};
//struct CUSTOMVERTEX
//{
//	FLOAT x, y, z, rhw;
//	DWORD color;
//};
////D3DFVF_XYZRHW����ʾ������������任�Ķ�������ֵ������Direct3D��֪����Щ�������겻��Ҫ�پ�����������任�ˣ����ǵ�����ֵ����������ʾ��Ļ�ϵ�����ֵ�ˡ�
//#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)  //FVF�����ʽ

struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)  //FVF�����ʽ