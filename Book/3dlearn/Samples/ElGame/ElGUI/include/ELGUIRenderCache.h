#ifndef __ELGUIRenderCache_H__
#define __ELGUIRenderCache_H__

#include "ELGUIVector.h"
#include "ELGUIRect.h"
#include "ELGUIImage.h"
#include "ELGUIFont.h"
#include "ELGUIColourRect.h"
#include <string>
#include <vector>

namespace ELGUI
{
	class RenderCache
	{
	public:
		bool hasCachedImagery() const;
		void render(const Point& basePos, const Rect& clipper);
		void clearCachedImagery();

		void cacheImage(const Image& image, const Rect& destArea, const ColourRect& cols, const Rect* clipper = 0, bool clipToDisplay = false);
		void cacheText(const std::wstring& text, Font* font, TextFormatting format, const Rect& destArea, const ColourRect& cols, const Rect* clipper = 0, bool clipToDisplay = false);

	protected:
		struct ImageInfo
		{
			const Image* source_image;
			Rect target_area;
			ColourRect colours;
			Rect customClipper;
			bool usingCustomClipper;
			bool clipToDisplay;
		};

		struct TextInfo
		{
			std::wstring text;
			Font* source_font;
			TextFormatting formatting;
			Rect target_area;
			ColourRect colours;
			Rect customClipper;
			bool usingCustomClipper;
			bool clipToDisplay;
		};

		typedef std::vector<ImageInfo>	ImageryList;
		typedef std::vector<TextInfo>	TextList;

		ImageryList d_cachedImages;
		TextList d_cachedTexts;
	};
}

#endif //__ELGUIRenderCache_H__