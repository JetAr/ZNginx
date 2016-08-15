#include "ELGUIPCH.h"
#include "ELGUIImageButton.h"

namespace ELGUI
{
	ImageButton::ImageButton(const std::string& type, const std::string& name)
	: ButtonBase(type, name)
	, d_image_normal(0)
	, d_image_hover(0)
	, d_image_pushed(0)
	, d_image_disabled(0)
	{

	}

	void ImageButton::setNormalImage(const Image* img)
	{
		d_image_normal = img;
		requestRedraw();
	}

	void ImageButton::setHoverImage(const Image* img)
	{
		d_image_hover = img;
		requestRedraw();
	}

	void ImageButton::setPushedImage(const Image* img)
	{
		d_image_pushed = img;
		requestRedraw();
	}

	void ImageButton::setDisabledImage(const Image* img)
	{
		d_image_disabled = img;
		requestRedraw();
	}

	void ImageButton::populateRenderCache()
	{
		const Image* image = 0;

		if (!d_enabled)
		{
			image = d_image_disabled;
		}
		else if (d_pushed)
		{
			image = d_image_pushed;
		}
		else if (d_hovering)
		{
			image = d_image_hover;
		}
		else
		{
			image = d_image_normal;
		}

		if (image)
		{
			Rect unclipped_rect = getUnclippedPixelRect();
			d_renderCache.cacheImage(*image, Rect(Point(0.0f, 0.0f), Size(unclipped_rect.getWidth(), unclipped_rect.getHeight())), ColourRect(Colour(1.0f, 1.0f, 1.0f, getEffectiveAlpha())));
		}
	}
}