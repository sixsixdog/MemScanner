#pragma once
#include "DxManager.h"


// ������ɫ
#define DARKORANGE  D3DCOLOR_ARGB(255,255,127,000)
#define TextWhite   D3DCOLOR_ARGB(255,255,255,255) 
#define TextRed     D3DCOLOR_ARGB(255,255,0,0) 
#define TextGreen   D3DCOLOR_ARGB(255,0,255,0)
#define TextBlue    D3DCOLOR_ARGB(255,0,0,255)
#define TextBlack   D3DCOLOR_ARGB(255,0,0,0)
#define TextPurple  D3DCOLOR_ARGB(255,125,0,255)
#define TextGrey    D3DCOLOR_ARGB(255,128,128,128)
#define TextYellow  D3DCOLOR_ARGB(255,255,255,0)
#define TextOrange  D3DCOLOR_ARGB(255,255,140,0)
#define TextCyan    D3DCOLOR_ARGB(255,0,139,200)
#define TextPink    D3DCOLOR_ARGB(255,255,192,203)

// �˵���ɫ
#define TextBorder  D3DCOLOR_ARGB(255,128,128,128)
#define TextFound   D3DCOLOR_ARGB(255,0,139,200)

#ifndef D3DFONT_RIGHT
#define D3DFONT_RIGHT   0x0008
#endif
#ifndef D3DFONT_SHADOW
#define D3DFONT_SHADOW  0x0010
#endif
#define MENUGROUP       1
#define MENUTEXT        2
#define MENUITEM        3
#define MENUGROUP1      4

#define MCOLOR_TITLE    TextGreen
#define MCOLOR_CURRENT  TextRed  //��ǰѡ�е��е���ɫ 
#define MCOLOR_GROUP    TextYellow //�����ɫ
#define MCOLOR_TEXT     TextWhite//�е�������ɫ
#define MCOLOR_OFF      TextWhite
#define MCOLOR_ON       TextGreen  


typedef struct {
	int  typ;         // �˵�����
	char *txt;    // ��ʾ����
	char **opt;   // ����
	int  *var;    // ״̬����
	int  maxval;    // ���ֵ,  Ĭ�� 1  ��0  ��1
} tMENU;

class D3DMenu 
{
public:
	D3DMenu(char *Name = 0, int maxentries = 130, int maxwidth = 120)
	{
		title = Name;
		maxitems = maxentries;
		cur = noitems = visible = 0;
		x = 5;//�˵�����
		y = 70;//
		totwidth = ofs = maxwidth;
		height = 15;
		titleheight = totheight = height + 4;
		col_title = MCOLOR_TITLE;
		col_group = MCOLOR_GROUP;
		col_text = MCOLOR_TEXT;
		col_off = MCOLOR_OFF;
		col_on = MCOLOR_ON;
		col_current = MCOLOR_CURRENT;
		MENU = (tMENU **)malloc(sizeof(LPVOID) * maxitems);
	
		for (int i = 0; i < maxitems; i++)
			MENU[i] = (tMENU *)malloc(sizeof(tMENU));
	}
	~D3DMenu() {
		for (int i = 0; i < maxitems; i++) free(MENU[i]);
		free(MENU);
	}

	DWORD       col_title;
	DWORD       col_group;
	DWORD       col_group1;
	DWORD       col_text;
	DWORD       col_off;
	DWORD       col_on;
	DWORD       col_current;

	int         x, y;               // �˵�λ��
	int         totwidth, totheight;    // �˵��ֿ�� �ܸ߶�
	int         height;             // һ�в˵��߶�
	int         titleheight;        // �������߶�
	int         ofs;                // �ı����
	// menu vars
	char        *title;             // �˵�����
	int         cur;                //  ѡ�и���
	int         noitems;            // �˵�������
	int         visible;            // 1=�˵��ɼ�

	tMENU       **MENU;
	DxManager *dxm;
	void AddItem( char *txt, int *var, char **opt, int maxvalue = 2, int typ = MENUITEM);
	void AddGroup( char *txt, int *var, char **opt, int maxvalue = 2);
	void AddGroup1( char *txt, int *var, char **opt, int maxvalue = 2);
	void AddText( char *txt, char *opt);
	void Show();
	void Nav(void);

private:
	int         maxitems;
};