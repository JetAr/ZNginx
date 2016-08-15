#ifndef __ELGUIStaticText_H__
#define __ELGUIStaticText_H__

#include "ELGUIWindow.h"
#include "ELGUIWindowFactory.h"
#include "ELGUIFont.h"
#include "ELGUIColourRect.h"
#include <string>

namespace ELGUI
{
	class StaticText : public Window
	{
	public:
		StaticText(const std::string& type, const std::string& name);

		const std::wstring& getText() const;
		void setText(const std::wstring& text);

		void setFont(const std::string& name);
		void setFont(Font* font);
		Font* getFont() const;

		void setTextColours(const ColourRect& colours);
		ColourRect getTextColours() const;

		void setTextFormatting(TextFormatting formatting);
		TextFormatting getTextFormatting() const;

	protected:
		virtual void populateRenderCache();

	protected:
		std::wstring	d_text;
		
		Font*			d_font;
		ColourRect      d_textCols;
		TextFormatting	d_formatting;
	};

	ELGUI_DECLARE_WINDOW_FACTORY(StaticText);
}

#endif //__ELGUIStaticText_H__