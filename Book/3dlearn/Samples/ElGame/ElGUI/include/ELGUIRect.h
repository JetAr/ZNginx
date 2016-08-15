#ifndef __ELGUIRect_H__
#define __ELGUIRect_H__

#include "ELGUISize.h"
#include "ELGUIVector.h"

namespace ELGUI
{
	class Rect
	{
	public:
		Rect() {}
		Rect(float left, float top, float right, float bottom);
		Rect(Point pos, Size sz);

		Point getPosition() const
		{
			return Point(d_left, d_top);
		}

		float getWidth() const
		{
			return d_right - d_left;
		}

		float getHeight() const
		{
			return d_bottom - d_top;
		}

		Size getSize() const
		{
			return Size(getWidth(), getHeight());
		}

		void setPosition(const Point& pt);

		void setWidth(float width)
		{
			d_right = d_left + width;
		}

		void setHeight(float height)
		{
			d_bottom = d_top + height;
		}

		void setSize(const Size& sze)
		{
			setWidth(sze.d_width); setHeight(sze.d_height);
		}

		Rect getIntersection(const Rect& rect) const;
		bool isPointInRect(const Point& pt) const;
		Rect& offset(const Point& pt);

		Rect& constrainSizeMax(const Size& sz);
		Rect& constrainSizeMin(const Size& sz);
		Rect& constrainSize(const Size& max_sz, const Size& min_sz);

		bool operator==(const Rect& rhs) const
		{
			return ((d_left == rhs.d_left) && (d_right == rhs.d_right) && (d_top == rhs.d_top) && (d_bottom == rhs.d_bottom));
		}

		bool operator!=(const Rect& rhs) const
		{
			return !operator==(rhs);
		}

		Rect& operator=(const Rect& rhs);

		Rect operator*(float scalar) const
		{
			return Rect(d_left * scalar, d_top * scalar, d_right * scalar, d_bottom * scalar);
		}

		const Rect& operator*=(float scalar)
		{
			d_left *= scalar; d_top *= scalar; d_right *= scalar; d_bottom *= scalar; return *this;
		}

		// Data Fields
		float	d_top, d_bottom, d_left, d_right;
	};
}

#endif //__ELGUIRect_H__