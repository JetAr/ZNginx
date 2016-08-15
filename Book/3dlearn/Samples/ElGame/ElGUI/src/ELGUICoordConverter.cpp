#include "ELGUIPCH.h"
#include "ELGUIWindow.h"
#include "ELGUICoordConverter.h"
#include "ELGUISystem.h"

namespace ELGUI
{
	float CoordConverter::windowToScreenX(const Window& window, const UDim& x)
	{
		return getBaseXValue(window) + x.asAbsolute(window.getPixelSize().d_width);
	}

	float CoordConverter::windowToScreenY(const Window& window, const UDim& y)
	{
		return getBaseYValue(window) + y.asAbsolute(window.getPixelSize().d_height);
	}

	Vector2 CoordConverter::windowToScreen(const Window& window, const UVector2& vec)
	{
		return getBaseValue(window) + vec.asAbsolute(window.getPixelSize());
	}

	Rect CoordConverter::windowToScreen(const Window& window, const URect& rect)
	{
		Rect tmp(rect.asAbsolute(window.getPixelSize()));
		return tmp.offset(getBaseValue(window));
	}

	float CoordConverter::screenToWindowX(const Window& window, const UDim& x)
	{
		return x.asAbsolute(System::getSingleton().getRenderer()->getWidth()) -
			getBaseXValue(window);
	}

	float CoordConverter::screenToWindowY(const Window& window, const UDim& y)
	{
		return y.asAbsolute(System::getSingleton().getRenderer()->getHeight()) -
			getBaseYValue(window);
	}

	Vector2 CoordConverter::screenToWindow(const Window& window, const UVector2& vec)
	{
		return vec.asAbsolute(System::getSingleton().getRenderer()->getSize()) -
			getBaseValue(window);
	}

	Rect CoordConverter::screenToWindow(const Window& window, const URect& rect)
	{
		Vector2 base(getBaseValue(window));
		Rect pixel(rect.asAbsolute(System::getSingleton().getRenderer()->getSize()));

		// negate base position
		base.d_x = -base.d_x;
		base.d_y = -base.d_y;

		return pixel.offset(base);
	}

	float CoordConverter::windowToScreenX(const Window& window, const float x)
	{
		return getBaseXValue(window) + x;
	}

	float CoordConverter::windowToScreenY(const Window& window, const float y)
	{
		return getBaseYValue(window) + y;
	}

	Vector2 CoordConverter::windowToScreen(const Window& window, const Vector2& vec)
	{
		return getBaseValue(window) + vec;
	}

	Rect CoordConverter::windowToScreen(const Window& window, const Rect& rect)
	{
		Rect tmp(rect);
		return tmp.offset(getBaseValue(window));
	}

	float CoordConverter::screenToWindowX(const Window& window, const float x)
	{
		return x - getBaseXValue(window);
	}

	float CoordConverter::screenToWindowY(const Window& window, const float y)
	{
		return y - getBaseYValue(window);
	}

	Vector2 CoordConverter::screenToWindow(const Window& window, const Vector2& vec)
	{
		return vec - getBaseValue(window);
	}

	Rect CoordConverter::screenToWindow(const Window& window, const Rect& rect)
	{
		Vector2 base(getBaseValue(window));

		// negate base position
		base.d_x = -base.d_x;
		base.d_y = -base.d_y;

		Rect tmp(rect);
		return tmp.offset(base);
	}

	float CoordConverter::getBaseXValue(const Window& window)
	{
		const float parent_width = window.getParentPixelWidth();
		float baseX = window.getParent() ? getBaseXValue(*window.getParent()) : 0;

		baseX += window.getArea().d_min.d_x.asAbsolute(parent_width);

		return baseX;
	}

	float CoordConverter::getBaseYValue(const Window& window)
	{
		const float parent_height = window.getParentPixelHeight();
		float baseY = window.getParent() ? getBaseYValue(*window.getParent()) : 0;

		baseY += window.getArea().d_min.d_y.asAbsolute(parent_height);

		return baseY;
	}

	Vector2 CoordConverter::getBaseValue(const Window& window)
	{
		return Vector2(getBaseXValue(window), getBaseYValue(window));
	}
}