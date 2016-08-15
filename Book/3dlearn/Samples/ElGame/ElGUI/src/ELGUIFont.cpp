#include "ELGUIPCH.h"
#include "ELGUIFont.h"
#include "ELGUIHelper.h"
#include <algorithm>

namespace ELGUI
{
	const FontGlyph* Font::getGlyphData(wchar_t codepoint)
	{
		rasterize(codepoint, codepoint);

		CodepointMap::const_iterator pos = d_cp_map.find(codepoint);
		return (pos != d_cp_map.end()) ? &pos->second : 0;
	}

	void Font::rasterize(wchar_t start_codepoint, wchar_t end_codepoint)
	{
		for (wchar_t c = start_codepoint; c <= end_codepoint; ++c)
		{
			rasterize(c);
		}
	}

	void Font::rasterize(wchar_t codepoint)
	{

	}

	float Font::getLineSpacing(float y_scale /* = 1.0f */) const
	{
		return d_height * y_scale;
	}

	size_t Font::getCharAtPixel(const std::wstring& text, float pixel, float x_scale /* = 1.0f */)
	{
		return getCharAtPixel(text, 0, pixel, x_scale);
	}

	size_t Font::getCharAtPixel(const std::wstring& text, size_t start_char, float pixel, float x_scale /* = 1.0f */)
	{
		const FontGlyph* glyph;
		float cur_extent = 0;
		size_t char_count = text.length();

		// handle simple cases
		if ((pixel <= 0) || (char_count <= start_char))
			return start_char;

		for (size_t c = start_char; c < char_count; ++c)
		{
			glyph = getGlyphData(text[c]);

			if (glyph)
			{
				cur_extent += glyph->getAdvance(x_scale);

				if (pixel < cur_extent)
					return c;
			}
		}

		return char_count;
	}

	float Font::getWrappedTextExtent(const std::wstring& text, float wrapWidth, float x_scale /* = 1.0f */)
	{
		std::wstring  whitespace = Helper::DefaultWhitespace;
		std::wstring	thisWord;
		size_t	currpos;
		float	lineWidth, wordWidth;
		float	widest = 0;

		// get first word.
		currpos = getNextWord(text, 0, thisWord);
		lineWidth = getTextExtent(thisWord, x_scale);

		// while there are words left in the string...
		while (std::wstring::npos != text.find_first_not_of(whitespace, currpos))
		{
			// get next word of the string...
			currpos += getNextWord(text, currpos, thisWord);
			wordWidth = getTextExtent(thisWord, x_scale);

			// if the new word would make the string too long
			if ((lineWidth + wordWidth) > wrapWidth)
			{
				if (lineWidth > widest)
					widest = lineWidth;

				// remove whitespace from next word - it will form start of next line
				//thisWord = thisWord.substr (thisWord.find_first_not_of (whitespace));
				std::wstring::size_type cutpos = thisWord.find_first_not_of(whitespace);
				if (cutpos != std::wstring::npos)
				{
					thisWord = thisWord.substr(cutpos);
				}

				wordWidth = getTextExtent(thisWord, x_scale);

				// reset for a new line.
				lineWidth = 0;
			}

			// add the next word to the line
			lineWidth += wordWidth;
		}

		if (lineWidth > widest)
			widest = lineWidth;

		return widest;
	}

	float Font::getTextExtent(const std::wstring& text, float x_scale)
	{
		const FontGlyph* glyph;
		float cur_extent = 0.0f, width;

		for (size_t c = 0; c < text.length(); ++c)
		{
			glyph = getGlyphData(text[c]);

			if (glyph)
			{
				width = glyph->getAdvance(x_scale);
				cur_extent += width;
			}
		}

		return cur_extent;
	}

	size_t Font::getFormattedLineCount(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale /* = 1.0f */)
	{
		// handle simple non-wrapped cases.
		if ((fmt == LeftAligned) || (fmt == Centred) || (fmt == RightAligned) || (fmt == Justified))
		{
			return std::count(text.begin(), text.end(), L'\n') + 1;
		}

		// handle wraping cases
		size_t lineStart = 0, lineEnd = 0;
		std::wstring	sourceLine;

		float	wrap_width = format_area.getWidth();
		std::wstring  whitespace = Helper::DefaultWhitespace;
		std::wstring	thisLine, thisWord;
		size_t	line_count = 0, currpos = 0;

		while (lineEnd < text.length())
		{
			if ((lineEnd = text.find_first_of('\n', lineStart)) == std::string::npos)
			{
				lineEnd = text.length();
			}

			sourceLine = text.substr(lineStart, lineEnd - lineStart);
			lineStart = lineEnd + 1;

			// get first word.
			currpos = getNextWord(sourceLine, 0, thisLine);

			// while there are words left in the string...
			while (std::wstring::npos != sourceLine.find_first_not_of(whitespace, currpos))
			{
				// get next word of the string...
				currpos += getNextWord(sourceLine, currpos, thisWord);

				// if the new word would make the string too long
				if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width)
				{
					// too long, so that's another line of text
					line_count++;

					// remove whitespace from next word - it will form start of next line
					//thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));
					std::wstring::size_type cutpos = thisWord.find_first_not_of(whitespace);
					if (cutpos != std::wstring::npos)
					{
						thisWord = thisWord.substr(cutpos);
					}

					// reset for a new line.
					thisLine.clear();
				}

				// add the next word to the line
				thisLine += thisWord;
			}

			// plus one for final line
			line_count++;
		}

		return line_count;
	}

	float Font::getFormattedTextExtent(const std::wstring& text, const Rect& format_area, TextFormatting fmt, float x_scale /* = 1.0f */)
	{
		float lineWidth;
		float widest = 0;

		size_t lineStart = 0, lineEnd = 0;
		std::wstring	currLine;

		while (lineEnd < text.length())
		{
			if ((lineEnd = text.find_first_of('\n', lineStart)) == std::wstring::npos)
			{
				lineEnd = text.length();
			}

			currLine = text.substr(lineStart, lineEnd - lineStart);
			lineStart = lineEnd + 1;	// +1 to skip \n char

			switch(fmt)
			{
			case Centred:
			case RightAligned:
			case LeftAligned:
				lineWidth = getTextExtent(currLine, x_scale);
				break;

			case Justified:
				// usually we use the width of the rect but we have to ensure the current line is not wider than that
				lineWidth = max(format_area.getWidth(), getTextExtent(currLine, x_scale));
				break;

			case WordWrapLeftAligned:
			case WordWrapRightAligned:
			case WordWrapCentred:
				lineWidth = getWrappedTextExtent(currLine, format_area.getWidth(), x_scale);
				break;

			case WordWrapJustified:
				// same as above
				lineWidth = max(format_area.getWidth(), getWrappedTextExtent(currLine, format_area.getWidth(), x_scale));
				break;

			default:
				assert(0 && "Font::getFormattedTextExtent - Unknown or unsupported TextFormatting value specified.");
			}

			if (lineWidth > widest)
			{
				widest = lineWidth;
			}

		}

		return widest;
	}

	size_t Font::getNextWord(const std::wstring& in_string, size_t start_idx, std::wstring& out_string) const
	{
		out_string = Helper::getNextWord(in_string, start_idx, Helper::DefaultWrapDelimiters);

		return out_string.length();
	}

	size_t Font::drawText(const std::wstring& text, const Rect& draw_area, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale /* = 1.0f */, float y_scale /* = 1.0f */)
	{
		size_t thisCount;
		size_t lineCount = 0;

		float y_base = draw_area.d_top;

		Rect tmpDrawArea(
			PixelAligned(draw_area.d_left),
			PixelAligned(draw_area.d_top),
			PixelAligned(draw_area.d_right),
			PixelAligned(draw_area.d_bottom)
			);

		size_t lineStart = 0, lineEnd = 0;
		std::wstring currLine;

		while (lineEnd < text.length())
		{
			if ((lineEnd = text.find_first_of('\n', lineStart)) == std::wstring::npos)
				lineEnd = text.length();

			currLine = text.substr(lineStart, lineEnd - lineStart);
			lineStart = lineEnd + 1;	// +1 to skip \n char

			switch (fmt)
			{
			case LeftAligned:
				drawTextLine(currLine, Vector2(tmpDrawArea.d_left, y_base), clip_rect, colours, x_scale, y_scale);
				thisCount = 1;
				y_base += getLineSpacing(y_scale);
				break;

			case RightAligned:
				drawTextLine(currLine, Vector2(tmpDrawArea.d_right - getTextExtent(currLine, x_scale), y_base), clip_rect, colours, x_scale, y_scale);
				thisCount = 1;
				y_base += getLineSpacing(y_scale);
				break;

			case Centred:
				drawTextLine(currLine, Vector2(PixelAligned(tmpDrawArea.d_left + ((tmpDrawArea.getWidth() - getTextExtent(currLine, x_scale)) / 2.0f)), y_base), clip_rect, colours, x_scale, y_scale);
				thisCount = 1;
				y_base += getLineSpacing(y_scale);
				break;

			case Justified:
				// new function in order to keep drawTextLine's signature unchanged
				drawTextLineJustified(currLine, draw_area, Vector2(tmpDrawArea.d_left, y_base), clip_rect, colours, x_scale, y_scale);
				thisCount = 1;
				y_base += getLineSpacing(y_scale);
				break;

			case WordWrapLeftAligned:
				thisCount = drawWrappedText(currLine, tmpDrawArea, clip_rect, LeftAligned, colours, x_scale, y_scale);
				tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
				break;

			case WordWrapRightAligned:
				thisCount = drawWrappedText(currLine, tmpDrawArea, clip_rect, RightAligned, colours, x_scale, y_scale);
				tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
				break;

			case WordWrapCentred:
				thisCount = drawWrappedText(currLine, tmpDrawArea, clip_rect, Centred, colours, x_scale, y_scale);
				tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
				break;

			case WordWrapJustified:
				// no change needed
				thisCount = drawWrappedText(currLine, tmpDrawArea, clip_rect, Justified, colours, x_scale, y_scale);
				tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
				break;

			default:
				assert(0 && "Font::drawText - Unknown or unsupported TextFormatting value specified.");
			}

			lineCount += thisCount;

		}

		// should not return 0
		return max(lineCount, (size_t)1);
	}

	void Font::drawTextLine(const std::wstring& text, const Vector2& position, const Rect& clip_rect, const ColourRect& colours, float x_scale /* = 1.0f */, float y_scale /* = 1.0f */)
	{
		Point cur_pos(position);

		const FontGlyph* glyph;
		float base_y = position.d_y;

		for (size_t c = 0; c < text.length(); ++c)
		{
			glyph = getGlyphData(text[c]);

			if (glyph)
			{
				cur_pos.d_x += glyph->getAdvanceA(x_scale);

				const Image* img = glyph->getImage();
				if (img)
				{
					cur_pos.d_y = base_y - (img->getOffsetY() - img->getOffsetY() * y_scale);

					// temporarily, we just give each character a one-pixel shadow
					img->draw(Point(cur_pos.d_x + 1.0f, cur_pos.d_y + 1.0f),
						glyph->getSize(x_scale, y_scale), clip_rect,
						ColourRect(Colour(0.0f, 0.0f, 0.0f, colours.d_top_left.getAlpha()),
						Colour(0.0f, 0.0f, 0.0f, colours.d_top_right.getAlpha()),
						Colour(0.0f, 0.0f, 0.0f, colours.d_bottom_left.getAlpha()),
						Colour(0.0f, 0.0f, 0.0f, colours.d_bottom_right.getAlpha())));

					img->draw(cur_pos, glyph->getSize(x_scale, y_scale), clip_rect, colours);
				}

				cur_pos.d_x += glyph->getAdvanceB(x_scale) + glyph->getAdvanceC(x_scale);
			}
		}
	}

	void Font::drawTextLineJustified(const std::wstring& text, const Rect& draw_area, const Vector2& position, const Rect& clip_rect, const ColourRect& colours, float x_scale /* = 1.0f */, float y_scale /* = 1.0f */)
	{
		Vector2	cur_pos(position);

		const FontGlyph* glyph;
		float base_y = position.d_y;
		size_t char_count = text.length();

		// Calculate the length difference between the justified text and the same text, left aligned
		// This space has to be shared between the space characters of the line
		float lost_space = getFormattedTextExtent(text, draw_area, Justified, x_scale) - getTextExtent(text, x_scale);

		// The number of spaces and tabs in the current line
		unsigned int space_count = 0;
		size_t c;
		for (c = 0; c < char_count; ++c)
			if ((text[c] == ' ') || (text[c] == '\t'))
				++space_count;

		// The width that must be added to each space character in order to transform the left aligned text in justified text
		float shared_lost_space = 0.0;
		if (space_count > 0)
			shared_lost_space = lost_space / (float)space_count;

		for (c = 0; c < char_count; ++c)
		{
			glyph = getGlyphData(text[c]);

			if (glyph)
			{
				cur_pos.d_x += glyph->getAdvanceA(x_scale);
				
				const Image* img = glyph->getImage();
				if (img)
				{
					cur_pos.d_y = base_y - (img->getOffsetY() - img->getOffsetY() * y_scale);

					// temporarily, we just give each character a one-pixel shadow
					img->draw(Point(cur_pos.d_x + 1.0f, cur_pos.d_y + 1.0f),
						glyph->getSize(x_scale, y_scale), clip_rect,
						ColourRect(Colour(0.0f, 0.0f, 0.0f, colours.d_top_left.getAlpha()),
						Colour(0.0f, 0.0f, 0.0f, colours.d_top_right.getAlpha()),
						Colour(0.0f, 0.0f, 0.0f, colours.d_bottom_left.getAlpha()),
						Colour(0.0f, 0.0f, 0.0f, colours.d_bottom_right.getAlpha())));

					img->draw(cur_pos, glyph->getSize(x_scale, y_scale), clip_rect, colours);
				}
				
				cur_pos.d_x += glyph->getAdvanceB(x_scale) + glyph->getAdvanceC(x_scale);

				// That's where we adjust the size of each space character
				if ((text[c] == ' ') || (text[c] == '\t'))
					cur_pos.d_x += shared_lost_space;
			}
		}
	}

	size_t Font::drawWrappedText(const std::wstring& text, const Rect& draw_area, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale /* = 1.0f */, float y_scale /* = 1.0f */)
	{
		size_t	line_count = 0;
		Rect	dest_area(draw_area);
		float	wrap_width = draw_area.getWidth();

		std::wstring  whitespace = Helper::DefaultWhitespace;
		std::wstring	thisLine, thisWord;
		size_t	currpos = 0;

		// get first word.
		currpos += getNextWord(text, currpos, thisLine);

		// while there are words left in the string...
		while (std::wstring::npos != text.find_first_not_of(whitespace, currpos))
		{
			// get next word of the string...
			currpos += getNextWord(text, currpos, thisWord);

			// if the new word would make the string too long
			if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width)
			{
				// output what we had until this new word
				line_count += drawText(thisLine, dest_area, clip_rect, fmt, colours, x_scale, y_scale);

				// remove whitespace from next word - it will form start of next line
				//thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));
				std::wstring::size_type cutpos = thisWord.find_first_not_of(whitespace);
				if (cutpos != std::wstring::npos)
				{
					thisWord = thisWord.substr(cutpos);
				}

				// reset for a new line.
				thisLine.clear();

				// update y co-ordinate for next line
				dest_area.d_top += getLineSpacing(y_scale);
			}

			// add the next word to the line
			thisLine += thisWord;
		}

		// Last line is left aligned
		TextFormatting last_fmt = (fmt == Justified ? LeftAligned : fmt);
		// output last bit of string
		line_count += drawText(thisLine, dest_area, clip_rect, last_fmt, colours, x_scale, y_scale);

		return line_count;
	}
}