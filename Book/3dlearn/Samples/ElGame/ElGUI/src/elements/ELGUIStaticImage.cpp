#include "ELGUIPCH.h"
#include "ELGUIStaticImage.h"
#include "ELGUISystem.h"
#include "ELGUIImageset.h"

namespace ELGUI
{
	const Image* StaticImage::getImage() const
	{
		return d_image;
	}

	void StaticImage::setImage(const Image* img)
	{
		d_image = img;
		requestRedraw();
	}

	void StaticImage::populateRenderCache()
	{
		if (d_image)
		{
			Rect unclipped_rect = getUnclippedPixelRect();
			d_renderCache.cacheImage(*d_image, Rect(Point(0.0f, 0.0f), Size(unclipped_rect.getWidth(), unclipped_rect.getHeight())), ColourRect(Colour(1.0f, 1.0f, 1.0f, getEffectiveAlpha())));
		}
	}
}