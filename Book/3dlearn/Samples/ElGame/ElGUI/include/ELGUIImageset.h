#ifndef __ELGUIImageset_H__
#define __ELGUIImageset_H__

#include "ELGUIImage.h"
#include "ELGUITexture.h"
#include "ELGUIImagesetManager.h"

namespace ELGUI
{
	class Imageset
	{
	private:
		typedef	std::map<std::string, Image> ImageRegistry;

		friend Imageset* ImagesetManager::createImageset(const std::string& name, Texture* texture);
		friend void ImagesetManager::destroyImageset(const std::string& name);

		Imageset(const std::string& name, Texture* texture);

	public:
		virtual ~Imageset();

	public:
		Texture* getTexture() const
		{
			return d_texture;
		}

		const std::string& getName() const
		{
			return d_name;
		}

		unsigned int getImageCount() const
		{
			return (unsigned int)d_images.size();
		}

		bool isImageDefined(const std::string& name) const
		{
			return d_images.find(name) != d_images.end();
		}

		Image& getImage(const std::string& name) const;

		void undefineImage(const std::string& name);

		void undefineAllImages();

		Size getImageSize(const std::string& name) const
		{
			return getImage(name).getSize();
		}

		float getImageWidth(const std::string& name) const
		{
			return getImage(name).getWidth();
		}

		float getImageHeight(const std::string& name) const
		{
			return getImage(name).getHeight();
		}

		Point getImageOffset(const std::string& name) const
		{
			return getImage(name).getOffsets();
		}

		float getImageOffsetX(const std::string& name) const
		{
			return getImage(name).getOffsetX();
		}

		float getImageOffsetY(const std::string& name) const
		{
			return getImage(name).getOffsetY();
		}

		void defineImage(const std::string& name, const Point& position, const Size& size, const Point& render_offset)
		{
			defineImage(name, Rect(position.d_x, position.d_y, position.d_x + size.d_width, position.d_y + size.d_height), render_offset);
		}

		void defineImage(const std::string& name, const Rect& image_rect, const Point& render_offset);

		void draw(const Rect& source_rect, const Rect& dest_rect, const Rect& clip_rect, const ColourRect& colours) const;

		bool isAutoScaled() const
		{
			return d_autoScale;
		}

		Size getNativeResolution() const
		{
			return Size(d_nativeHorzRes, d_nativeVertRes);
		}

		void setAutoScalingEnabled(bool setting);

		void setNativeResolution(const Size& size);

		void notifyScreenResolution(const Size& size);

	protected:
		void setTexture(Texture* texture);
		void Imageset::unload();
		void updateImageScalingFactors();

	protected:
		std::string				d_name;
		ImageRegistry			d_images;
		Texture*				d_texture;
		std::string				d_textureFilename;

		// auto-scaling fields
		bool					d_autoScale;
		float					d_horzScaling;
		float					d_vertScaling;
		float					d_nativeHorzRes;
		float					d_nativeVertRes;
	};
}

#endif //__ELGUIImageset_H__