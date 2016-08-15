#ifndef __ELGUICoordConverter_H__
#define __ELGUICoordConverter_H__

#include "ELGUIVector.h"
#include "ELGUIUDim.h"

namespace ELGUI
{
	class Window;

	class CoordConverter
	{
	public:
		static float windowToScreenX(const Window& window, const UDim& x);
		static float windowToScreenX(const Window& window, const float x);

		static float windowToScreenY(const Window& window, const UDim& y);
		static float windowToScreenY(const Window& window, const float y);

		static Vector2 windowToScreen(const Window& window, const UVector2& vec);
		static Vector2 windowToScreen(const Window& window, const Vector2& vec);
		static Rect windowToScreen(const Window& window, const URect& rect);
		static Rect windowToScreen(const Window& window, const Rect& rect);

		static float screenToWindowX(const Window& window, const UDim& x);
		static float screenToWindowX(const Window& window, const float x);

		static float screenToWindowY(const Window& window, const UDim& y);
		static float screenToWindowY(const Window& window, const float y);

		static Vector2 screenToWindow(const Window& window, const UVector2& vec);
		static Vector2 screenToWindow(const Window& window, const Vector2& vec);
		static Rect screenToWindow(const Window& window, const URect& rect);
		static Rect screenToWindow(const Window& window, const Rect& rect);

	protected:
		static float getBaseXValue(const Window& window);
		static float getBaseYValue(const Window& window);
		static Vector2 getBaseValue(const Window& window);
	};
}

#endif //__ELGUICoordConverter_H__