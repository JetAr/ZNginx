#include "ElSamplePCH.h"
#include "GUIHelper.h"

std::string ElGUIHelper::WChar2Ascii(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0)
		return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst)
		return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen -1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

std::string ElGUIHelper::ws2s(std::wstring& inputws)
{
	return WChar2Ascii(inputws.c_str());
}

std::wstring ElGUIHelper::Ascii2WChar(LPCSTR pszSrc, int nLen)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if (nSize <= 0)
		return std::wstring(L"");

	WCHAR *pwszDst = new WCHAR[nSize + 1];
	if (NULL == pwszDst)
		return std::wstring(L"");

	MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if (pwszDst[0] == 0xFEFF)                    // skip Oxfeff
		for(int i = 0; i < nSize; ++i) 
			pwszDst[i] = pwszDst[i + 1]; 

	std::wstring wcharString(pwszDst);
	delete[] pwszDst;

	return wcharString;
}

std::wstring ElGUIHelper::s2ws(const std::string& s)
{
	return Ascii2WChar(s.c_str(),s.size());
}

UINT ElGUIHelper::Virtualkey2scancode(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(lParam) & 0x0F00)
	{ 
		UINT scancode = MapVirtualKey(wParam, 0);
		return scancode | 0x80;
	}
	else
	{
		return HIWORD(lParam) & 0x00FF;
	}
}