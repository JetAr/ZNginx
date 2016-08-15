#ifndef __ELGUIWindow_H__
#define __ELGUIWindow_H__

#include "ELGUIVector.h"
#include "ELGUIRect.h"
#include "ELGUIUDim.h"
#include "ELGUIInputEvent.h"
#include "ELGUIEventSet.h"
#include "ELGUIRenderCache.h"
#include <vector>
#include <string>

namespace ELGUI
{
	class Window : public EventSet
	{
	public:
		static const std::string EventParentSized;
		static const std::string EventSized;
		static const std::string EventMoved;
		static const std::string EventAlphaChanged;
		static const std::string EventActivated;
		static const std::string EventDeactivated;
		static const std::string EventEnabled;
		static const std::string EventDisabled;
		static const std::string EventShown;
		static const std::string EventHidden;
		static const std::string EventInputCaptureGained;
		static const std::string EventInputCaptureLost;
		static const std::string EventChildAdded;
		static const std::string EventChildRemoved;
		static const std::string EventZOrderChanged;
		static const std::string EventMouseEnters;
		static const std::string EventMouseLeaves;
		static const std::string EventMouseMove;
		static const std::string EventMouseWheel;
		static const std::string EventMouseButtonDown;
		static const std::string EventMouseButtonUp;
		static const std::string EventMouseClick;
		static const std::string EventMouseDoubleClick;
		static const std::string EventMouseTripleClick;
		static const std::string EventKeyDown;
		static const std::string EventKeyUp;
		static const std::string EventCharacterKey;

	public:
		Window(const std::string& type, const std::string& name);
		virtual ~Window();

		void destroy();
		
		virtual void onMoved(WindowEventArgs& e);
		virtual void onSized(WindowEventArgs& e);
		virtual void onParentSized(WindowEventArgs& e);
		virtual void onActivated(ActivationEventArgs& e);
		virtual void onDeactivated(ActivationEventArgs& e);
		virtual void onAlphaChanged(WindowEventArgs& e);
		virtual void onZChanged(WindowEventArgs& e);
		virtual void onChildAdded(WindowEventArgs& e);
		virtual void onChildRemoved(WindowEventArgs& e);
		virtual void onCaptureGained(WindowEventArgs& e);
		virtual void onCaptureLost(WindowEventArgs& e);
		virtual void onEnabled(WindowEventArgs& e);
		virtual void onDisabled(WindowEventArgs& e);
		virtual void onShown(WindowEventArgs& e);
		virtual void onHidden(WindowEventArgs& e);
		
		virtual void onMouseEnters(MouseEventArgs& e);
		virtual void onMouseLeaves(MouseEventArgs& e);
		virtual void onMouseMove(MouseEventArgs& e);
		virtual void onMouseWheel(MouseEventArgs& e);
		
		virtual void onMouseButtonDown(MouseEventArgs& e);
		virtual void onMouseButtonUp(MouseEventArgs& e);
		virtual void onMouseClicked(MouseEventArgs& e);
		virtual void onMouseDoubleClicked(MouseEventArgs& e);
		virtual void onMouseTripleClicked(MouseEventArgs& e);

		virtual void onKeyDown(KeyEventArgs& e);
		virtual void onKeyUp(KeyEventArgs& e);
		virtual void onCharacter(KeyEventArgs& e);

		void setArea(const UDim& xpos, const UDim& ypos, const UDim& width, const UDim& height);
		void setArea(const UVector2& pos, const UVector2& size);
		void setArea(const URect& area);
		void setPosition(const UVector2& pos);
		void setXPosition(const UDim& x);
		void setYPosition(const UDim& y);
		void setSize(const UVector2& size);
		void setWidth(const UDim& width);
		void setHeight(const UDim& height);

		const std::string& getName() const;
		const std::string& getType() const;
		
		Window* getParent() const;
		Window* getTargetChildAtPosition(const Vector2& position) const;

		void Window::setParent(Window* parent);

		Rect getPixelRect() const;
		Rect getInnerRect() const;
		Rect getUnclippedPixelRect() const;
		Rect getUnclippedInnerRect() const;
		
		const URect& getArea() const;
		Size getPixelSize() const;

		float getParentPixelWidth() const;
		float getParentPixelHeight() const;
		Size getParentPixelSize() const;

		bool isCapturedByThis(void) const;
		static Window* getCaptureWindow();

		void notifyScreenAreaChanged();

		void activate();
		void deactivate();

		bool isVisible(bool localOnly = false) const;
		bool isDisabled(bool localOnly = false) const;
		bool isClippedByParent() const;
		bool isAlwaysOnTop() const;
		bool isMousePassThroughEnabled() const;
		bool isDestroyedByParent() const;
		bool wantsMultiClickEvents() const;
		bool distributesCapturedInputs() const;

		void setMousePassThroughEnabled(bool setting);

		bool inheritsAlpha() const;
		float getAlpha() const;
		float getEffectiveAlpha() const;
		void setAlpha(float alpha);

		void setEnabled(bool setting);
		void setVisible(bool setting);

		void requestRedraw() const;

		bool isActive() const;
		bool isTopOfZOrder() const;
		bool isHit(const Vector2& position) const;

		void addChildWindow(const std::string& name);
		void addChildWindow(Window* window);
		void removeChildWindow(Window* window);

		size_t getChildCount() const;
		size_t getChildRecursiveCount();

		bool isChild(const std::string& name) const;
		bool isChild(const Window* window) const;

		Window* getChild(const std::string& name) const;
		Window* recursiveChildSearch(const std::string& name) const;
		Window* getChildAtIdx(size_t idx) const;

		Window* getActiveChild();
		const Window* getActiveChild() const;
		Window* getActiveSibling();

		bool isAncestor(const Window* window) const;

		void addWindowToDrawList(Window& wnd, bool at_back = false);
		void removeWindowFromDrawList(const Window& wnd);

		bool doRiseOnClick();
		void moveToFront();
		void moveToBack();

		bool captureInput();
		void releaseInput();

		void render();

	protected:
		void setArea_impl(const UVector2& pos, const UVector2& size, bool topLeftSizing = false, bool fireEvents = true);

		Rect getPixelRect_impl() const;
		Rect getUnclippedInnerRect_impl() const;
		Size getSize_impl(const Window* window) const;
		
		void cleanupChildren();
		void addChild_impl(Window* wnd);
		void removeChild_impl(Window* wnd);

		virtual void onZChange_impl();

		bool moveToFront_impl(bool wasClicked);

		virtual void drawSelf();
		virtual void populateRenderCache();
		RenderCache& getRenderCache();

	protected:
		typedef std::vector<Window*> ChildList;
		// The list of child Window objects attached to this.
		ChildList			d_children;
		// Child window objects arranged in rendering order.
		ChildList			d_drawList;

		const std::string	d_type;
		std::string			d_name;

		static Window*		d_captureWindow;
		Window*				d_oldCapture;

		Window*				d_parent;

		bool				d_visible;
		bool				d_enabled;
		bool				d_clippedByParent;
		bool				d_alwaysOnTop;
		bool				d_mousePassThroughEnabled;
		bool				d_riseOnClick;
		bool				d_destroyedByParent;
		bool				d_wantsMultiClicks;
		bool				d_distCapturedInputs;
		bool				d_inheritsAlpha;
		bool				d_zOrderingEnabled;
		float				d_alpha;
		bool				d_restoreOldCapture;

		bool				d_active;

		URect				d_area;
		Size				d_pixelSize;

		mutable Rect		d_screenUnclippedRect;
		mutable bool		d_screenUnclippedRectValid;
		mutable Rect		d_screenUnclippedInnerRect;
		mutable bool		d_screenUnclippedInnerRectValid;
		mutable Rect		d_screenRect;
		mutable bool		d_screenRectValid;
		mutable Rect		d_screenInnerRect;
		mutable bool		d_screenInnerRectValid;

		RenderCache			d_renderCache;
		mutable bool		d_needsRedraw;
	};
}

#endif //__ELGUIWindow_H__