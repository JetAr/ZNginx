#ifndef __ELGUITooltip_H__
#define __ELGUITooltip_H__

#include "ELGUIFrameWindow.h"
#include "ELGUIWindowFactory.h"

namespace ELGUI
{
	class Tooltip : public FrameWindow
	{
	public:
		Tooltip(const std::string& type, const std::string& name);
		virtual ~Tooltip();

		const std::wstring& getText() const;
		void setText(const std::wstring& text);

		void setFont(const std::string& name);
		void setFont(Font* font);
		Font* getFont() const;

		void setTextColours(const ColourRect& colours);
		ColourRect getTextColours() const;

		void setTextFormatting(TextFormatting formatting);
		TextFormatting getTextFormatting() const;

		Size getTextSize() const;

		void sizeSelf();
		void positionSelf();

	protected:
		virtual void populateRenderCache();

	protected:
		std::wstring	d_text;

		Font*			d_font;
		ColourRect      d_textCols;
		TextFormatting	d_formatting;

		float			d_maxWidth;
	};

	ELGUI_DECLARE_WINDOW_FACTORY(Tooltip);
}

#endif //__ELGUITooltip_H__