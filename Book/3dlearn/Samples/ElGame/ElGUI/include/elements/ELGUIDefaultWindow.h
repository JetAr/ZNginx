#ifndef __ELGUIDefaultWindow_H__
#define __ELGUIDefaultWindow_H__

#include "ELGUIWindow.h"
#include "ELGUIWindowFactory.h"

namespace ELGUI
{
	class DefaultWindow : public Window
	{
	public:
		DefaultWindow(const std::string& type, const std::string& name) : Window(type, name) {}
	};

	ELGUI_DECLARE_WINDOW_FACTORY(DefaultWindow);
}

#endif //__ELGUIDefaultWindow_H__