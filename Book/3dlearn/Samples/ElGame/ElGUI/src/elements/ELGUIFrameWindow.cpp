#include "ELGUIPCH.h"
#include "ELGUIFrameWindow.h"

namespace ELGUI
{
	FrameWindow::FrameWindow(const std::string& type, const std::string& name)
	: Window(type, name)
	, d_width_left(0.0f)
	, d_width_middle(0.0f)
	, d_width_right(0.0f)
	, d_height_top(0.0f)
	, d_height_middle(0.0f)
	, d_height_bottom(0.0f)
	{
		memset((void*)d_section_images, 0, sizeof(d_section_images));
	}

	FrameWindow::~FrameWindow()
	{

	}

	void FrameWindow::setSectionImage(Section st, const Image* img)
	{
		assert(st >= TopLeft && st < SectionCount);
		d_section_images[st] = img;
		updateSectionSizes();
	}

	void FrameWindow::populateRenderCache()
	{
		Rect unclipped_rect = getUnclippedPixelRect();
		Rect rect(Point(0.0f, 0.0f), Size(unclipped_rect.getWidth(), unclipped_rect.getHeight()));
		ColourRect colours(Colour(1.0f, 1.0f, 1.0f, getEffectiveAlpha()));

		if (d_section_images[TopLeft])
			d_renderCache.cacheImage(*d_section_images[TopLeft], Rect(Point(0.0f, 0.0f), Size(d_width_left, d_height_top)), colours);
		if (d_section_images[TopMiddle])
			d_renderCache.cacheImage(*d_section_images[TopMiddle], Rect(Point(d_width_left, 0.0f), Size(d_width_middle, d_height_top)), colours);
		if (d_section_images[TopRight])
			d_renderCache.cacheImage(*d_section_images[TopRight], Rect(Point(d_width_left + d_width_middle, 0.0f), Size(d_width_right, d_height_top)), colours);
		if (d_section_images[MiddleLeft])
			d_renderCache.cacheImage(*d_section_images[MiddleLeft], Rect(Point(0.0f, d_height_top), Size(d_width_left, d_height_middle)), colours);
		if (d_section_images[MiddleMiddle])
			d_renderCache.cacheImage(*d_section_images[MiddleMiddle], Rect(Point(d_width_left, d_height_top), Size(d_width_middle, d_height_middle)), colours);
		if (d_section_images[MiddleRight])
			d_renderCache.cacheImage(*d_section_images[MiddleRight], Rect(Point(d_width_left + d_width_middle, d_height_top), Size(d_width_right, d_height_middle)), colours);
		if (d_section_images[BottomLeft])
			d_renderCache.cacheImage(*d_section_images[BottomLeft], Rect(Point(0.0f, d_height_top + d_height_middle), Size(d_width_left, d_height_bottom)), colours);
		if (d_section_images[BottomMiddle])
			d_renderCache.cacheImage(*d_section_images[BottomMiddle], Rect(Point(d_width_left, d_height_top + d_height_middle), Size(d_width_middle, d_height_bottom)), colours);
		if (d_section_images[BottomRight])
			d_renderCache.cacheImage(*d_section_images[BottomRight], Rect(Point(d_width_left + d_width_middle, d_height_top + d_height_middle), Size(d_width_right, d_height_bottom)), colours);
	}

	void FrameWindow::updateSectionSizes()
	{
		Size wnd_size = getPixelSize();

		d_width_left = max(getSectionImageWidth(TopLeft), max(getSectionImageWidth(MiddleLeft), getSectionImageWidth(BottomLeft)));
		//d_width_middle = max(getSectionImageWidth(TopMiddle), max(getSectionImageWidth(MiddleMiddle), getSectionImageWidth(BottomMiddle)));
		d_width_right = max(getSectionImageWidth(TopRight), max(getSectionImageWidth(MiddleRight), getSectionImageWidth(BottomRight)));
		float width_middle = wnd_size.d_width - d_width_left - d_width_right;
		d_width_middle = max(width_middle, 0.0f);

		d_height_top = max(getSectionImageHeight(TopLeft), max(getSectionImageHeight(TopMiddle), getSectionImageHeight(TopRight)));
		//d_height_middle = max(getSectionImageHeight(MiddleLeft), max(getSectionImageHeight(MiddleMiddle), getSectionImageHeight(MiddleRight)));
		d_height_bottom = max(getSectionImageHeight(BottomLeft), max(getSectionImageHeight(BottomMiddle), getSectionImageHeight(BottomRight)));
		float height_middle = wnd_size.d_height - d_height_top - d_height_bottom;
		d_height_middle = max(height_middle, 0.0f);
	}

	float FrameWindow::getSectionImageWidth(Section st) const
	{
		return d_section_images[st] ? d_section_images[st]->getWidth() : 0.0f;
	}

	float FrameWindow::getSectionImageHeight(Section st) const
	{
		return d_section_images[st] ? d_section_images[st]->getHeight() : 0.0f;
	}
}