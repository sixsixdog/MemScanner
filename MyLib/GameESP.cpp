#include "GameESP.h"
#include "Misc.h"
#include "Define.h"



DxManager *pDxm = NULL;
D3DMenu* pMenu = NULL;
MemoryManager *pMM = NULL;
#define DEBUG

#ifdef DEBUG
int nDebug = 1;
#else
int nDebug = 0;
#endif // 是否显示调试信息



//------全局实时更新的地址-------
DWORD dwJuzhenAddr = 0;
DWORD dwPeople[1000];
DWORD dwPeopleCount;

DWORD dwGoods[2000];
DWORD dwGoodsCount;

DWORD dwCars[1000];
DWORD dwCarsCount;

DWORD MyTeamId;
DWORD dwLocalPlayerAddr;
//------全局实时更新的地址-------


CRITICAL_SECTION k;//定义临界区对象控制CPU
DWORD dwGamePid;


namespace GameData {
	BYTE bJuzhenTag[] = { 0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x00,0x00,
						  0x00,0x00,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,
						  0x80,0x3F };  //+D0
	//BYTE bPeopleTag[] = { 0x64,'?', '?', '?', 0x08,0x00,0x00,0x00,'?', '?','?',0x00,0x00,'?', '?','?', '?','?',0x01,0x00,'?', 0x00,
	//	                  0x00,0x00 };
	BYTE bPeopleTag[] = { 0xC0,'?','?','?',0x08,0x00,0x04,0x00 };
	//                          0x02
	BYTE bLocalPlayerTag[] = { 0x42,'?', '?', '?',0x42,0x00,0x00,0xB0,0x42,0x00,0x00,0xB0,0x42 };
	//BYTE bGoodsTag[] = { 0x9C,'?','?','?', 0x00 ,0x00 ,0x20 ,0x00 };
	//BYTE bGoodsTag[] = { 0x9C,0xdf,'?','?', 0x00 ,0x00 ,0x20 ,0x00 };
	BYTE bGoodsTag[] = { 0x9C,'?','?',0x2a, 0x00 ,0x00 ,0x20 ,0x00 };
	BYTE bCarsTag[] = { 0xEC,'?', '?', '?',0x00,0x00,0x20,0x00 };
	DWORD X_OFFSET = 0x0; //24
	DWORD STATUS_OFFSET = 0xAE0; //24
	DWORD TEAM_OFFSET = 0xED0; //288
// DWORD BLOOD_OFFSET = 0xC58;
	DWORD BLOOD_OFFSET = 0x3D8;
// DWORD POS_OFFSET = 0XEC4;
	DWORD POS_OFFSET = 0x138;
	DWORD RENJI_OFFSET = 0x330;
	
	/*未更新*/
	DWORD B_GOODS_OFFSET = 132;//判断是否为物品
	DWORD GOODS_ID_OFFSET = 272;//物品id偏移
	DWORD GOODS_POS_OFFSET = 336;//物品坐标偏移
}

//功能开关
namespace global
{
	int bEsp = false;
	int bPlayer = false;
	int bVehicle = false;
	int bGoods = false;
	int bAim = false;
	int bAimOpen = false;
	int nAimPos = 0;
	int bAbout = 0;
	int nOther = 0;
	int bSpeed = false;
	int bWeapon = false;
	int bXiGoods = false;
	int bMiniMap = false;
}

const char *opt_Grp[] = { "+", "-" };
const char *opt_OFFON[] = { "关", "开" };
const char *sRadar[] = { "关", "敌人", "所有人" };
const char *sAimSlot[] = { "头部", "颈部", "胸部", "裆部" };
const char *sSpeedHax[] = { "关", "1.5倍", "2倍", "2.5倍" ,"5倍" };

void ShowMenu()
{
	
	if (pMenu == 0) {
		pMenu = new D3DMenu((char*)"菜单选项", 390, 134);
		pMenu->dxm = pDxm;
		pMenu->visible = 1;
		pMenu->col_title = MCOLOR_TITLE;
	}
	else
	{
		pMenu->dxm = pDxm;
		if (pMenu->noitems == 0)
		{
			pMenu->AddGroup((char*)"[透視]", &global::bEsp, (char**)opt_Grp);
			if (global::bEsp)
			{
				pMenu->AddItem((char*)"人物透視", &global::bPlayer, (char**)opt_OFFON);
				pMenu->AddItem((char*)"物品透視", &global::bGoods, (char**)opt_OFFON);
				pMenu->AddItem((char*)"车辆透視", &global::bVehicle, (char**)opt_OFFON);
			}
			pMenu->AddGroup((char*)"[自瞄]", &global::bAim, (char**)opt_Grp);
			if (global::bAim)
			{
				pMenu->AddItem((char*)"自瞄开关", &global::bAimOpen, (char**)opt_OFFON);
				pMenu->AddItem((char*)"自瞄位置", &global::nAimPos, (char**)sAimSlot, 4);
			}
			//其他
			pMenu->AddGroup((char*)"[其他]", &global::nOther, (char**)opt_Grp);
			if (global::nOther)
			{
				pMenu->AddItem((char*)"绘小地图", &global::bMiniMap, (char**)opt_OFFON);
				pMenu->AddItem((char*)"无后坐力", &global::bWeapon, (char**)opt_OFFON);
				pMenu->AddItem((char*)"吸引空投", &global::bXiGoods, (char**)opt_OFFON);
				pMenu->AddItem((char*)"全局加速", &global::bSpeed, (char**)sSpeedHax, 4);
			}
			pMenu->AddGroup((char*)"[关于]", &global::bAbout, (char**)opt_Grp);
			if (global::bAbout)
			{
				pMenu->AddItem((char*)"打印日志", &nDebug, (char**)opt_OFFON);
			}
			
		}
		
		pMenu->Show();
		pMenu->Nav();
	}
}


inline BOOL WorldToScreen2(D3DXVECTOR3 _Enemy, D3DXVECTOR3 &_Screen,int &nLen)
{
	_Screen = D3DXVECTOR3(0, 0, 0);
	float ScreenW = 0;
	D3DMATRIX GameViewMatrix = pMM->RPM<D3DMATRIX>(dwJuzhenAddr, sizeof(D3DMATRIX));
	ScreenW = (GameViewMatrix._14 * _Enemy.x) + (GameViewMatrix._24* _Enemy.y) + (GameViewMatrix._34 * _Enemy.z + GameViewMatrix._44);
	nLen = (int)ScreenW / 100;
	if (ScreenW < 0.0001f)
	{ 
		return FALSE;
	}
		

	float ScreenY = (GameViewMatrix._12 * _Enemy.x) + (GameViewMatrix._22 * _Enemy.y) + (GameViewMatrix._32 * (_Enemy.z+85) + GameViewMatrix._42);
	float ScreenX = (GameViewMatrix._11 * _Enemy.x) + (GameViewMatrix._21 * _Enemy.y) + (GameViewMatrix._31 * _Enemy.z + GameViewMatrix._41);

	_Screen.y = (pDxm->s_height / 2) - (pDxm->s_height / 2) * ScreenY / ScreenW;
	_Screen.x = (pDxm->s_width / 2) + (pDxm->s_width / 2) * ScreenX / ScreenW;
	float y1 = (pDxm->s_height / 2) - (GameViewMatrix._12*_Enemy.x + GameViewMatrix._22 * _Enemy.y +GameViewMatrix._32 *(_Enemy.z -95 ) +GameViewMatrix._42) *(pDxm->s_height / 2)/ ScreenW;
	_Screen.z=y1 - _Screen.y;
	return TRUE;
}
//世界坐标转视窗坐标
inline BOOL WorldToScreenGoods(D3DXVECTOR3 _Enemy, D3DXVECTOR3 &_Screen, int &nLen)
{
	//观察点
	_Screen = D3DXVECTOR3(0, 0, 0);
	float ScreenW = 0;
	//获取矩阵
	D3DMATRIX GameViewMatrix = pMM->RPM<D3DMATRIX>(dwJuzhenAddr, sizeof(D3DMATRIX));
	
	ScreenW = (GameViewMatrix._14 * _Enemy.x) + (GameViewMatrix._24* _Enemy.y) + (GameViewMatrix._34 * _Enemy.z + GameViewMatrix._44);
	//换算屏幕比例
	nLen = (int)ScreenW / 100;
	//过滤极小错误值
	if (ScreenW < 0.0001f)
	{
		return FALSE;
	}

	float ScreenY = (GameViewMatrix._12 * _Enemy.x) + (GameViewMatrix._22 * _Enemy.y) + (GameViewMatrix._32 * (_Enemy.z + 5) + GameViewMatrix._42);
	float ScreenX = (GameViewMatrix._11 * _Enemy.x) + (GameViewMatrix._21 * _Enemy.y) + (GameViewMatrix._31 * _Enemy.z + GameViewMatrix._41);
	//横纵坐标
	_Screen.y = (pDxm->s_height / 2) - (pDxm->s_height / 2) * ScreenY / ScreenW;
	_Screen.x = (pDxm->s_width / 2) + (pDxm->s_width / 2) * ScreenX / ScreenW;
	float y1 = (pDxm->s_height / 2) - (GameViewMatrix._12*_Enemy.x + GameViewMatrix._22 * _Enemy.y + GameViewMatrix._32 *(_Enemy.z) + GameViewMatrix._42) *(pDxm->s_height / 2) / ScreenW;
	//深度坐标
	_Screen.z = y1 - _Screen.y;
	return TRUE;
}

int initia = 0;
//矩阵与对象更新
DWORD WINAPI ThreadUpdateData(LPVOID p)
{
	while (1)
	{
		if (initia == 0)
		{
			//第一次运行时初始化临界线程锁
			InitializeCriticalSection(&k);
			initia++;
		}
		//矩阵对象不存在或者矩阵失效时重新获取矩阵
		if (dwJuzhenAddr == 0 || pMM->RPM<float>(dwJuzhenAddr + 56, 4) != 3)
		{
			std::vector<DWORD_PTR> vAddr;
		    //pMM->MemSearch(GameData::bJuzhenTag, sizeof(GameData::bJuzhenTag), 0x40000000, 0x7fffffff, FALSE, 1, vAddr);
			pMM->MemSearch(GameData::bJuzhenTag, sizeof(GameData::bJuzhenTag), 0x00000000, 0x7fffffff, FALSE, 1, vAddr);
			
			//vAddr = AobScan::FindSigX32(dwGamePid, "ABAAAA3E00000080000000800000803F00000080618B983F", 0x40000000, 0x7fffffff);
			std::vector<DWORD_PTR>::iterator it;

			//验证结果
			for (it = vAddr.begin(); it != vAddr.end(); it++)
			{
				float dmTmp = pMM->RPM<float>(*it + 0xd0 +0x38, sizeof(float));
				//ViewWorld.m[0][3]
				if (dmTmp == 3||dmTmp==380)
				{
					dwJuzhenAddr = *it + 0xd0;
					break;
				}
			}
#ifdef DEBUG
			printf("矩阵地址：%x \n", dwJuzhenAddr);
#endif // DEBUG
		}
		//更新对象
		std::vector<DWORD_PTR> vPeople;
																																			//0x00327000
		pMM->MemSearch(GameData::bPeopleTag, sizeof(GameData::bPeopleTag), 0x00000000, 0xdfffffff, FALSE, 0, vPeople);
		//vPeople = AobScan::FindSigX32(dwGamePid, "fcd7????00002000", 0x10000000, 0x7fffffff);
	//	LeaveCriticalSection(&k);//离开临界区
		dwPeopleCount = vPeople.size();

#ifdef DEBUG
		printf("更新对象 \n");
#endif // DEBUG

		for (unsigned int i=0;i< dwPeopleCount;i++)
		{
			dwPeople[i] = vPeople[i]+ GameData::X_OFFSET;
#ifdef DEBUG
			printf("对象%d号：%x \n",i, dwPeople[i]);
#endif // DEBUG

		}

		

		Sleep(1000* UpdateTime);//更新数据的时间
	}
	return 1;
}

DWORD WINAPI ThreadUpdateData2(LPVOID p)
{
	while (1)
	{
		//更新物品
		std::vector<DWORD_PTR> vGoods;

	
		//EnterCriticalSection(&k);
		pMM->MemSearch(GameData::bGoodsTag, sizeof(GameData::bGoodsTag), 0x00327000, 0x7fffffff, FALSE, 0, vGoods);
		//vGoods = AobScan::FindSigX32(dwGamePid, "9CDF????00002000", 0x10000000, 0x7fffffff);
#ifdef DEBUG
		printf("更新物品 \n");
#endif
		
		//LeaveCriticalSection(&k);//离开临界区

		dwGoodsCount = vGoods.size();
		for (unsigned int i = 0; i < dwGoodsCount; i++)
		{
			dwGoods[i] = vGoods[i];
		}
#ifdef DEBUG
		printf("物品数目 %d\n", dwGoodsCount);
#endif
		Sleep(1000 * UpdateTime);
	}
}

DWORD WINAPI ThreadUpdateData3(LPVOID p)
{
	while (1)
	{
		//更新车辆
		std::vector<DWORD_PTR> vCars;

		//EnterCriticalSection(&k);
		pMM->MemSearch(GameData::bCarsTag, sizeof(GameData::bCarsTag), 0x00327000, 0x7fffffff, FALSE, 0, vCars);
		//vGoods = AobScan::FindSigX32(dwGamePid, "9CDF????00002000", 0x10000000, 0x7fffffff);
#ifdef DEBUG
		printf("更新车辆 \n");
#endif
		//LeaveCriticalSection(&k);//离开临界区

		dwCarsCount = vCars.size();
		for (unsigned int i = 0; i < dwCarsCount; i++)
		{
			dwCars[i] = vCars[i];
		}
#ifdef DEBUG
		printf("车辆数目 %d\n", dwCarsCount);
#endif
		Sleep(1000 * UpdateTime);
	}
}
//计算距离
FLOAT GetDistance(FLOAT X, FLOAT Y, FLOAT X1, FLOAT Y1)
{
	FLOAT XX, YY;
	XX = X - X1;
	YY = Y - Y1;
	return (float)sqrt(XX * XX + YY * YY);
}
//锁定
BOOL bLockAim = FALSE;
float fMin = 0.0f;
float g_AimX = 0.0f;
float g_AimY = 0.0f;


void ESPWork()
{
#pragma region 菜单处理
	ShowMenu();
#pragma endregion 

#pragma region FPS准心等杂项
	pDxm->DrawFPS(pDxm->s_width-320,5,D3DCOLOR_XRGB(168, 39, 136),pDxm->pFont);
	//pDxm->DrawCrosshair();
#pragma endregion 

#pragma region 主要功能

	int nRealPeopleCount = 0;
	fMin = 0.0f;
	g_AimX = 0.0f;
	g_AimY = 0.0f;

	for (unsigned int i = 0;i<dwPeopleCount;i++)
	{
		float fBlood = pMM->RPM<float>(dwPeople[i] + GameData::BLOOD_OFFSET, sizeof(float));
		float fBlood2 = pMM->RPM<float>(dwPeople[i] + 0x6b0, sizeof(float));
		if ((fabs(fBlood) > 1e-5 && fBlood > 0.0)||(fabs(fBlood2) > 1e-5 && fBlood > 0.0))//大于0
		{

			DWORD dwTeamId = pMM->RPM<DWORD_PTR>(dwPeople[i] + GameData::TEAM_OFFSET, sizeof(DWORD_PTR));
	
// 			if(dwTeamId<0x10)
// 			{
// 				continue;
// 			}

			//队伍ID未更新 直接输出
			nRealPeopleCount++;
			D3DXVECTOR3 vPos = pMM->RPM<D3DXVECTOR3>(dwPeople[i] + GameData::POS_OFFSET, sizeof(D3DXVECTOR3));


			
			
			D3DXVECTOR3 vret;
			int nLen = 0;

			if (WorldToScreen2(vPos, vret, nLen))
			{	
				if((nLen-1)>700) continue;
								
				DWORD dwRenji = pMM->RPM<DWORD>(dwPeople[i] + GameData::RENJI_OFFSET, sizeof(DWORD));

				DWORD dwStatus = pMM->RPM<DWORD>(dwPeople[i] + GameData::STATUS_OFFSET, sizeof(DWORD));
				if (nDebug)
				{
					if (nLen-1==1)
					{
					//	printf("%d %x %x\n", dwStatus,dwTeamId,dwPeople[i]);
					}

				}
				float x = vret.x;
				float y = vret.y;
				float h = vret.z;
				float w = vret.z / 2;

				float aimx = vret.x;
				float aimy = vret.y + vret.z / 4;
				

				if (dwStatus == 1 || dwStatus == 4 )
				{
					y = vret.y+ vret.z / 5;
					h = h - vret.z / 5;
					aimy = aimy + vret.z / 6;
				}
				if (dwStatus == 2 || dwStatus == 5 || dwStatus == 7)
				{
					y = vret.y + vret.z / 4;
					h = h - vret.z / 4;
					aimx = aimx + vret.z / 10;
					aimy = aimy + vret.z / 4;
				}
				if (global::bAimOpen)
				{
					//if (bLockAim == FALSE)//没锁定的时候再计算最小屏幕距离
					if(1)
					{
						FLOAT CurrentDistance = (float)GetDistance(pDxm->s_width / 2.0, pDxm->s_height / 2.0, aimx, aimy);
						if (fMin == 0)
						{
							fMin = CurrentDistance;
							g_AimX = aimx;
							g_AimY = aimy;
						}
						else
						{
							if (CurrentDistance < fMin)
							{
								fMin = CurrentDistance;
								g_AimX = aimx;
								g_AimY = aimy;
							}
						}
					}
				}
				
				//pDxm->DrawPoint(aimx, aimy, 10, 10, TextBlack);

				if (dwRenji == 0)//是人机
				{
					pDxm->DrawString((int)x , (int)y, TextBlue, pDxm->pFont, "[%dm]", nLen - 1);
#ifdef DEBUG
					pDxm->DrawString((int)x, (int)y+15, TextGreen, pDxm->pFont, "人物地址:[0x%x]", dwPeople[i]);
					//人物地址
					pDxm->DrawString((int)x, (int)y + 30, TextCyan, pDxm->pFont, "队伍ID:[%d]", dwTeamId);
					//队伍ID
					pDxm->DrawString((int)x, (int)y + 50, TextPink, pDxm->pFont, "x:[%f]", vPos.x);
					pDxm->DrawString((int)x, (int)y + 60, TextGreen, pDxm->pFont, "y:[%f]", vPos.y);
					pDxm->DrawString((int)x, (int)y + 70, TextBlue, pDxm->pFont, "z:[%f]", vPos.z);
					//坐标
#endif 
				}
				else
				{
					pDxm->DrawString((int)x,(int)y, TextPink, pDxm->pFont, "[%dm]", nLen - 1);
#ifdef DEBUG
					pDxm->DrawString((int)x, (int)y + 15, TextGreen, pDxm->pFont, "人物地址:[0x%x]", dwPeople[i]);
					//人物地址
					pDxm->DrawString((int)x, (int)y + 30, TextCyan, pDxm->pFont, "队伍ID:[%d]", dwTeamId);
					//队伍ID
					pDxm->DrawString((int)x, (int)y + 50, TextPink, pDxm->pFont, "x:[%f]", vPos.x);
					pDxm->DrawString((int)x, (int)y + 60, TextGreen, pDxm->pFont, "y:[%f]", vPos.y);
					pDxm->DrawString((int)x, (int)y + 70, TextBlue, pDxm->pFont, "z:[%f]", vPos.z);
					//坐标
#endif 
				}
				pDxm->DrawBox(x - vret.z / 4, y, w, h, TextWhite);
				pDxm->DrawBlood(x - vret.z / 4 - 5, y, h, w, fBlood);				

			}
		}
	}
	if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && global::bAimOpen)
	//if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 && global::bAimOpen)
	{
		bLockAim = TRUE;
		float boneX = (float)g_AimX - pDxm->s_width / 2.0;
		float boneY = (float)g_AimY - pDxm->s_height / 2.0;
		
		if (boneX >= -60.0 && boneX <= 60.0 && boneY >= -60.0 && boneY <= 60.0)
		{
			mouse_event(MOUSEEVENTF_MOVE, boneX/5, boneY/5, NULL, NULL);//除以5因为游戏中移动这个相对距离的话实际上会有更大的偏差
		}
	}
	else
	{
		bLockAim = FALSE;
		g_AimX = 0;
		g_AimY = 0;
	}

	pDxm->DrawString(pDxm->s_width/2-300, pDxm->s_height/11, DARKORANGE, pDxm->pFont, "附近存在[%d]敌人", nRealPeopleCount/2);
#pragma endregion

 #pragma region 物品
 	if (global::bGoods)
 	{
 		int nNowGoodCount = 0;
 		for (size_t i = 0; i < dwGoodsCount; i++)
 		{
 			DWORD_PTR t1 = pMM->RPM<DWORD_PTR>(dwGoods[i] + GameData::B_GOODS_OFFSET, sizeof(DWORD_PTR));
 			//if (t1 == 0xa8a0613)
 			if(1)
 			{
 				nNowGoodCount++;
 				int nId = pMM->RPM<int>(dwGoods[i] + GameData::GOODS_ID_OFFSET, sizeof(int));
 
 				char szGoodName[20] = { 0 };
 				switch (nId)
 				{
 				case 1098798372:
 					strcat_s(szGoodName, "三级包");
 					break;
 				case 1078228938:
 					strcat_s(szGoodName, "SCAR-L");
 					break;
 				case 1070342297:
 					strcat_s(szGoodName, "M416");
 					break;
 				case 1075583328:
 					strcat_s(szGoodName, "QBZ");
 					break;
 				case 1075923558:
 					strcat_s(szGoodName, "SKS");
 					break;
 				case 1081607025:
 					strcat_s(szGoodName, "SLR");
 					break;
 				case 1084841537:
 					strcat_s(szGoodName, "三级甲");
 					break;
 				case 1103479324:
 					strcat_s(szGoodName, "三级头");
 					break;
 				case 1089312141:
 					strcat_s(szGoodName, "八倍镜");
 					break;
 				case 1090071692:
 					strcat_s(szGoodName, "六倍镜");
 					break;
 				case 1091232615:
 					strcat_s(szGoodName, "四倍镜");
 					break;
 				case 1080193518:
 					strcat_s(szGoodName, "7.62mm");
 					break;
 				case 1079905560:
 					strcat_s(szGoodName, "5.56mm");
 					break;
 				default:
 					break;
 				}
 				if (szGoodName[0] == 0)
 				{
 					break;
 				}
 				printf("%s\n",szGoodName);
 
 				D3DXVECTOR3 vPosGoods = pMM->RPM<D3DXVECTOR3>(dwGoods[i] + GameData::GOODS_POS_OFFSET, sizeof(D3DXVECTOR3));
 				D3DXVECTOR3 vret;
 				int nLen = 0;
 
 				if (WorldToScreenGoods(vPosGoods, vret, nLen))
 				{
 					printf("draw %f %f %d\n",vret.x,vret.y,nLen);
 					pDxm->DrawString(vret.x, vret.y, TextRed, pDxm->pFont, "%s[%dm]", szGoodName, nLen);
 				}
 			}
 		}
 
 	}
 	
 #pragma endregion
 #pragma region 车辆
 	if (global::bVehicle)
 	{
 		for (size_t i = 0; i < dwCarsCount; i++)
 		{
 			D3DXVECTOR3 vPosCars = pMM->RPM<D3DXVECTOR3>(dwCars[i] + GameData::GOODS_POS_OFFSET, sizeof(D3DXVECTOR3));
 			D3DXVECTOR3 vret;
 			int nLen = 0;
 			if (WorldToScreenGoods(vPosCars, vret, nLen))
 			{
 				if (nLen>0 && nLen<1000)
 				{
 					pDxm->DrawString(vret.x, vret.y, TextCyan, pDxm->pFont, "座驾[%dm]", nLen);
 				}
 				
 			}
 		}
 	}
 	
 #pragma endregion


}