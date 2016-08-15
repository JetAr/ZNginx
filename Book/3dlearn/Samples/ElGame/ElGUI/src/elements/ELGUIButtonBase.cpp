#include "ELGUIPCH.h"
#include "ELGUIButtonBase.h"
#include "ELGUIMouseCursor.h"
#include "ELGUISystem.h"

namespace ELGUI
{
	bool ButtonBase::isHovering() const
	{
		return d_hovering;
	}

	bool ButtonBase::isPushed() const
	{
		return d_pushed;
	}

	void ButtonBase::onMouseMove(MouseEventArgs& e)
	{
		// this is needed to discover whether mouse is in the widget area or not.
		// The same thing used to be done each frame in the rendering method,
		// but in this version the rendering method may not be called every frame
		// so we must discover the internal widget state here - which is actually
		// more efficient anyway.

		// base class processing
		Window::onMouseMove(e);

		updateInternalState(e.position);
		e.handled = true;
	}

	void ButtonBase::onMouseButtonDown(MouseEventArgs& e)
	{
		// default processing
		Window::onMouseButtonDown(e);

		if (e.button == LeftButton)
		{
			if (captureInput())
			{
				d_pushed = true;
				updateInternalState(e.position);
				requestRedraw();
			}

			// event was handled by us.
			e.handled = true;
		}
	}

	void ButtonBase::onMouseButtonUp(MouseEventArgs& e)
	{
		// default processing
		Window::onMouseButtonUp(e);

		if (e.button == LeftButton)
		{
			releaseInput();

			// event was handled by us.
			e.handled = true;
		}
	}

	void ButtonBase::onCaptureLost(WindowEventArgs& e)
	{
		// Default processing
		Window::onCaptureLost(e);

		d_pushed = false;
		updateInternalState(MouseCursor::getSingleton().getPosition());
		requestRedraw();

		// event was handled by us.
		e.handled = true;
	}

	void ButtonBase::onMouseLeaves(MouseEventArgs& e)
	{
		// deafult processing
		Window::onMouseLeaves(e);

		d_hovering = false;
		requestRedraw();

		e.handled = true;
	}

	void ButtonBase::updateInternalState(const Point& mouse_pos)
	{
		// This code is rewritten and has a slightly different behaviour
		// it is no longer fully "correct", as overlapping windows will not be
		// considered if the widget is currently captured.
		// On the other hand it's alot faster, so I believe it's a worthy
		// tradeoff

		bool oldstate = d_hovering;

		// assume not hovering 
		d_hovering = false;

		// if input is captured, but not by 'this', then we never hover highlight
		const Window* capture_wnd = getCaptureWindow();
		if (capture_wnd == 0)
		{
			System* sys = System::getSingletonPtr();
			if (sys->getWindowContainingMouse() == this && isHit(mouse_pos))
			{
				d_hovering = true;
			}
		}
		else if (capture_wnd == this && isHit(mouse_pos))
		{
			d_hovering = true;
		}

		// if state has changed, trigger a re-draw
		if (oldstate != d_hovering)
		{
			requestRedraw();
		}
	}
}