#include "ELGUIPCH.h"
#include "ELGUIStaticText.h"
#include "ELGUIFontManager.h"

namespace ELGUI
{
	StaticText::StaticText(const std::string& type, const std::string& name)
	: d_text(L"")
	, d_font(0)
	, d_textCols(0xFF000000)
	, d_formatting(WordWrapLeftAligned)
	, Window(type, name)
	{

	}

	const std::wstring& StaticText::getText() const
	{
		return d_text;
	}

	void StaticText::setText(const std::wstring& text)
	{
		d_text = text;
		requestRedraw();
	}

	void StaticText::setFont(const std::string& name)
	{
		if (name.empty())
		{
			setFont(0);
		}
		else
		{
			setFont(FontManager::getSingleton().getFont(name));
		}
	}

	void StaticText::setFont(Font* font)
	{
		d_font = font;
		requestRedraw();
	}

	Font* StaticText::getFont() const
	{
		return d_font;
	}

	void StaticText::setTextColours(const ColourRect& colours)
	{
		d_textCols = colours;
		requestRedraw();
	}

	ColourRect StaticText::getTextColours() const
	{
		return d_textCols;
	}

	void StaticText::setTextFormatting(TextFormatting formatting)
	{
		d_formatting = formatting;
		requestRedraw();
	}

	TextFormatting StaticText::getTextFormatting() const
	{
		return d_formatting;
	}

	void StaticText::populateRenderCache()
	{
		if (!d_text.empty())
		{
			ColourRect final_cols(d_textCols);
			final_cols.modulateAlpha(getEffectiveAlpha());

			Rect unclipped_rect = getUnclippedPixelRect();

			d_renderCache.cacheText(d_text, d_font, d_formatting, Rect(Point(0.0f, 0.0f), Size(unclipped_rect.getWidth(), unclipped_rect.getHeight())), final_cols);
		}
	}
}