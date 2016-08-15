#ifndef __ELGUIFont_H__
#define __ELGUIFont_H__

#include "ELGUIImageset.h"
#include "ELGUISize.h"
#include "ELGUIRect.h"
#include "ELGUIVector.h"
#include "ELGUIColourRect.h"
#include <string>
#include <map>

namespace ELGUI
{
	// Enumerated type that contains valid formatting types that can be specified when rendering text into a Rect area (the formatting Rect).
	enum TextFormatting
	{
		LeftAligned,
		RightAligned,
		Centred,
		Justified,
		WordWrapLeftAligned,
		WordWrapRightAligned,
		WordWrapCentred,
		WordWrapJustified
	};

	class FontGlyph
	{
	public:
		FontGlyph() : d_image(0), d_advance_a(0.0f), d_advance_b(0.0f), d_advance_c(0.0f), d_advance(0.0f) {}
		FontGlyph(const Image *image, float advance_a, float advance_b, float advance_c) : d_image(image), d_advance_a(advance_a), d_advance_b(advance_b), d_advance_c(advance_c)
		{
			d_advance = d_advance_a + d_advance_b + d_advance_c;
		}
		
		const Image* getImage() const
		{
			return d_image;
		}

		const Imageset* getImageset() const
		{
			return d_image ? d_image->getImageset() : 0;
		}

		Size getSize(float x_scale, float y_scale) const
		{
			return Size (getWidth (x_scale), getHeight (y_scale));
		}

		float getWidth(float x_scale) const
		{
			return d_image ? d_image->getWidth () * x_scale : 0.0f;
		}

		float getHeight(float y_scale) const
		{
			return d_image ? d_image->getHeight () * y_scale : 0.0f;
		}

		float getAdvance(float x_scale = 1.0) const
		{
			return d_advance * x_scale;
		}

		float getAdvanceA(float x_scale = 1.0) const
		{
			return d_advance_a * x_scale;
		}

		float getAdvanceB(float x_scale = 1.0) const
		{
			return d_advance_b * x_scale;
		}

		float getAdvanceC(float x_scale = 1.0) const
		{
			return d_advance_c * x_scale;
		}

	protected:
		const Image*	d_image;

		float			d_advance_a;
		float			d_advance_b;
		float			d_advance_c;
		
		float			d_advance;
	};

	class Font
	{
	public:
		Font() : d_height(0.0f) {}
		virtual ~Font() {}

		virtual void load () = 0;

		virtual const FontGlyph* getGlyphData(wchar_t codepoint);
		virtual void rasterize(wchar_t start_codepoint, wchar_t end_codepoint);
		virtual void rasterize(wchar_t codepoint);

		float getLineSpacing(float y_scale = 1.0f) const;
		size_t getCharAtPixel(const std::wstring& text, float pixel, float x_scale = 1.0f);
		size_t getCharAtPixel(const std::wstring& text, size_t start_char, float pixel, float x_scale = 1.0f);
		float getWrappedTextExtent(const std::wstring& text, float wrapWidth, float x_scale = 1.0f);
		float getTextExtent(const std::wstring& text, float x_scale);
		size_t getFormattedLineCount(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale = 1.0f);
		float getFormattedTextExtent(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale = 1.0f);

		size_t getNextWord(const std::wstring& in_string, size_t start_idx, std::wstring& out_string) const;

		size_t drawText(const std::wstring& text, const Rect& draw_area, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);

		void drawTextLine(const std::wstring& text, const Vector2& position, const Rect& clip_rect, const ColourRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);
		void drawTextLineJustified(const std::wstring& text, const Rect& draw_area, const Vector2& position, const Rect& clip_rect, const ColourRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);
		size_t drawWrappedText(const std::wstring& text, const Rect& draw_area, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale = 1.0f, float y_scale = 1.0f);

	protected:
		typedef std::map<wchar_t, FontGlyph> CodepointMap;
		CodepointMap	d_cp_map;

		float			d_height;
	};
}

#endif //__ELGUIFont_H__