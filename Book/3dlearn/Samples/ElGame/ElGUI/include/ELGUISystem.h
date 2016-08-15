#ifndef __ELGUISystem_H__
#define __ELGUISystem_H__

#include "ELGUISingleton.h"
#include "ELGUIRenderer.h"
#include "ELGUIWindow.h"
#include "ELGUIVector.h"
#include "ELGUISize.h"
#include "ELGUIInputEvent.h"

namespace ELGUI
{
	class SimpleTimer
	{
	public:
		static double currentTime();

		SimpleTimer() : d_baseTime(currentTime()) {}

		void restart() { d_baseTime = currentTime(); }
		double elapsed() { return currentTime() - d_baseTime; }

	public:
		double	d_baseTime;
	};

	struct MouseClickTracker
	{
		MouseClickTracker(void) : d_click_count(0), d_click_area(0, 0, 0, 0) {}

		SimpleTimer		d_timer;			// Timer used to track clicks for this button.
		int				d_click_count;		//  count of clicks made so far.
		Rect			d_click_area;		// area used to detect multi-clicks
		Window*         d_target_window;    // target window for any events generated.
	};

	struct MouseClickTrackerImpl
	{
		MouseClickTracker	click_trackers[MouseButtonCount];
	};

	class System : public Singleton<System>
	{
	public:
		System(Renderer* renderer);
		virtual ~System();

		Window*	setGUISheet(Window* sheet);
		Window*	getGUISheet() const;

		void renderGUI();

		bool injectMouseMove(float delta_x, float delta_y);
		bool injectMousePosition(float x_pos, float y_pos);
		bool injectMouseButtonDown(MouseButton button);
		bool injectMouseButtonUp(MouseButton button);
		bool injectMouseWheelChange(float delta);
		bool injectMouseSize(float width, float hight);

		bool injectKeyDown(unsigned int key_code);
		bool injectKeyUp(unsigned int key_code);
		bool injectChar(unsigned int code_point);

		SystemKey mouseButtonToSyskey(MouseButton btn) const;
		SystemKey keyCodeToSyskey(Key::Scan key, bool direction);
		unsigned int getSystemKeys() const;

		void notifyWindowDestroyed(const Window* window);

		void setModalTarget(Window* target);

		Window*	getWindowContainingMouse() const;
		Window* getModalTarget(void) const;
		Window* getFocusWindow( void );
		Window* getTargetWindow(const Point& pt) const;
		Window* getKeyboardTargetWindow() const;

		Renderer* getRenderer() const;
		bool handleDisplaySizeChange();

		void signalRedraw();

		static System& getSingleton();
		static System* getSingletonPtr();

	protected:
		void createSingletons();
		void destroySingletons();

		void addWindowFactories();

		Window* getNextTargetWindow(Window* w) const;

	protected:
		Renderer*		d_renderer;
		Window*			d_wndWithMouse;
		Window*			d_activeSheet;
		Window*			d_modalTarget;

		bool			d_gui_redraw;

		unsigned int	d_sysKeys;				// Current set of system keys pressed (in mk1 these were passed in, here we track these ourself).
		bool			d_lshift;				// Tracks state of left shift.
		bool			d_rshift;				// Tracks state of right shift.
		bool			d_lctrl;				// Tracks state of left control.
		bool			d_rctrl;				// Tracks state of right control.
		bool			d_lalt;					// Tracks state of left alt.
		bool			d_ralt;					// Tracks state of right alt.

		double			d_click_timeout;		// Timeout value, in seconds, used to generate a single-click (button down then up)
		double			d_dblclick_timeout;		// Timeout value, in seconds, used to generate multi-click events (botton down, then up, then down, and so on).
		Size			d_dblclick_size;		// Size of area the mouse can move and still make multi-clicks.

		MouseClickTrackerImpl* const	d_clickTrackerPimpl;

		float			d_mouseScalingFactor;	// Scaling applied to mouse movement inputs.

	public:
		static const double		DefaultSingleClickTimeout;		// Default timeout for generation of single click events.
		static const double		DefaultMultiClickTimeout;		// Default timeout for generation of multi-click events.
		static const Size		DefaultMultiClickAreaSize;		// Default allowable mouse movement for multi-click event generation.
	};
}

#endif //__ELGUISystem_H__