#include "ELGUIPCH.h"
#include "ELGUIWindow.h"
#include "ELGUICoordConverter.h"
#include "ELGUIWindowManager.h"
#include "ELGUISystem.h"
#include <algorithm>

namespace ELGUI
{
	Window*	Window::d_captureWindow = 0;
	
	const std::string Window::EventParentSized("ParentSized");
	const std::string Window::EventSized("Sized");
	const std::string Window::EventMoved("Moved");
	const std::string Window::EventActivated("Activated");
	const std::string Window::EventDeactivated("Deactivated");
	const std::string Window::EventEnabled("Enabled");
	const std::string Window::EventDisabled("Disabled");
	const std::string Window::EventShown("Shown");
	const std::string Window::EventHidden("Hidden");
	const std::string Window::EventAlphaChanged("AlphaChanged");
	const std::string Window::EventZOrderChanged("ZChanged");
	const std::string Window::EventInputCaptureGained("CaptureGained");
	const std::string Window::EventInputCaptureLost("CaptureLost");
	const std::string Window::EventChildAdded("AddedChild");
	const std::string Window::EventChildRemoved("RemovedChild");
	const std::string Window::EventMouseEnters("MouseEnter");
	const std::string Window::EventMouseLeaves("MouseLeave");
	const std::string Window::EventMouseMove("MouseMove");
	const std::string Window::EventMouseWheel("MouseWheel");
	const std::string Window::EventMouseButtonDown("MouseButtonDown");
	const std::string Window::EventMouseButtonUp("MouseButtonUp");
	const std::string Window::EventMouseClick("MouseClick");
	const std::string Window::EventMouseDoubleClick("MouseDoubleClick");
	const std::string Window::EventMouseTripleClick("MouseTripleClick");
	const std::string Window::EventKeyDown("KeyDown");
	const std::string Window::EventKeyUp("KeyUp");
	const std::string Window::EventCharacterKey("CharacterKey");

	Window::Window(const std::string& type, const std::string& name)
	: d_type(type)
	, d_name(name)
	, d_oldCapture(0)
	, d_parent(0)
	, d_visible(true)
	, d_enabled(true)
	, d_clippedByParent(true)
	, d_alwaysOnTop(false)
	, d_mousePassThroughEnabled(false)
	, d_riseOnClick(true)
	, d_destroyedByParent(false)
	, d_wantsMultiClicks(true)
	, d_distCapturedInputs(false)
	, d_inheritsAlpha(true)
	, d_zOrderingEnabled(true)
	, d_alpha(1.0f)
	, d_restoreOldCapture(false)
	, d_active(false)
	, d_area(elgui_absdim(0.0f), elgui_absdim(0.0f), elgui_absdim(0.0f), elgui_absdim(0.0f))
	, d_pixelSize(0, 0)
	, d_screenUnclippedRect(0, 0, 0, 0)
	, d_screenUnclippedRectValid(false)
	, d_screenUnclippedInnerRect(0, 0, 0, 0)
	, d_screenUnclippedInnerRectValid(false)
	, d_screenRect(0, 0, 0, 0)
	, d_screenRectValid(false)
	, d_screenInnerRect(0, 0, 0, 0)
	, d_screenInnerRectValid(false)
	, d_needsRedraw(true)
	{

	}

	Window::~Window()
	{
		// cleanup events actually happened earlier.
	}

	void Window::destroy()
	{
		// double check we are detached from parent
		if (d_parent)
		{
			d_parent->removeChildWindow(this);
		}

		cleanupChildren();
	}

	void Window::onMoved(WindowEventArgs& e)
	{
		// inform children their parent has been moved
		const size_t child_count = getChildCount();
		for (size_t i = 0; i < child_count; ++i)
		{
			d_children[i]->notifyScreenAreaChanged();
		}

		// we no longer want a total redraw here, instead we just get each window
		// to resubmit it's imagery to the Renderer.
		System::getSingleton().signalRedraw();
		fireEvent(EventMoved, e);
	}

	void Window::onSized(WindowEventArgs& e)
	{
		// inform children their parent has been re-sized
		size_t child_count = getChildCount();
		for (size_t i = 0; i < child_count; ++i)
		{
			WindowEventArgs args(this);
			d_children[i]->onParentSized(args);
		}

		requestRedraw();

		fireEvent(EventSized, e);
	}

	void Window::onParentSized(WindowEventArgs& e)
	{
		// set window area back on itself to cause minimum and maximum size
		// constraints to be applied as required.  (fire no events though)
		setArea_impl(d_area.getPosition(), d_area.getSize(), false, false);

		bool moved = ((d_area.d_min.d_x.d_scale != 0) || (d_area.d_min.d_y.d_scale != 0));
		bool sized = ((d_area.d_max.d_x.d_scale != 0) || (d_area.d_max.d_y.d_scale != 0));

		// now see if events should be fired.
		if (moved)
		{
			WindowEventArgs args(this);
			onMoved(args);
		}

		if (sized)
		{
			WindowEventArgs args(this);
			onSized(args);
		}

		fireEvent(EventParentSized, e);
	}

	void Window::onActivated(ActivationEventArgs& e)
	{
		d_active = true;
		requestRedraw();
		fireEvent(EventActivated, e);
	}

	void Window::onDeactivated(ActivationEventArgs& e)
	{
		// first de-activate all children
		size_t child_count = getChildCount();
		for (size_t i = 0; i < child_count; ++i)
		{
			if (d_children[i]->isActive())
			{
				// make sure the child gets itself as the .window member
				ActivationEventArgs child_e(d_children[i]);
				child_e.otherWindow = e.otherWindow;
				d_children[i]->onDeactivated(child_e);
			}
		}

		d_active = false;
		requestRedraw();
		fireEvent(EventDeactivated, e);
	}

	void Window::onAlphaChanged(WindowEventArgs& e)
	{
		// scan child list and call this method for all children that inherit alpha
		size_t child_count = getChildCount();

		for (size_t i = 0; i < child_count; ++i)
		{
			if (d_children[i]->inheritsAlpha())
			{
				WindowEventArgs args(d_children[i]);
				d_children[i]->onAlphaChanged(args);
			}
		}

		requestRedraw();
		fireEvent(EventAlphaChanged, e);
	}

	void Window::onZChanged(WindowEventArgs& e)
	{
		// we no longer want a total redraw here, instead we just get each window
		// to resubmit it's imagery to the Renderer.
		System::getSingleton().signalRedraw();
		fireEvent(EventZOrderChanged, e);
	}

	void Window::onChildAdded(WindowEventArgs& e)
	{
		// we no longer want a total redraw here, instead we just get each window
		// to resubmit it's imagery to the Renderer.
		System::getSingleton().signalRedraw();
		fireEvent(EventChildAdded, e);
	}

	void Window::onChildRemoved(WindowEventArgs& e)
	{
		// we no longer want a total redraw here, instead we just get each window
		// to resubmit it's imagery to the Renderer.
		System::getSingleton().signalRedraw();
		fireEvent(EventChildRemoved, e);
	}

	void Window::onCaptureGained(WindowEventArgs& e)
	{
		fireEvent(EventInputCaptureGained, e);
	}

	void Window::onCaptureLost(WindowEventArgs& e)
	{
		// handle restore of previous capture window as required.
		if (d_restoreOldCapture && (d_oldCapture != 0)) {
			d_oldCapture->onCaptureLost(e);
			d_oldCapture = 0;
		}

		// handle case where mouse is now in a different window
		// (this is a bit of a hack that uses the mouse input injector to handle this for us).
		System::getSingleton().injectMouseMove(0, 0);

		fireEvent(EventInputCaptureLost, e);
	}

	void Window::onEnabled(WindowEventArgs& e)
	{
		// signal all non-disabled children that they are now enabled (via inherited state)
		size_t child_count = getChildCount();
		for (size_t i = 0; i < child_count; ++i)
		{
			if (d_children[i]->d_enabled)
			{
				WindowEventArgs args(d_children[i]);
				d_children[i]->onEnabled(args);
			}
		}

		requestRedraw();
		fireEvent(EventEnabled, e);
	}


	void Window::onDisabled(WindowEventArgs& e)
	{
		// signal all non-disabled children that they are now disabled (via inherited state)
		size_t child_count = getChildCount();
		for (size_t i = 0; i < child_count; ++i)
		{
			if (d_children[i]->d_enabled)
			{
				WindowEventArgs args(d_children[i]);
				d_children[i]->onDisabled(args);
			}
		}

		requestRedraw();
		fireEvent(EventDisabled, e);
	}

	void Window::onShown(WindowEventArgs& e)
	{
		requestRedraw();
		fireEvent(EventShown, e);
	}


	void Window::onHidden(WindowEventArgs& e)
	{
		requestRedraw();
		fireEvent(EventHidden, e);
	}

	void Window::onMouseEnters(MouseEventArgs& e)
	{
		fireEvent(EventMouseEnters, e);
	}

	void Window::onMouseLeaves(MouseEventArgs& e)
	{
		fireEvent(EventMouseLeaves, e);
	}

	void Window::onMouseMove(MouseEventArgs& e)
	{
		fireEvent(EventMouseMove, e);
	}

	void Window::onMouseWheel(MouseEventArgs& e)
	{
		fireEvent(EventMouseWheel, e);
	}

	void Window::onMouseButtonDown(MouseEventArgs& e)
	{
		if (e.button == LeftButton)
		{
			e.handled |= doRiseOnClick();
		}

		fireEvent(EventMouseButtonDown, e);
	}

	void Window::onMouseButtonUp(MouseEventArgs& e)
	{
		fireEvent(EventMouseButtonUp, e);
	}

	void Window::onMouseClicked(MouseEventArgs& e)
	{
		fireEvent(EventMouseClick, e);
	}

	void Window::onMouseDoubleClicked(MouseEventArgs& e)
	{
		fireEvent(EventMouseDoubleClick, e);
	}

	void Window::onMouseTripleClicked(MouseEventArgs& e)
	{
		fireEvent(EventMouseTripleClick, e);
	}

	void Window::onKeyDown(KeyEventArgs& e)
	{
		fireEvent(EventKeyDown, e);
	}

	void Window::onKeyUp(KeyEventArgs& e)
	{
		fireEvent(EventKeyUp, e);
	}

	void Window::onCharacter(KeyEventArgs& e)
	{
		fireEvent(EventCharacterKey, e);
	}

	void Window::setArea(const UDim& xpos, const UDim& ypos, const UDim& width, const UDim& height)
	{
		setArea(UVector2(xpos, ypos), UVector2(width, height));
	}

	void Window::setArea(const UVector2& pos, const UVector2& size)
	{
		setArea_impl(pos, size);
	}

	void Window::setArea(const URect& area)
	{
		setArea(area.d_min, area.getSize());
	}

	void Window::setPosition(const UVector2& pos)
	{
		setArea_impl(pos, d_area.getSize());
	}

	void Window::setXPosition(const UDim& x)
	{
		setArea_impl(UVector2(x, d_area.d_min.d_y), d_area.getSize());
	}

	void Window::setYPosition(const UDim& y)
	{
		setArea_impl(UVector2(d_area.d_min.d_x, y), d_area.getSize());
	}

	void Window::setSize(const UVector2& size)
	{
		setArea_impl(d_area.getPosition(), size);
	}

	void Window::setWidth(const UDim& width)
	{
		setArea_impl(d_area.getPosition(), UVector2(width, d_area.getSize().d_y));
	}

	void Window::setHeight(const UDim& height)
	{
		setArea_impl(d_area.getPosition(), UVector2(d_area.getSize().d_x, height));
	}

	const std::string& Window::getName() const
	{
		return d_name;
	}

	const std::string& Window::getType() const
	{
		return d_type;
	}

	Window* Window::getParent() const
	{
		return d_parent;
	}

	Window* Window::getTargetChildAtPosition(const Vector2& position) const
	{
		ChildList::const_reverse_iterator child, end;
		end = d_drawList.rend();

		for (child = d_drawList.rbegin(); child != end; ++child)
		{
			if ((*child)->isVisible())
			{
				// recursively scan children of this child windows...
				Window* wnd = (*child)->getTargetChildAtPosition(position);

				// return window pointer if we found a 'hit' down the chain somewhere
				if (wnd)
					return wnd;
				// see if this child is hit and return it's pointer if it is
				else if (!(*child)->isMousePassThroughEnabled() && (*child)->isHit(position))
					return (*child);
			}
		}

		// nothing hit
		return 0;
	}

	void Window::setParent(Window* parent)
	{
		d_parent = parent;
	}

	Rect Window::getPixelRect() const
	{
		if (!d_screenRectValid)
		{
			d_screenRect = getPixelRect_impl();
			d_screenRectValid = true;
		}

		return d_screenRect;
	}

	Rect Window::getInnerRect() const
	{
		if (!d_screenInnerRectValid)
		{
			// clip to parent?
			if (isClippedByParent() && (d_parent != 0))
			{
				d_screenInnerRect = getUnclippedInnerRect().getIntersection(d_parent->getInnerRect());
			}
			// else, clip to screen
			else
			{
				d_screenInnerRect = getUnclippedInnerRect().getIntersection(
					System::getSingleton().getRenderer()->getRect());
			}
			d_screenInnerRectValid = true;
		}

		return d_screenInnerRect;
	}

	Rect Window::getUnclippedPixelRect() const
	{
		if (!d_screenUnclippedRectValid)
		{
			Rect localArea(0, 0, d_pixelSize.d_width, d_pixelSize.d_height);
			d_screenUnclippedRect = CoordConverter::windowToScreen(*this, localArea);
			d_screenUnclippedRectValid = true;
		}

		return d_screenUnclippedRect;
	}

	Rect Window::getUnclippedInnerRect() const
	{
		if (!d_screenUnclippedInnerRectValid)
		{
			d_screenUnclippedInnerRect = getUnclippedInnerRect_impl();
			d_screenUnclippedInnerRectValid = true;
		}

		return d_screenUnclippedInnerRect;
	}

	const URect& Window::getArea() const
	{
		return d_area;
	}

	Size Window::getPixelSize() const
	{
		return d_pixelSize;
	}

	float Window::getParentPixelWidth() const
	{
		return d_parent ?
			d_parent->d_pixelSize.d_width :
			System::getSingleton().getRenderer()->getWidth();
	}

	float Window::getParentPixelHeight() const
	{
		return d_parent ?
			d_parent->d_pixelSize.d_height:
			System::getSingleton().getRenderer()->getHeight();
	}

	Size Window::getParentPixelSize() const
	{
		return getSize_impl(d_parent);
	}

	bool Window::isCapturedByThis() const
	{
		return getCaptureWindow() == this;
	}

	Window* Window::getCaptureWindow()
	{
		return d_captureWindow;
	}

	void Window::notifyScreenAreaChanged()
	{
		d_screenUnclippedRectValid = false;
		d_screenUnclippedInnerRectValid = false;
		d_screenRectValid = false;
		d_screenInnerRectValid = false;

		// inform children that their screen area must be updated
		const size_t child_count = getChildCount();
		for (size_t i = 0; i < child_count; ++i)
		{
			d_children[i]->notifyScreenAreaChanged();
		}
	}

	void Window::activate()
	{
		// force complete release of input capture.
		// NB: This is not done via releaseCapture() because that has
		// different behaviour depending on the restoreOldCapture setting.
		if ((d_captureWindow != 0) && (d_captureWindow != this))
		{
			Window* tmpCapture = d_captureWindow;
			d_captureWindow = 0;

			WindowEventArgs args(0);
			tmpCapture->onCaptureLost(args);
		}

		moveToFront();
	}

	void Window::deactivate()
	{
		ActivationEventArgs args(this);
		args.otherWindow = 0;
		onDeactivated(args);
	}

	bool Window::isVisible(bool localOnly /* = false */) const
	{
		bool parVisible = ((d_parent == 0) || localOnly) ? true : d_parent->isVisible();

		return d_visible && parVisible;
	}

	bool Window::isDisabled(bool localOnly /* = false */) const
	{
		bool parDisabled = ((d_parent == 0) || localOnly) ? false : d_parent->isDisabled();

		return (!d_enabled) || parDisabled;
	}

	bool Window::isClippedByParent() const
	{
		return d_clippedByParent;
	}

	bool Window::isAlwaysOnTop() const
	{
		return d_alwaysOnTop;
	}

	bool Window::isMousePassThroughEnabled() const
	{
		return d_mousePassThroughEnabled;
	}

	bool Window::isDestroyedByParent() const
	{
		return d_destroyedByParent;
	}

	bool Window::wantsMultiClickEvents() const
	{
		return d_wantsMultiClicks;
	}

	bool Window::distributesCapturedInputs() const
	{
		return d_distCapturedInputs;
	}

	void Window::setMousePassThroughEnabled(bool setting)
	{
		d_mousePassThroughEnabled = setting;
	}

	bool Window::inheritsAlpha() const
	{
		return d_inheritsAlpha;
	}

	float Window::getAlpha() const
	{
		return d_alpha;
	}

	float Window::getEffectiveAlpha() const
	{
		if ((d_parent == 0) || (!inheritsAlpha()))
		{
			return d_alpha;
		}

		return d_alpha * d_parent->getEffectiveAlpha();
	}

	void Window::setAlpha(float alpha)
	{
		d_alpha = alpha;
		WindowEventArgs args(this);
		onAlphaChanged(args);
	}

	void Window::setEnabled(bool setting)
	{
		// only react if setting has changed
		if (d_enabled != setting)
		{
			d_enabled = setting;
			WindowEventArgs args(this);

			if (d_enabled)
			{
				// check to see if the window is actually enabled (which depends upon all ancestor windows being enabled)
				// we do this so that events we fire give an accurate indication of the state of a window.
				if ((d_parent && !d_parent->isDisabled()) || !d_parent)
					onEnabled(args);
			}
			else
			{
				onDisabled(args);
			}
		}
	}

	void Window::setVisible(bool setting)
	{
		// only react if setting has changed
		if (d_visible != setting)
		{
			d_visible = setting;
			WindowEventArgs args(this);
			d_visible ? onShown(args) : onHidden(args);

			if (!setting)
			{
				if (isActive())
				{
					deactivate();
				}
			}
		}
	}

	void Window::requestRedraw() const
	{
		d_needsRedraw = true;
		System::getSingleton().signalRedraw();
	}

	bool Window::isActive() const
	{
		bool parActive = (d_parent == 0) ? true : d_parent->isActive();

		return d_active && parActive;
	}

	bool Window::isTopOfZOrder() const
	{
		// if not attached, then always on top!
		if (!d_parent)
			return true;

		// get position of window at top of z-order in same group as this window
		ChildList::reverse_iterator pos = d_parent->d_drawList.rbegin();
		if (!d_alwaysOnTop)
		{
			// find last non-topmost window
			while ((pos != d_parent->d_drawList.rend()) && (*pos)->isAlwaysOnTop())
				++pos;
		}

		// return whether the window at the top of the z order is us
		return *pos == this;
	}

	bool Window::isHit(const Vector2& position) const
	{
		// cannot be hit if we are disabled.
		if (isDisabled())
			return false;

		Rect clipped_area(getPixelRect());

		if (clipped_area.getWidth() == 0)
			return false;

		return clipped_area.isPointInRect(position);
	}

	void Window::addChildWindow(const std::string& name)
	{
		addChildWindow(WindowManager::getSingleton().getWindow(name));
	}

	void Window::addChildWindow(Window* window)
	{
		// don't add ourselves as a child
		// and don't add null windows
		if (window == this || window == 0)
		{
			return;
		}
		addChild_impl(window);
		WindowEventArgs args(window);
		onChildAdded(args);
		window->onZChange_impl();
	}

	void Window::removeChildWindow(Window* window)
	{
		removeChild_impl(window);
		WindowEventArgs args(window);
		onChildRemoved(args);
		window->onZChange_impl();
	}

	size_t Window::getChildCount() const
	{
		return d_children.size();
	}

	size_t Window::getChildRecursiveCount()
	{
		size_t child_recursive_count = 0;

		size_t child_count = getChildCount();

		for (size_t i = 0; i < child_count; ++i)
		{
			child_recursive_count += d_children[i]->getChildRecursiveCount();
		}

		child_recursive_count += child_count;

		return child_recursive_count;
	}

	bool Window::isChild(const std::string& name) const
	{
		size_t child_count = getChildCount();

		for (size_t i = 0; i < child_count; ++i)
		{
			if (d_children[i]->getName() == name)
			{
				return true;
			}

		}

		return false;
	}

	bool Window::isChild(const Window* window) const
	{
		size_t child_count = getChildCount();

		for (size_t i = 0; i < child_count; ++i)
		{
			if (d_children[i] == window)
			{
				return true;
			}

		}

		return false;
	}

	Window* Window::getChild(const std::string& name) const
	{
		size_t child_count = getChildCount();

		for (size_t i = 0; i < child_count; ++i)
		{
			if (d_children[i]->getName() == name)
			{
				return d_children[i];
			}

		}

		for (size_t i = 0; i < child_count; ++i)
		{
			Window* temp = d_children[i]->recursiveChildSearch(name);
			if (temp)
				return temp;
		}

		return 0;
	}

	Window* Window::recursiveChildSearch(const std::string& name) const
	{
		size_t child_count = getChildCount();

		for (size_t i = 0; i < child_count; ++i)
		{
			if (d_children[i]->getName() == name)
			{
				return d_children[i];
			}
		}

		for (size_t i = 0; i < child_count; ++i)
		{
			Window* temp = d_children[i]->recursiveChildSearch(name);
			if (temp)
				return temp;
		}

		return 0;
	}

	Window* Window::getChildAtIdx(size_t idx) const
	{
		return d_children[idx];
	}

	Window* Window::getActiveChild()
	{
		return const_cast<Window*>(static_cast<const Window*>(this)->getActiveChild());
	}

	const Window* Window::getActiveChild() const
	{
		// are children can't be active if we are not
		if (!isActive())
		{
			return 0;
		}

		size_t pos = getChildCount();

		while (pos-- > 0)
		{
			// don't need full backward scan for activeness as we already know 'this' is active
			// NB: This uses the draw-ordered child list, as that should be quicker in most cases.
			if (d_drawList[pos]->d_active)
				return d_drawList[pos]->getActiveChild();
		}

		// no child was active, therefore we are the topmost active window
		return this;
	}

	Window* Window::getActiveSibling()
	{
		// initialise with this if we are active, else 0
		Window* activeWnd = isActive() ? this : 0;

		// if active window not already known, and we have a parent window
		if (!activeWnd && d_parent)
		{
			// scan backwards through the draw list, as this will
			// usually result in the fastest result.
			size_t idx = d_parent->getChildCount();
			while (idx-- > 0)
			{
				// if this child is active
				if (d_parent->d_drawList[idx]->isActive())
				{
					// set the return value
					activeWnd = d_parent->d_drawList[idx];
					// exit loop early, as we have found what we needed
					break;
				}
			}
		}

		// return whatever we discovered
		return activeWnd;
	}

	bool Window::isAncestor(const Window* window) const
	{
		// if we have no parent, then return false
		if (!d_parent)
		{
			return false;
		}

		// check our immediate parent
		if (d_parent == window)
		{
			return true;
		}

		// not our parent, check back up the family line
		return d_parent->isAncestor(window);
	}

	void Window::addWindowToDrawList(Window& wnd, bool at_back /* = false */)
	{
		// add behind other windows in same group
		if (at_back)
		{
			// calculate position where window should be added for drawing
			ChildList::iterator pos = d_drawList.begin();
			if (wnd.isAlwaysOnTop())
			{
				// find first topmost window
				while ((pos != d_drawList.end()) && (!(*pos)->isAlwaysOnTop()))
					++pos;
			}
			// add window to draw list
			d_drawList.insert(pos, &wnd);
		}
		// add in front of other windows in group
		else
		{
			// calculate position where window should be added for drawing
			ChildList::reverse_iterator	position = d_drawList.rbegin();
			if (!wnd.isAlwaysOnTop())
			{
				// find last non-topmost window
				while ((position != d_drawList.rend()) && ((*position)->isAlwaysOnTop()))
					++position;
			}
			// add window to draw list
			d_drawList.insert(position.base(), &wnd);
		}
	}

	void Window::removeWindowFromDrawList(const Window& wnd)
	{
		// if draw list is not empty
		if (!d_drawList.empty())
		{
			// attempt to find the window in the draw list
			ChildList::iterator	position = std::find(d_drawList.begin(), d_drawList.end(), &wnd);

			// remove the window if it was found in the draw list
			if (position != d_drawList.end())
				d_drawList.erase(position);
		}
	}

	void Window::moveToFront()
	{
		moveToFront_impl(false);
	}

	void Window::moveToBack()
	{
		// if the window is active, de-activate it.
		if (isActive())
		{
			ActivationEventArgs args(this);
			args.otherWindow = 0;
			onDeactivated(args);
		}

		// we only need to proceed if we have a parent (otherwise we have no siblings)
		if (d_parent)
		{
			if (d_zOrderingEnabled)
			{
				// remove us from our parent's draw list
				d_parent->removeWindowFromDrawList(*this);
				// re-attach ourselves to our parent's draw list which will move us in behind
				// sibling windows with the same 'always-on-top' setting as we have.
				d_parent->addWindowToDrawList(*this, true);
				// notify relevant windows about the z-order change.
				onZChange_impl();
			}

			d_parent->moveToBack();
		}
	}

	bool Window::doRiseOnClick()
	{
		// does this window rise on click?
		if (d_riseOnClick)
		{
			return moveToFront_impl(true);
		}
		else if (d_parent)
		{
			return d_parent->doRiseOnClick();
		}

		return false;
	}

	bool Window::captureInput()
	{
		// we can only capture if we are the active window (LEAVE THIS ALONE!)
		if (!isActive())
			return false;

		if (d_captureWindow != this)
		{
			Window* current_capture = d_captureWindow;
			d_captureWindow = this;
			WindowEventArgs args(this);

			// inform any window which previously had capture that it doesn't anymore!
			if ((current_capture != 0) && (current_capture != this) && (!d_restoreOldCapture))
				current_capture->onCaptureLost(args);

			if (d_restoreOldCapture)
				d_oldCapture = current_capture;

			onCaptureGained(args);
		}

		return true;
	}

	void Window::releaseInput()
	{
		// if we are not the window that has capture, do nothing
		if (!isCapturedByThis())
		{
			return;
		}

		// restore old captured window if that mode is set
		if (d_restoreOldCapture)
		{
			d_captureWindow = d_oldCapture;

			// check for case when there was no previously captured window
			if (d_oldCapture)
			{
				d_oldCapture = 0;
				d_captureWindow->moveToFront();
			}

		}
		else
		{
			d_captureWindow = 0;
		}

		WindowEventArgs args(this);
		onCaptureLost(args);
	}

	void Window::render()
	{
		// don't do anything if window is not visible
		if (!isVisible())
		{
			return;
		}

		// perform drawing for 'this' Window
		drawSelf();

		// render any child windows
		size_t child_count = getChildCount();

		for (size_t i = 0; i < child_count; ++i)
		{
			d_drawList[i]->render();
		}
	}

	void Window::setArea_impl(const UVector2& pos, const UVector2& size, bool topLeftSizing /* = false */, bool fireEvents /* = true */)
	{
		// we make sure the screen areas are recached when this is called as we need it in most cases
		d_screenUnclippedRectValid = false;
		d_screenUnclippedInnerRectValid = false;
		d_screenRectValid = false;
		d_screenInnerRectValid = false;

		// notes of what we did
		bool moved = false, sized;

		// save original size so we can work out how to behave later on
		Size oldSize(d_pixelSize);

		// calculate pixel sizes for everything, so we have a common format for comparisons.
		//Vector2 absMax(d_maxSize.asAbsolute(System::getSingleton().getRenderer()->getSize()));
		//Vector2 absMin(d_minSize.asAbsolute(System::getSingleton().getRenderer()->getSize()));
		d_pixelSize = size.asAbsolute(getParentPixelSize()).asSize();

		d_area.setSize(size);
		sized = (d_pixelSize != oldSize);

		// If this is a top/left edge sizing op, only modify position if the size actually changed.
		// If it is not a sizing op, then position may always change.
		if (!topLeftSizing || sized)
		{
			// only update position if a change has occurred.
			if (pos != d_area.d_min)
			{
				d_area.setPosition(pos);
				moved = true;
			}
		}

		// fire events as required
		if (fireEvents)
		{
			WindowEventArgs args(this);

			if (moved)
			{
				onMoved(args);
			}

			if (sized)
			{
				onSized(args);
			}
		}
	}

	Rect Window::getPixelRect_impl() const
	{
		// clip to parent?
		if (isClippedByParent() && (d_parent != 0))
		{
			return getUnclippedPixelRect().getIntersection(d_parent->getInnerRect());
		}
		// else, clip to screen
		else
		{
			return getUnclippedPixelRect().getIntersection(System::getSingleton().getRenderer()->getRect());
		}
	}

	Rect Window::getUnclippedInnerRect_impl() const
	{
		return getUnclippedPixelRect();
	}

	void Window::cleanupChildren()
	{
		while(getChildCount() != 0)
		{
			Window* wnd = d_children[0];

			// always remove child
			removeChildWindow(wnd);

			// destroy child if that is required
			if (wnd->isDestroyedByParent())
			{
				WindowManager::getSingleton().destroyWindow(wnd);
			}
		}
	}

	Size Window::getSize_impl(const Window* window) const
	{
		return window ?
			window->d_pixelSize :
			System::getSingleton().getRenderer()->getSize();
	}

	void Window::addChild_impl(Window* wnd)
	{
		// if window is already attached, detach it first (will fire normal events)
		if (wnd->getParent())
			wnd->getParent()->removeChildWindow(wnd);

		addWindowToDrawList(*wnd);

		// add window to child list
		d_children.push_back(wnd);

		// set the parent window
		wnd->setParent(this);

		// Force and update for the area Rects for 'wnd' so they're correct for it's new parent.
		WindowEventArgs args(this);
		wnd->onParentSized(args);
	}

	void Window::removeChild_impl(Window* wnd)
	{
		// remove from draw list
		removeWindowFromDrawList(*wnd);

		// if window has children
		if (!d_children.empty())
		{
			// find this window in the child list
			ChildList::iterator	position = std::find(d_children.begin(), d_children.end(), wnd);

			// if the window was found in the child list
			if (position != d_children.end())
			{
				// remove window from child list
				d_children.erase(position);
				// reset windows parent so it's no longer this window.
				wnd->setParent(0);
			}
		}
	}

	void Window::onZChange_impl()
	{
		if (!d_parent)
		{
			WindowEventArgs args(this);
			onZChanged(args);
		}
		else
		{
			size_t child_count = d_parent->getChildCount();

			for (size_t i = 0; i < child_count; ++i)
			{
				WindowEventArgs args(d_parent->d_children[i]);
				d_parent->d_children[i]->onZChanged(args);
			}
		}
	}

	bool Window::moveToFront_impl(bool wasClicked)
	{
		bool took_action = false;

		// if the window has no parent then we can have no siblings
		if (!d_parent)
		{
			// perform initial activation if required.
			if (!isActive())
			{
				took_action = true;
				ActivationEventArgs args(this);
				args.otherWindow = 0;
				onActivated(args);
			}

			return took_action;
		}

		// bring parent window to front of it's siblings
		took_action = wasClicked ? d_parent->doRiseOnClick() :
			d_parent->moveToFront_impl(false);

		// get immediate child of parent that is currently active (if any)
		Window* activeWnd = getActiveSibling();

		// if a change in active window has occurred
		if (activeWnd != this)
		{
			took_action = true;

			// notify ourselves that we have become active
			ActivationEventArgs args(this);

			// notify ourselves that we have become active, and
			// notify any previously active window that it is no longer active
			if (activeWnd)
			{
				args.window = activeWnd;
				args.otherWindow = this;
				args.handled = false;
				activeWnd->onDeactivated(args);
			}

			args.otherWindow = activeWnd;
			onActivated(args);
		}

		// bring us to the front of our siblings
		if ((d_zOrderingEnabled) && !isTopOfZOrder())
		{
			took_action = true;

			// remove us from our parent's draw list
			d_parent->removeWindowFromDrawList(*this);
			// re-attach ourselves to our parent's draw list which will move us in front of
			// sibling windows with the same 'always-on-top' setting as we have.
			d_parent->addWindowToDrawList(*this);
			// notify relevant windows about the z-order change.
			onZChange_impl();
		}

		return took_action;
	}

	void Window::drawSelf()
	{
		// window image cache needs to be regenerated.
		if (d_needsRedraw)
		{
			// dispose of already cached imagery.
			d_renderCache.clearCachedImagery();
			// re-populate cache
			populateRenderCache();

			// mark ourselves as no longer needed a redraw.
			d_needsRedraw = false;
		}

		// if render cache contains imagery.
		if (d_renderCache.hasCachedImagery())
		{
			Point absPos(getUnclippedPixelRect().getPosition());
			// calculate clipping area for this window
			Rect clipper(getPixelRect());
			// If window is not totally clipped.
			if (clipper.getWidth())
			{
				// send cached imagery to the renderer.
				d_renderCache.render(absPos, clipper);
			}
		}
	}

	void Window::populateRenderCache()
	{

	}

	RenderCache& Window::getRenderCache()
	{
		return d_renderCache;
	}
}