#include "ELGUIPCH.h"
#include "ELGUISystem.h"
#include "ELGUISingleton.h"
#include "ELGUIImagesetManager.h"
#include "ELGUIFontManager.h"
#include "ELGUIWindowManager.h"
#include "ELGUIWindowFactoryManager.h"
#include "ELGUIBase.h"
#include "ELGUIMouseCursor.h"
#include "ELGUIElements.h"

namespace ELGUI
{
	template<> System* Singleton<System>::ms_Singleton = 0;

	const double System::DefaultSingleClickTimeout = 0.2;
	const double System::DefaultMultiClickTimeout = 0.33;
	const Size System::DefaultMultiClickAreaSize(12, 12);

#if defined(__WIN32__) || defined(_WIN32)
#include "mmsystem.h"
	double SimpleTimer::currentTime()
	{
		return timeGetTime() / 1000.0;
	}

#elif defined(__linux__)
#include <sys/time.h>
	double SimpleTimer::currentTime()
	{
		timeval timeStructure;
		gettimeofday(&timeStructure, 0);
		return timeStructure.tv_sec + timeStructure.tv_usec / 1000000.0;
	}
#endif

	System::System(Renderer* renderer)
	: d_renderer(renderer)
	, d_wndWithMouse(0)
	, d_activeSheet(0)
	, d_modalTarget(0)
	, d_gui_redraw(true)
	, d_sysKeys(0)
	, d_lshift(false)
	, d_rshift(false)
	, d_lctrl(false)
	, d_rctrl(false)
	, d_lalt(false)
	, d_ralt(false)
	, d_click_timeout(DefaultSingleClickTimeout)
	, d_dblclick_timeout(DefaultMultiClickTimeout)
	, d_dblclick_size(DefaultMultiClickAreaSize)
	, d_clickTrackerPimpl(new MouseClickTrackerImpl)
	, d_mouseScalingFactor(1.0f)
	{
		// create the core system singleton objects
		createSingletons();

		// add the window factories for the core window types
		addWindowFactories();
	}

	System::~System()
	{
		// cleanup singletons
		destroySingletons();

		if (d_clickTrackerPimpl)
			delete d_clickTrackerPimpl;
	}

	Window* System::setGUISheet(Window* sheet)
	{
		Window* old = d_activeSheet;
		d_activeSheet = sheet;

		// Force and update for the area Rects for 'sheet' so they're correct according
		// to the screen size.
		if (sheet)
		{
			WindowEventArgs sheetargs(0);
			sheet->onParentSized(sheetargs);
		}

		return old;
	}

	Window* System::getGUISheet() const
	{
		return d_activeSheet;
	}

	void System::renderGUI()
	{
		// This makes use of some tricks the Renderer can do so that we do not
		// need to do a full redraw every frame - only when some UI element has
		// changed.
		// Since the mouse is likely to move very often, and in order not to
		// short-circuit the above optimisation, the mouse is not queued, but is
		// drawn directly to the display every frame.

		if (d_gui_redraw)
		{
			d_renderer->clearRenderList();

			if (d_activeSheet)
			{
				d_activeSheet->render();
			}
			d_gui_redraw = false;
		}

		d_renderer->doRender();
	}

	bool System::injectMouseMove(float delta_x, float delta_y)
	{
		MouseEventArgs ma(0);
		MouseCursor& mouse = MouseCursor::getSingleton();

		ma.moveDelta.d_x = delta_x * d_mouseScalingFactor;
		ma.moveDelta.d_y = delta_y * d_mouseScalingFactor;
		ma.sysKeys = d_sysKeys;
		ma.wheelChange = 0;
		ma.clickCount = 0;
		ma.button = NoButton;

		// move the mouse cursor & update position in args.
		mouse.offsetPosition(ma.moveDelta);
		ma.position = mouse.getPosition();

		Window* dest_window = getTargetWindow(ma.position);

		// has window containing mouse changed?
		if (dest_window != d_wndWithMouse)
		{
			// store previous window that contained mouse
			Window* oldWindow = d_wndWithMouse;
			// set the new window that contains the mouse.
			d_wndWithMouse = dest_window;

			// inform previous window the mouse has left it
			if (oldWindow)
			{
				ma.window = oldWindow;
				oldWindow->onMouseLeaves(ma);
			}

			// inform window containing mouse that mouse has entered it
			if (d_wndWithMouse)
			{
				ma.window = d_wndWithMouse;
				ma.handled = false;
				d_wndWithMouse->onMouseEnters(ma);
			}
		}

		// inform appropriate window of the mouse movement event
		if (dest_window)
		{
			// ensure event starts as 'not handled'
			ma.handled = false;

			// loop backwards until event is handled or we run out of windows.
			while ((!ma.handled) && (dest_window != 0))
			{
				ma.window = dest_window;
				dest_window->onMouseMove(ma);
				dest_window = getNextTargetWindow(dest_window);
			}
		}

		return ma.handled;
	}

	bool System::injectMousePosition(float x_pos, float y_pos)
	{
		// set new mouse position
		MouseCursor::getSingleton().setPosition(Point(x_pos, y_pos));

		// do the real work
		return injectMouseMove(0, 0);
	}

	bool System::injectMouseButtonDown(MouseButton button)
	{
		// update system keys
		d_sysKeys |= mouseButtonToSyskey(button);

		MouseEventArgs ma(0);
		ma.position = MouseCursor::getSingleton().getPosition();
		ma.moveDelta = Vector2(0.0f, 0.0f);
		ma.button = button;
		ma.sysKeys = d_sysKeys;
		ma.wheelChange = 0;

		// find the likely destination for generated events.
		Window* dest_window = getTargetWindow(ma.position);

		//
		// Handling for multi-click generation
		//
		MouseClickTracker& tkr = d_clickTrackerPimpl->click_trackers[button];

		tkr.d_click_count++;

		// if multi-click requirements are not met
		if ((tkr.d_timer.elapsed() > d_dblclick_timeout) ||
			(!tkr.d_click_area.isPointInRect(ma.position)) ||
			(tkr.d_target_window != dest_window) ||
			(tkr.d_click_count > 3))
		{
			// reset to single down event.
			tkr.d_click_count = 1;

			// build new allowable area for multi-clicks
			tkr.d_click_area.setPosition(ma.position);
			tkr.d_click_area.setSize(d_dblclick_size);
			tkr.d_click_area.offset(Point(-(d_dblclick_size.d_width / 2), -(d_dblclick_size.d_height / 2)));

			// set target window for click events on this tracker
			tkr.d_target_window = dest_window;
		}

		// set click count in the event args
		ma.clickCount = tkr.d_click_count;

		// loop backwards until event is handled or we run out of windows.
		while ((!ma.handled) && (dest_window != 0))
		{
			ma.window = dest_window;

			if (dest_window->wantsMultiClickEvents())
			{
				switch (tkr.d_click_count)
				{
				case 1:
					dest_window->onMouseButtonDown(ma);
					break;

				case 2:
					dest_window->onMouseDoubleClicked(ma);
					break;

				case 3:
					dest_window->onMouseTripleClicked(ma);
					break;
				}
			}
			// current target window does not want multi-clicks,
			// so just send a mouse down event instead.
			else
			{
				dest_window->onMouseButtonDown(ma);
			}

			dest_window = getNextTargetWindow(dest_window);
		}

		// reset timer for this tracker.
		tkr.d_timer.restart();

		return ma.handled;
	}

	bool System::injectMouseButtonUp(MouseButton button)
	{
		// update system keys
		d_sysKeys &= ~mouseButtonToSyskey(button);

		MouseEventArgs ma(0);
		ma.position = MouseCursor::getSingleton().getPosition();
		ma.moveDelta = Vector2(0.0f, 0.0f);
		ma.button = button;
		ma.sysKeys = d_sysKeys;
		ma.wheelChange = 0;

		// get the tracker that holds the number of down events seen so far for this button
		MouseClickTracker& tkr = d_clickTrackerPimpl->click_trackers[button];
		// set click count in the event args
		ma.clickCount = tkr.d_click_count;

		Window* const initial_dest_window = getTargetWindow(ma.position);
		Window* dest_window = initial_dest_window;

		// loop backwards until event is handled or we run out of windows.
		while ((!ma.handled) && (dest_window != 0))
		{
			ma.window = dest_window;
			dest_window->onMouseButtonUp(ma);
			dest_window = getNextTargetWindow(dest_window);
		}

		bool wasUpHandled = ma.handled;

		// if requirements for click events are met
		if ((tkr.d_timer.elapsed() <= d_click_timeout) &&
			(tkr.d_click_area.isPointInRect(ma.position)) &&
			(tkr.d_target_window == initial_dest_window))
		{
			ma.handled = false;
			dest_window = initial_dest_window;

			// loop backwards until event is handled or we run out of windows.
			while ((!ma.handled) && (dest_window != 0))
			{
				ma.window = dest_window;
				dest_window->onMouseClicked(ma);
				dest_window = getNextTargetWindow(dest_window);
			}

		}

		return (ma.handled | wasUpHandled);
	}

	bool System::injectMouseWheelChange(float delta)
	{
		MouseEventArgs ma(0);
		ma.position = MouseCursor::getSingleton().getPosition();
		ma.moveDelta = Vector2(0.0f, 0.0f);
		ma.button = NoButton;
		ma.sysKeys = d_sysKeys;
		ma.wheelChange = delta;
		ma.clickCount = 0;

		Window* dest_window = getTargetWindow(ma.position);

		// loop backwards until event is handled or we run out of windows.
		while ((!ma.handled) && (dest_window != 0))
		{
			ma.window = dest_window;
			dest_window->onMouseWheel(ma);
			dest_window = getNextTargetWindow(dest_window);
		}

		return ma.handled;
	}

	bool System::injectMouseSize(float width, float hight)
	{
		// set new mouse cursor size
		MouseCursor::getSingleton().setSize(Size(width, hight));
		return true;
	}

	bool System::injectKeyDown(unsigned int key_code)
	{
		// update system keys
		d_sysKeys |= keyCodeToSyskey((Key::Scan)key_code, true);

		KeyEventArgs args(0);

		if (d_activeSheet)
		{
			args.scancode = (Key::Scan)key_code;
			args.sysKeys = d_sysKeys;

			Window* dest = getKeyboardTargetWindow();

			// loop backwards until event is handled or we run out of windows.
			while ((dest != 0) && (!args.handled))
			{
				args.window = dest;
				dest->onKeyDown(args);
				dest = getNextTargetWindow(dest);
			}

		}

		return args.handled;
	}

	bool System::injectKeyUp(unsigned int key_code)
	{
		// update system keys
		d_sysKeys &= ~keyCodeToSyskey((Key::Scan)key_code, false);

		KeyEventArgs args(0);

		if (d_activeSheet)
		{
			args.scancode = (Key::Scan)key_code;
			args.sysKeys = d_sysKeys;

			Window* dest = getKeyboardTargetWindow();

			// loop backwards until event is handled or we run out of windows.
			while ((dest != 0) && (!args.handled))
			{
				args.window = dest;
				dest->onKeyUp(args);
				dest = getNextTargetWindow(dest);
			}

		}

		return args.handled;
	}

	bool System::injectChar(unsigned int code_point)
	{
		// whether is a normal character
		if (d_sysKeys & Control)
		{
			return false;
		}

		KeyEventArgs args(0);

		if (d_activeSheet)
		{
			args.codepoint = code_point;
			args.sysKeys = d_sysKeys;

			Window* dest = getKeyboardTargetWindow();

			// loop backwards until event is handled or we run out of windows.
			while ((dest != 0) && (!args.handled))
			{
				args.window = dest;
				dest->onCharacter(args);
				dest = getNextTargetWindow(dest);
			}

		}

		return args.handled;
	}

	SystemKey System::mouseButtonToSyskey(MouseButton btn) const
	{
		switch (btn)
		{
		case LeftButton:
			return LeftMouse;

		case RightButton:
			return RightMouse;

		case MiddleButton:
			return MiddleMouse;

		case X1Button:
			return X1Mouse;

		case X2Button:
			return X2Mouse;

		default:
			assert(0 && "System::mouseButtonToSyskey - the parameter 'btn' is not a valid MouseButton value.");
		}
	}

	SystemKey System::keyCodeToSyskey(Key::Scan key, bool direction)
	{
		switch (key)
		{
		case Key::LeftShift:
			d_lshift = direction;

			if (!d_rshift)
			{
				return Shift;
			}
			break;

		case Key::RightShift:
			d_rshift = direction;

			if (!d_lshift)
			{
				return Shift;
			}
			break;


		case Key::LeftControl:
			d_lctrl = direction;

			if (!d_rctrl)
			{
				return Control;
			}
			break;

		case Key::RightControl:
			d_rctrl = direction;

			if (!d_lctrl)
			{
				return Control;
			}
			break;

		case Key::LeftAlt:
			d_lalt = direction;

			if (!d_ralt)
			{
				return Alt;
			}
			break;

		case Key::RightAlt:
			d_ralt = direction;

			if (!d_lalt)
			{
				return Alt;
			}
			break;

		default:
			break;
		}

		// if not a system key or overall state unchanged, return 0.
		return (SystemKey)0;
	}

	unsigned int System::getSystemKeys() const
	{
		return d_sysKeys;
	}

	void System::notifyWindowDestroyed(const Window* window)
	{
		if (d_wndWithMouse == window)
		{
			d_wndWithMouse = 0;
		}

		if (d_activeSheet == window)
		{
			d_activeSheet = 0;
		}

		if (d_modalTarget == window)
		{
			d_modalTarget = 0;
		}
	}

	void System::setModalTarget(Window* target)
	{
		d_modalTarget = target;
	}

	Window* System::getWindowContainingMouse() const
	{
		return d_wndWithMouse;
	}

	Window* System::getModalTarget() const
	{
		return d_modalTarget;
	}

	Window* System::getFocusWindow()
	{
		return getKeyboardTargetWindow();
	}

	Window* System::getTargetWindow(const Point& pt) const
	{
		Window* dest_window = 0;

		// if there is no GUI sheet, then there is nowhere to send input
		if (d_activeSheet)
		{
			dest_window = Window::getCaptureWindow();

			if (!dest_window)
			{
				dest_window = d_activeSheet->getTargetChildAtPosition(pt);

				if (!dest_window)
				{
					dest_window = d_activeSheet;
				}

			}
			else
			{
				if (dest_window->distributesCapturedInputs())
				{
					Window* child_window = dest_window->getTargetChildAtPosition(pt);

					if (child_window)
					{
						dest_window = child_window;
					}

				}

			}

			// modal target overrules
			if (d_modalTarget != 0 && dest_window != d_modalTarget)
			{
				if (!dest_window->isAncestor(d_modalTarget))
				{
					dest_window = d_modalTarget;
				}

			}

		}

		return dest_window;
	}

	Window* System::getKeyboardTargetWindow() const
	{
		Window* target = 0;

		if (!d_modalTarget)
		{
			target = d_activeSheet->getActiveChild();
		}
		else
		{
			target = d_modalTarget->getActiveChild();
			if (!target)
			{
				target = d_modalTarget;
			}
		}

		return target;
	}

	Renderer* System::getRenderer() const
	{
		return d_renderer;
	}

	bool System::handleDisplaySizeChange()
	{
		// notify the imageset manager of the size change
		Size new_sz = getRenderer()->getSize();
		ImagesetManager::getSingleton().notifyScreenResolution(new_sz);

		// notify gui sheet / root if size change, event propagation will ensure everything else
		// gets updated as required.
		if (d_activeSheet)
		{
			WindowEventArgs args(0);
			d_activeSheet->onParentSized(args);
		}

		return true;
	}

	void System::signalRedraw()
	{
		d_gui_redraw = true;
	}

	System& System::getSingleton()
	{
		return Singleton<System>::getSingleton();
	}

	System* System::getSingletonPtr()
	{
		return Singleton<System>::getSingletonPtr();
	}

	void System::createSingletons()
	{
		// cause creation of other singleton objects
		new ImagesetManager();
		new FontManager();
		new WindowFactoryManager();
		new WindowManager();
		new MouseCursor();
	}

	void System::destroySingletons()
	{
		delete WindowManager::getSingletonPtr();
		delete WindowFactoryManager::getSingletonPtr();
		delete FontManager::getSingletonPtr();
		delete MouseCursor::getSingletonPtr();
		delete ImagesetManager::getSingletonPtr();
	}

	void System::addWindowFactories()
	{
		// Add factories for types all windows
		WindowFactoryManager& wfMgr = WindowFactoryManager::getSingleton();
		wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(DefaultWindow));
		wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(StaticImage));
		wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(StaticText));
		wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(ButtonBase));
		wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(ImageButton));
		wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(FrameWindow));
		wfMgr.addFactory(ELGUI_CREATE_WINDOW_FACTORY(Tooltip));
	}

	Window* System::getNextTargetWindow(Window* w) const
	{
		// if we have not reached the modal target, return the parent
		if (w != d_modalTarget)
		{
			return w->getParent();
		}

		// otherwise stop now
		return 0;
	}
}