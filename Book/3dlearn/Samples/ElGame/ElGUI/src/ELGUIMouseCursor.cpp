#include "ELGUIPCH.h"
#include "ELGUIMouseCursor.h"

namespace ELGUI
{
	template<> MouseCursor* Singleton<MouseCursor>::ms_Singleton	= 0;

	MouseCursor::MouseCursor()
	: d_position(0.0f, 0.0f)
	, d_size(0.0f, 0.0f)
	{

	}

	MouseCursor::~MouseCursor()
	{

	}

	MouseCursor& MouseCursor::getSingleton()
	{
		return Singleton<MouseCursor>::getSingleton();
	}

	MouseCursor* MouseCursor::getSingletonPtr()
	{
		return Singleton<MouseCursor>::getSingletonPtr();
	}

	void MouseCursor::setPosition(const Point& position)
	{
		d_position.d_x = position.d_x;
		d_position.d_y = position.d_y;
	}

	void MouseCursor::offsetPosition(const Point& offset)
	{
		d_position.d_x += offset.d_x;
		d_position.d_y += offset.d_y;
	}

	Point MouseCursor::getPosition() const
	{
		return Point(d_position.d_x, d_position.d_y);
	}

	void MouseCursor::setSize(const Size& sz)
	{
		d_size = sz;
	}

	Size MouseCursor::getSize() const
	{
		return d_size;
	}
}