#include "ELGUIPCH.h"
#include "ELGUIColourRect.h"

namespace ELGUI
{
	ColourRect::ColourRect(const Colour& top_left, const Colour& top_right, const Colour& bottom_left, const Colour& bottom_right)
	: d_top_left(top_left)
	, d_top_right(top_right)
	, d_bottom_left(bottom_left)
	, d_bottom_right(bottom_right)
	{
	}

	ColourRect::ColourRect(const Colour& col)
	: d_top_left(col)
	, d_top_right(col)
	, d_bottom_left(col)
	, d_bottom_right(col)
	{
	}

	ColourRect::ColourRect()
	: d_top_left()
	, d_top_right()
	, d_bottom_left()
	, d_bottom_right()
	{
	}

	void ColourRect::setAlpha(float alpha)
	{
		d_top_left.setAlpha(alpha);
		d_top_right.setAlpha(alpha);
		d_bottom_left.setAlpha(alpha);
		d_bottom_right.setAlpha(alpha);
	}

	void ColourRect::setTopAlpha(float alpha)
	{
		d_top_left.setAlpha(alpha);
		d_top_right.setAlpha(alpha);
	}

	void ColourRect::setBottomAlpha(float alpha)
	{
		d_bottom_left.setAlpha(alpha);
		d_bottom_right.setAlpha(alpha);
	}

	void ColourRect::setLeftAlpha(float alpha)
	{
		d_top_left.setAlpha(alpha);
		d_bottom_left.setAlpha(alpha);
	}

	void ColourRect::setRightAlpha(float alpha)
	{
		d_top_right.setAlpha(alpha);
		d_bottom_right.setAlpha(alpha);
	}

	void ColourRect::setColours(const Colour& col)
	{
		d_top_left = d_top_right = d_bottom_left = d_bottom_right = col;
	}
	
	Colour ColourRect::getColourAtPoint( float x, float y ) const
	{
		Colour h1((d_top_right - d_top_left) * x + d_top_left);
		Colour h2((d_bottom_right - d_bottom_left) * x + d_bottom_left);
		return Colour((h2 - h1) * y + h1);
	}

	ColourRect ColourRect::getSubRectangle( float left, float right, float top, float bottom ) const
	{
		return ColourRect(
			getColourAtPoint(left, top),
			getColourAtPoint(right, top),
			getColourAtPoint(left, bottom),
			getColourAtPoint(right, bottom)
			);
	}

	bool ColourRect::isMonochromatic() const
	{
		return d_top_left == d_top_right &&
			d_top_left == d_bottom_left &&
			d_top_left == d_bottom_right;
	}

	void ColourRect::modulateAlpha(float alpha)
	{
		d_top_left.setAlpha(d_top_left.getAlpha()*alpha);
		d_top_right.setAlpha(d_top_right.getAlpha()*alpha);
		d_bottom_left.setAlpha(d_bottom_left.getAlpha()*alpha);
		d_bottom_right.setAlpha(d_bottom_right.getAlpha()*alpha);
	}

	ColourRect& ColourRect::operator *=(const ColourRect& other)
	{
		d_top_left *= other.d_top_left;
		d_top_right *= other.d_top_right;
		d_bottom_left *= other.d_bottom_left;
		d_bottom_right *= other.d_bottom_right;

		return *this;
	}
}