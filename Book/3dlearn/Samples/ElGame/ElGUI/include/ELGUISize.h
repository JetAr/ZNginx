#ifndef __ELGUISize_H__
#define __ELGUISize_H__

namespace ELGUI
{
	class Size
	{
	public:
		Size() {}
		Size(float width, float height) : d_width(width), d_height(height) {}

		bool operator==(const Size& other) const
		{
			return d_width == other.d_width && d_height == other.d_height;
		}
		bool operator!=(const Size& other) const
		{
			return !operator==(other);
		}

		float d_width, d_height;
	};
}

#endif //__ELGUISize_H__