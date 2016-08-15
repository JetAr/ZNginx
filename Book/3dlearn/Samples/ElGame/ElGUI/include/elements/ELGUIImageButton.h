#ifndef __ELGUIImageButton_H__
#define __ELGUIImageButton_H__

#include "ELGUIButtonBase.h"
#include "ELGUIImage.h"

namespace ELGUI
{
	class ImageButton : public ButtonBase
	{
	public:
		ImageButton(const std::string& type, const std::string& name);

		void setNormalImage(const Image* img);
		void setHoverImage(const Image* img);
		void setPushedImage(const Image* img);
		void setDisabledImage(const Image* img);

	protected:
		virtual void populateRenderCache();

	protected:
		const Image* d_image_normal;
		const Image* d_image_hover;
		const Image* d_image_pushed;
		const Image* d_image_disabled;
	};

	ELGUI_DECLARE_WINDOW_FACTORY(ImageButton);
}

#endif //__ELGUIImageButton_H__