#ifndef __ELGUIStaticImage_H__
#define __ELGUIStaticImage_H__

#include "ELGUIWindow.h"
#include "ELGUIWindowFactory.h"
#include "ELGUIImage.h"

namespace ELGUI
{
	class StaticImage : public Window
	{
	public:
		StaticImage(const std::string& type, const std::string& name) : d_image (0), Window(type, name) {}

		const Image* getImage() const;
		void setImage(const Image* img);

	protected:
		virtual void populateRenderCache();

	protected:
		const Image* d_image;
	};

	ELGUI_DECLARE_WINDOW_FACTORY(StaticImage);
}

#endif //__ELGUIStaticImage_H__