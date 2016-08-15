#include "ELGUIPCH.h"
#include "ELGUIHelper.h"

namespace ELGUI
{
	const std::wstring Helper::DefaultWhitespace(L" \n\t\r");
	const std::wstring Helper::DefaultAlphanumerical(L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
	const std::wstring Helper::DefaultWrapDelimiters(L" \n\t\r");

	std::wstring Helper::getNextWord(const std::wstring& str, std::wstring::size_type start_idx /*= 0*/, const std::wstring& delimiters /*= DefaultWhitespace*/)
	{
		std::wstring::size_type word_len = 0;

		std::wstring::size_type word_end = str.find_first_not_of(DefaultAlphanumerical, start_idx);

		if (word_end == std::wstring::npos)
		{
			word_len = str.length();
		}
		else if (word_end == start_idx)
		{
			word_len = 1;
		}
		else
		{
			word_len = word_end - start_idx;
		}

		return str.substr(start_idx, word_len);
	}

	const Image* Helper::stringToImage(const std::string& str)
	{
		using namespace std;

		// handle empty string case
		if (str.empty())
			return 0;

		char imageSet[128];
		char imageName[128];

		sscanf(str.c_str(), " set:%127s image:%127s", imageSet, imageName);

		const Image* image;

		try
		{
			image = &ImagesetManager::getSingleton().getImageset(imageSet)->getImage(imageName);
		}
		catch (...)
		{
			image = 0;
		}

		return image;
	}
}