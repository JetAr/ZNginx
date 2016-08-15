#ifndef __ELGUIColour_H__
#define __ELGUIColour_H__

namespace ELGUI
{
	typedef unsigned int argb_t;

	class Colour
	{
	public:
		Colour();
		Colour(const Colour& val);
		Colour(float red, float green, float blue, float alpha = 1.0f);
		Colour(argb_t argb);

		argb_t	getARGB() const
		{
			argb_t d_argb = 0xff000000;
			//if (!d_argbValid)
			{
				d_argb = calculateARGB();
				//d_argbValid = true;
			}

			return d_argb;
		}

		float	getAlpha() const
		{
			return d_alpha;
		}
		float	getRed() const
		{
			return d_red;
		}
		float	getGreen() const
		{
			return d_green;
		}
		float	getBlue() const
		{
			return d_blue;
		}

		float	getHue() const;
		float	getSaturation() const;
		float	getLumination() const;

		void	setARGB(argb_t argb);
		inline void setAlpha(float alpha)
		{
			d_argbValid = false;
			d_alpha = alpha;
		}

		inline void setRed(float red)
		{   
			d_argbValid = false;
			d_red = red;
		}

		inline void setGreen(float green)
		{
			d_argbValid = false;
			d_green = green;
		}

		inline void setBlue(float blue)
		{
			d_argbValid = false;
			d_blue = blue;
		}

		inline void set(float red, float green, float blue, float alpha = 1.0f)
		{
			d_argbValid = false;
			d_alpha = alpha;
			d_red = red;
			d_green = green;
			d_blue = blue;
		}

		inline void setRGB(float red, float green, float blue)
		{
			d_argbValid = false;
			d_red = red;
			d_green = green;
			d_blue = blue;
		}

		inline void setRGB(const Colour& val)
		{
			d_red = val.d_red;
			d_green = val.d_green;
			d_blue = val.d_blue;
			if (d_argbValid)
			{
				d_argbValid = val.d_argbValid;
				//if (d_argbValid)
				//d_argb = (d_argb & 0xFF000000) | (val.d_argb & 0x00FFFFFF);
			}
		}

		void	setHSL(float hue, float saturation, float luminance, float alpha = 1.0f);

		void	invertColour();
		void	invertColourWithAlpha();

		inline Colour& operator=(argb_t val)
		{
			setARGB(val);
			return *this;
		}

		inline Colour& operator=(const Colour& val)
		{
			d_alpha = val.d_alpha;
			d_red   = val.d_red;
			d_green = val.d_green;
			d_blue  = val.d_blue;
			//d_argb  = val.d_argb;
			d_argbValid = val.d_argbValid;

			return *this;
		}

		inline Colour& operator&=(argb_t val)
		{
			setARGB(getARGB() & val);
			return *this;
		}

		inline Colour& operator&=(const Colour& val)
		{
			setARGB(getARGB() & val.getARGB());
			return *this;
		}

		inline Colour& operator|=(argb_t val)
		{
			setARGB(getARGB() | val);
			return *this;
		}

		inline Colour& operator|=(const Colour& val)
		{
			setARGB(getARGB() | val.getARGB());
			return *this;
		}

		inline Colour& operator<<=(int val)
		{
			setARGB(getARGB() << val);
			return *this;
		}

		inline Colour& operator>>=(int val)
		{
			setARGB(getARGB() >> val);
			return *this;
		}

		inline Colour operator+(const Colour& val) const
		{
			return Colour(
				d_red   + val.d_red, 
				d_green + val.d_green, 
				d_blue  + val.d_blue,
				d_alpha + val.d_alpha
				);
		}

		inline Colour operator-(const Colour& val) const
		{
			return Colour(
				d_red   - val.d_red,
				d_green - val.d_green,
				d_blue  - val.d_blue,
				d_alpha - val.d_alpha
				);
		}

		inline Colour operator*(const float val) const
		{       
			return Colour(
				d_red   * val, 
				d_green * val, 
				d_blue  * val,
				d_alpha * val 
				);  
		}

		inline Colour& operator*=(const Colour& val)
		{
			d_red *= val.d_red;
			d_blue *= val.d_blue;
			d_green *= val.d_green;
			d_alpha *= val.d_alpha;

			d_argbValid = false;

			return *this;
		}

		inline bool operator==(const Colour& rhs) const
		{
			return d_red   == rhs.d_red   &&
				d_green == rhs.d_green &&
				d_blue  == rhs.d_blue  &&
				d_alpha == rhs.d_alpha;
		}

		inline bool operator!=(const Colour& rhs) const
		{
			return !(*this == rhs);
		}

		operator argb_t() const
		{
			return getARGB();
		}

	protected:
		argb_t calculateARGB() const;

	protected:
		float d_alpha, d_red, d_green, d_blue;
		mutable bool d_argbValid;
	};
}

#endif //__ELGUIColour_H__