#include "ELGUIPCH.h"
#include "ELGUITooltip.h"
#include "ELGUIFontManager.h"
#include "ELGUISystem.h"
#include "ELGUIMouseCursor.h"

namespace ELGUI
{
	Tooltip::Tooltip(const std::string& type, const std::string& name)
	: FrameWindow(type, name)
	, d_text(L"")
	, d_font(0)
	, d_textCols(0xFFFFFFFF)
	, d_formatting(WordWrapLeftAligned)
	, d_maxWidth(240.0f)
	{
		d_mousePassThroughEnabled = true;
		d_alwaysOnTop = true;
	}

	Tooltip::~Tooltip()
	{

	}

	const std::wstring& Tooltip::getText() const
	{
		return d_text;
	}

	void Tooltip::setText(const std::wstring& text)
	{
		d_text = text;
		sizeSelf();
		requestRedraw();
	}

	void Tooltip::setFont(const std::string& name)
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

	void Tooltip::setFont(Font* font)
	{
		d_font = font;
		sizeSelf();
	}

	Font* Tooltip::getFont() const
	{
		return d_font;
	}

	void Tooltip::setTextColours(const ColourRect& colours)
	{
		d_textCols = colours;
		requestRedraw();
	}

	ColourRect Tooltip::getTextColours() const
	{
		return d_textCols;
	}

	void Tooltip::setTextFormatting(TextFormatting formatting)
	{
		d_formatting = formatting;
		sizeSelf();
	}

	TextFormatting Tooltip::getTextFormatting() const
	{
		return d_formatting;
	}

	Size Tooltip::getTextSize() const
	{
		Size texSize(0.0f, 0.0f);

		if (d_font && (!d_text.empty()))
		{
			Rect area(System::getSingleton().getRenderer()->getRect());
			area.setWidth(d_maxWidth);

			float width = PixelAligned(d_font->getFormattedTextExtent(d_text, area, d_formatting));
			float height = PixelAligned(d_font->getFormattedLineCount(d_text, area, d_formatting) * d_font->getLineSpacing());

			return Size(width, height);
		}
		else
		{
			return Size(0.0f, 0.0f);
		}
	}

	void Tooltip::sizeSelf()
	{
		Size textSize(getTextSize());

		textSize.d_width += d_width_left + d_width_right;
		textSize.d_height += d_height_top + d_height_bottom;

		setSize(UVector2(elgui_absdim(textSize.d_width), elgui_absdim(textSize.d_height)));

		updateSectionSizes();
	}

	void Tooltip::positionSelf()
	{
		MouseCursor& cursor = MouseCursor::getSingleton();
		Rect screen(System::getSingleton().getRenderer()->getRect());
		Rect tipRect(getUnclippedPixelRect());

		Point mousePos(cursor.getPosition());
		Size mouseSz(cursor.getSize());

		Point tmpPos(mousePos.d_x + mouseSz.d_width, mousePos.d_y + mouseSz.d_height);
		tipRect.setPosition(tmpPos);

		// if tooltip would be off the right of the screen,
		// reposition to the other side of the mouse cursor.
		if (screen.d_right < tipRect.d_right)
		{
			tmpPos.d_x = mousePos.d_x - tipRect.getWidth() - 5;
		}

		// if tooltip would be off the bottom of the screen,
		// reposition to the other side of the mouse cursor.
		if (screen.d_bottom < tipRect.d_bottom)
		{
			tmpPos.d_y = mousePos.d_y - tipRect.getHeight() - 5;
		}

		// set final position of tooltip window.
		setPosition(UVector2(elgui_absdim(tmpPos.d_x), elgui_absdim(tmpPos.d_y)));
	}

	void Tooltip::populateRenderCache()
	{
		FrameWindow::populateRenderCache();

		if (!d_text.empty())
		{
			ColourRect final_cols(d_textCols);
			final_cols.modulateAlpha(getEffectiveAlpha());

			Rect unclipped_rect = getUnclippedPixelRect();

			d_renderCache.cacheText(d_text, d_font, d_formatting, Rect(Point(d_width_left, d_height_top), Size(unclipped_rect.getWidth(), unclipped_rect.getHeight())), final_cols);
		}
	}
}