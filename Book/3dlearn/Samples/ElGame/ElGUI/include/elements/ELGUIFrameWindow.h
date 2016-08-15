#ifndef __ELGUIFrameWindow_H__
#define __ELGUIFrameWindow_H__

#include "ELGUIWindow.h"
#include "ELGUIWindowFactory.h"
#include "ELGUIImage.h"

namespace ELGUI
{
	// window with nine imagery sections
	class FrameWindow : public Window
	{
	public:
		enum Section
		{
			TopLeft		= 0,
			TopMiddle,
			TopRight,
			MiddleLeft,
			MiddleMiddle,
			MiddleRight,
			BottomLeft,
			BottomMiddle,
			BottomRight,
			SectionCount
		};

		FrameWindow(const std::string& type, const std::string& name);
		virtual ~FrameWindow();

		void setSectionImage(Section st, const Image* img);

	protected:
		virtual void populateRenderCache();
		
		virtual void updateSectionSizes();

		float getSectionImageWidth(Section st) const;
		float getSectionImageHeight(Section st) const;

	protected:
		const Image*	d_section_images[SectionCount];

		float			d_width_left;
		float			d_width_middle;
		float			d_width_right;
		float			d_height_top;
		float			d_height_middle;
		float			d_height_bottom;
	};

	ELGUI_DECLARE_WINDOW_FACTORY(FrameWindow);
}

#endif //__ELGUIFrameWindow_H__