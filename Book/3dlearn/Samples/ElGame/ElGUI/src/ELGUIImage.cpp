#include "ELGUIPCH.h"
#include "ELGUIImage.h"
#include "ELGUIImageset.h"

namespace ELGUI
{
	Image::Image(const Imageset* owner, const std::string& name, const Rect& area, const Point& render_offset, float horzScaling /* = 1.0f */, float vertScaling /* = 1.0f */)
	: d_owner(owner)
	, d_area(area)
	, d_offset(render_offset)
	, d_name(name)
	{
		assert(d_owner && "Image::Image - Imageset pointer passed to Image constructor must be valid.");

		// setup initial image scaling
		setHorzScaling(horzScaling);
		setVertScaling(vertScaling);
	}

	Image::Image(const Image& image)
	: d_owner(image.d_owner)
	, d_area(image.d_area)
	, d_offset(image.d_offset)
	, d_scaledWidth(image.d_scaledWidth)
	, d_scaledHeight(image.d_scaledHeight)
	, d_scaledOffset(image.d_scaledOffset)
	, d_name(image.d_name)
	{
	}

	Image::~Image()
	{
	}

	void Image::setArea(Rect area)
	{
		d_area = area;
	}

	void Image::setOffset(Point render_offset)
	{
		d_offset = render_offset;
	}

	void Image::setHorzScaling(float factor)
	{
		d_scaledWidth		= PixelAligned(d_area.getWidth() * factor);
		d_scaledOffset.d_x	= PixelAligned(d_offset.d_x * factor);
	}

	void Image::setVertScaling(float factor)
	{
		d_scaledHeight		= PixelAligned(d_area.getHeight() * factor);
		d_scaledOffset.d_y	= PixelAligned(d_offset.d_y * factor);
	}

	void Image::draw(const Rect& dest_rect, const Rect& clip_rect, const ColourRect& colours) const
	{
		Rect dest(dest_rect);

		// apply rendering offset to the destination Rect
		dest.offset(d_scaledOffset);

		// draw
		d_owner->draw(d_area, dest, clip_rect, colours);
	}

	const std::string& Image::getName() const
	{
		return d_name;
	}

	const std::string& Image::getImagesetName() const
	{
		return d_owner->getName();
	}

	const Rect& Image::getSourceTextureArea() const
	{
		return d_area;
	}
}