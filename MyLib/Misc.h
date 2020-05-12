#pragma once
#define MYPRINT
#include <Windows.h>
//调试信息输出 任意参数
//MyOutputDebugStringA("%d,%s",123,"hello");
void MyOutputDebugStringA(const char * lpcszOutputString, ...);
//MyOutputDebugStringW(L"%d,%s",456,L"world!");
void MyOutputDebugStringW(const wchar_t * szOutputString, ...);
