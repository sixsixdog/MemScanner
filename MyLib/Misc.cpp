#include "Misc.h"

#include "string"
#include <windows.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>

#define MYPRINT

void MyOutputDebugStringA(const char * lpcszOutputString, ...)
{
#ifdef MYPRINT
	std::string strResult;
	if (NULL != lpcszOutputString)
	{
		va_list marker = NULL;
		va_start(marker, lpcszOutputString); //��ʼ����������
		size_t nLength = _vscprintf(lpcszOutputString, marker) + 1; //��ȡ��ʽ���ַ�������
		std::vector<char> vBuffer(nLength, '\0'); //�������ڴ洢��ʽ���ַ������ַ�����
		int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcszOutputString, marker);
		if (nWritten > 0)
		{
			strResult = &vBuffer[0];
		}
		va_end(marker); //���ñ�������
	}
	if (!strResult.empty())
	{
		std::string strFormated = "[wwhtest] ";
		strFormated.append(strResult);
		OutputDebugStringA(strFormated.c_str());
	}
#endif
}

void MyOutputDebugStringW(const wchar_t * lpcwszOutputString, ...)
{
#ifdef MYPRINT
	std::wstring strResult;
	if (NULL != lpcwszOutputString)
	{
		va_list marker = NULL;
		va_start(marker, lpcwszOutputString); //��ʼ����������
		size_t nLength = _vscwprintf(lpcwszOutputString, marker) + 1; //��ȡ��ʽ���ַ�������
		std::vector<wchar_t> vBuffer(nLength, '\0'); //�������ڴ洢��ʽ���ַ������ַ�����
		int nWritten = _vsnwprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcwszOutputString, marker);
		if (nWritten > 0)
		{
			strResult = &vBuffer[0];
		}
		va_end(marker); //���ñ�������
	}
	if (!strResult.empty())
	{
		std::wstring strFormated = L"[wwhtest] ";
		strFormated.append(strResult);
		OutputDebugStringW(strFormated.c_str());
	}
#endif
}