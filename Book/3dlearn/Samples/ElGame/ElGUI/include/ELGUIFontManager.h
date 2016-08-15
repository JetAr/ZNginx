#ifndef __ELGUIFontManager_H__
#define __ELGUIFontManager_H__

#include "ELGUIFont.h"
#include <string>
#include <map>

namespace ELGUI
{
	class FontManager : public Singleton<FontManager>
	{
	public:
		FontManager();
		virtual ~FontManager();

		static FontManager& getSingleton();
		static FontManager* getSingletonPtr();

		void registFont(const std::string& name, Font* font);

		void destroyAllFonts();
		void destroyFont(const std::string& name);

		bool isFontPresent(const std::string& name) const;
		Font* getFont(const std::string& name) const;

	protected:
		typedef std::map<std::string, Font*> FontRegistry;
		FontRegistry	d_fonts;
	};
}

#endif //__ELGUIFontManager_H__