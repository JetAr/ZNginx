#include "ELGUIPCH.h"
#include "ELGUIFontManager.h"

namespace ELGUI
{
	template<> FontManager* Singleton<FontManager>::ms_Singleton = 0;

	FontManager::FontManager()
	{

	}

	FontManager::~FontManager()
	{
		destroyAllFonts();
	}

	FontManager& FontManager::getSingleton()
	{
		return Singleton<FontManager>::getSingleton();
	}

	FontManager* FontManager::getSingletonPtr()
	{
		return Singleton<FontManager>::getSingletonPtr();
	}

	void FontManager::registFont(const std::string& name, Font* font)
	{
		if (isFontPresent(name))
			assert(0 && std::string("FontManager::createFont - A font named '" + name + "' already exists.").c_str());
		else
			d_fonts[name] = font;
	}

	void FontManager::destroyAllFonts()
	{
		for (FontRegistry::iterator i = d_fonts.begin(); i != d_fonts.end(); ++i)
		{
			delete i->second;
		}
		d_fonts.clear();
	}

	void FontManager::destroyFont(const std::string& name)
	{
		FontRegistry::iterator pos = d_fonts.find(name.c_str());

		if (pos != d_fonts.end())
		{
			delete pos->second;
			d_fonts.erase(pos);
		}
	}

	bool FontManager::isFontPresent(const std::string& name) const
	{
		return (d_fonts.find(name.c_str()) != d_fonts.end());
	}

	Font* FontManager::getFont(const std::string& name) const
	{
		FontRegistry::const_iterator pos = d_fonts.find(name.c_str());

		if (pos == d_fonts.end())
		{
			assert(0 && std::string("FontManager::getFont - A Font object with the specified name '" + name +"' does not exist within the system").c_str());
		}

		return pos->second;
	}
}