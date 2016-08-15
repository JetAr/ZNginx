#include "ELGUIPCH.h"
#include "ELGUIRenderCache.h"
#include "ELGUISystem.h"

namespace ELGUI
{
	bool RenderCache::hasCachedImagery() const
	{
		return !(d_cachedImages.empty() && d_cachedTexts.empty());
	}

	void RenderCache::render(const Point& basePos, const Rect& clipper)
	{
		Rect displayArea(System::getSingleton().getRenderer()->getRect());
		Rect custClipper;
		const Rect* finalClipper;
		Rect finalRect;

		// Send all cached images to renderer.
		for(ImageryList::const_iterator image = d_cachedImages.begin(); image != d_cachedImages.end(); ++image)
		{
			if ((*image).usingCustomClipper)
			{
				custClipper = (*image).customClipper;
				custClipper.offset(basePos);
				custClipper = (*image).clipToDisplay ? displayArea.getIntersection(custClipper) : clipper.getIntersection(custClipper);
				finalClipper = &custClipper;
			}
			else
			{
				finalClipper = (*image).clipToDisplay ? &displayArea : &clipper;
			}

			finalRect = (*image).target_area;
			finalRect.offset(basePos);
			assert((*image).source_image);
			(*image).source_image->draw(finalRect, *finalClipper, (*image).colours);
		}

		// send all cached texts to renderer.
		for(TextList::const_iterator text = d_cachedTexts.begin(); text != d_cachedTexts.end(); ++text)
		{
			if ((*text).usingCustomClipper)
			{
				custClipper = (*text).customClipper;
				custClipper.offset(basePos);
				custClipper = (*text).clipToDisplay ? displayArea.getIntersection(custClipper) : clipper.getIntersection(custClipper);
				finalClipper = &custClipper;
			}
			else
			{
				finalClipper = (*text).clipToDisplay ? &displayArea : &clipper;
			}

			finalRect = (*text).target_area;
			finalRect.offset(basePos);
			assert((*text).source_font);
			(*text).source_font->drawText((*text).text, finalRect, *finalClipper, (*text).formatting, (*text).colours);
		}
	}

	void RenderCache::clearCachedImagery()
	{
		d_cachedImages.clear();
		d_cachedTexts.clear();
	}

	void RenderCache::cacheImage(const Image& image, const Rect& destArea, const ColourRect& cols, const Rect* clipper, bool clipToDisplay)
	{
		ImageInfo imginf;
		imginf.source_image = &image;
		imginf.target_area  = destArea;
		imginf.colours      = cols;
		imginf.clipToDisplay = clipToDisplay;

		if (clipper)
		{
			imginf.customClipper = *clipper;
			imginf.usingCustomClipper = true;
		}
		else
		{
			imginf.usingCustomClipper = false;
		}

		d_cachedImages.push_back(imginf);
	}

	void RenderCache::cacheText(const std::wstring& text, Font* font, TextFormatting format, const Rect& destArea, const ColourRect& cols, const Rect* clipper /* = 0 */, bool clipToDisplay /* = false */)
	{
		TextInfo txtinf;
		txtinf.text         = text;
		txtinf.source_font  = font;
		txtinf.formatting   = format;
		txtinf.target_area  = destArea;
		txtinf.colours      = cols;
		txtinf.clipToDisplay = clipToDisplay;		

		if (clipper)
		{
			txtinf.customClipper = *clipper;
			txtinf.usingCustomClipper = true;
		}
		else
		{
			txtinf.usingCustomClipper = false;
		}

		d_cachedTexts.push_back(txtinf);
	}
}