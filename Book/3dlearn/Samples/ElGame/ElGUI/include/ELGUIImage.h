#ifndef __ELGUIImage_H__
#define __ELGUIImage_H__

#include "ELGUIBase.h"
#include "ELGUISize.h"
#include "ELGUIRect.h"
#include "ELGUIVector.h"
#include "ELGUIColourRect.h"
#include <string>
#include <map>

namespace ELGUI
{
	class Imageset;

	class Image
	{
	public:
		Image() {}
		Image(const Imageset* owner, const std::string& name, const Rect& area, const Point& render_offset, float horzScaling = 1.0f, float vertScaling = 1.0f);
		Image(const Image& image);
		virtual ~Image();

		void setArea(Rect area);
		void setOffset(Point render_offset);
		
		Size getSize() const
		{
			return Size(d_scaledWidth, d_scaledHeight);
		}

		float getWidth() const
		{
			return d_scaledWidth;
		}

		float getHeight() const
		{
			return d_scaledHeight;
		}

		Point getOffsets() const
		{
			return d_scaledOffset;
		}

		float getOffsetX() const
		{
			return d_scaledOffset.d_x;
		}

		float getOffsetY() const
		{
			return d_scaledOffset.d_y;
		}

		void draw(const Rect& dest_rect, const Rect& clip_rect, const ColourRect& colours) const;
		void draw(const Point& position, const Size& size, const Rect& clip_rect, const ColourRect& colours) const
		{
			draw(Rect(position.d_x, position.d_y, position.d_x + size.d_width, position.d_y + size.d_height), clip_rect, colours);
		}
		
		const std::string& getName() const;
		const std::string& getImagesetName() const;

		const Imageset*	getImageset() const
		{
			return d_owner;
		}

		const Rect& getSourceTextureArea() const;

		friend class std::map<std::string, Image>;
		friend struct std::pair<const std::string, Image>;

	private:
		friend class Imageset;

		void setHorzScaling(float factor);
		void setVertScaling(float factor);

	protected:
		const Imageset*	d_owner;
		Rect			d_area;
		Point			d_offset;

		// image auto-scaling fields.
		float			d_scaledWidth;
		float			d_scaledHeight;
		Point			d_scaledOffset;
		std::string		d_name;
	};
}

#endif //__ELGUIImage_H__