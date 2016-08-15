#ifndef __ELGUIButtonBase_H__
#define __ELGUIButtonBase_H__

#include "ELGUIWindow.h"
#include "ELGUIWindowFactory.h"

namespace ELGUI
{
	class ButtonBase : public Window
	{
	public:
		ButtonBase(const std::string& type, const std::string& name) : d_pushed(false), d_hovering(false), Window(type, name) {}

		bool isHovering() const;
		bool isPushed()	const;

		virtual void onMouseMove(MouseEventArgs& e);
		virtual void onMouseButtonDown(MouseEventArgs& e);
		virtual void onMouseButtonUp(MouseEventArgs& e);
		virtual void onCaptureLost(WindowEventArgs& e);
		virtual void onMouseLeaves(MouseEventArgs& e);

		void updateInternalState(const Point& mouse_pos);

	protected:
		bool	d_pushed;
		bool	d_hovering;
	};

	ELGUI_DECLARE_WINDOW_FACTORY(ButtonBase);
}

#endif //__ELGUIButtonBase_H__