#ifndef __ElGUIHelper_H__
#define __ElGUIHelper_H__

#include <string>

class ElGUIHelper
{
public:
	static std::string WChar2Ascii(LPCWSTR pwszSrc);
	static std::string ws2s(std::wstring& inputws);

	static std::wstring Ascii2WChar(LPCSTR pszSrc, int nLen);
	static std::wstring s2ws(const std::string& s);

	static UINT Virtualkey2scancode(WPARAM wParam, LPARAM lParam); 
};

#endif //__ElGUIHelper_H__