#ifndef __ELGUIVector_H__
#define __ELGUIVector_H__

#include "ELGUISize.h"

namespace ELGUI
{
	class Vector2
	{
	public:
		Vector2() {}
		Vector2(float x, float y) : d_x(x), d_y(y) {}

		Vector2& operator*=(const Vector2& vec)
		{
			d_x *= vec.d_x;
			d_y *= vec.d_y;

			return *this;
		}

		Vector2& operator/=(const Vector2& vec)
		{
			d_x /= vec.d_x;
			d_y /= vec.d_y;

			return *this;
		}

		Vector2& operator+=(const Vector2& vec)
		{
			d_x += vec.d_x;
			d_y += vec.d_y;

			return *this;
		}

		Vector2& operator-=(const Vector2& vec)
		{
			d_x -= vec.d_x;
			d_y -= vec.d_y;

			return *this;
		}

		Vector2	operator+(const Vector2& vec) const
		{
			return Vector2(d_x + vec.d_x, d_y + vec.d_y);
		}

		Vector2	operator-(const Vector2& vec) const
		{
			return Vector2(d_x - vec.d_x, d_y - vec.d_y);
		}

		Vector2	operator*(const Vector2& vec) const
		{
			return Vector2(d_x * vec.d_x, d_y * vec.d_y);
		}

		bool	operator==(const Vector2& vec) const
		{
			return ((d_x == vec.d_x) && (d_y == vec.d_y));
		}

		bool	operator!=(const Vector2& vec) const
		{
			return !(operator==(vec));
		}

		Size asSize() const { return Size(d_x, d_y); }

		float d_x, d_y;
	};
	
	typedef	Vector2 Point;

	class Vector3
	{
	public:
		Vector3() {}
		Vector3(float x, float y, float z) : d_x(x), d_y(y), d_z(z) {}

		float	d_x, d_y, d_z;
	};
}

#endif //__ELGUIVector_H__