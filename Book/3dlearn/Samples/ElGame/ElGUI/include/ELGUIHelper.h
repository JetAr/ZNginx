#ifndef __ELGUIHelper_H__
#define __ELGUIHelper_H__

#include "ELGUIImageset.h"
#include <string>

namespace ELGUI
{
	class Helper
	{
	public:
		static const std::wstring DefaultWhitespace;
		static const std::wstring DefaultAlphanumerical;
		static const std::wstring DefaultWrapDelimiters;

		static std::wstring getNextWord(const std::wstring& str, std::wstring::size_type start_idx = 0, const std::wstring& delimiters = DefaultWhitespace);

		static const Image*	stringToImage(const std::string& str);
	};
}

#endif //__ELGUIHelper_H__