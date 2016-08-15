#ifndef __ELGUIColourRect_H__
#define __ELGUIColourRect_H__

#include "ELGUIColour.h"

namespace ELGUI
{
	class ColourRect
	{
	public:
		ColourRect();
		ColourRect(const Colour& col);
		ColourRect(const Colour& top_left, const Colour& top_right, const Colour& bottom_left, const Colour& bottom_right);

		void setAlpha(float alpha);
		void setTopAlpha(float alpha);
		void setBottomAlpha(float alpha);
		void setLeftAlpha(float alpha);
		void setRightAlpha(float alpha);

		void setColours(const Colour& col);

		ColourRect getSubRectangle( float left, float right, float top, float bottom ) const;
		Colour getColourAtPoint( float x, float y ) const;

		bool isMonochromatic() const;

		void modulateAlpha(float alpha);

		ColourRect& operator*=(const ColourRect& other);

		Colour	d_top_left, d_top_right, d_bottom_left, d_bottom_right;
	};
}

#endif //__ELGUIColourRect_H__