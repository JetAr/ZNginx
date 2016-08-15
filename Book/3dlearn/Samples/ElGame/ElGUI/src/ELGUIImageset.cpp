#include "ELGUIPCH.h"
#include "ELGUIImageset.h"
#include "ELGUISystem.h"
#include "ELGUISystem.h"

namespace ELGUI
{
	Imageset::Imageset(const std::string& name, Texture* texture)
	: d_name(name)
	, d_texture(texture)
	{
		assert(d_texture && "Imageset::Imageset - Texture object supplied for Imageset creation must be valid.");
		
		// defaults for scaling options
		d_autoScale = false;
		setNativeResolution(Size(DefaultNativeHorzRes, DefaultNativeVertRes));
	}

	Imageset::~Imageset()
	{
		unload();
	}

	void Imageset::setTexture(Texture* texture)
	{
		assert(d_texture && "Imageset::setTexture - Texture object supplied for Imageset creation must be valid.");
		
		d_texture = texture;
	}

	Image& Imageset::getImage(const std::string& name) const
	{
		ImageRegistry::const_iterator pos = d_images.find(name);

		if (pos == d_images.end())
		{
			assert(0 && std::string("Imageset::getImage - The Image named '" + name + "' could not be found in Imageset '" + d_name + "'.").c_str());
		}

		return *(Image*)&pos->second;	// anyway else...?
	}

	void Imageset::defineImage(const std::string& name, const Rect& image_rect, const Point& render_offset)
	{
		//if (isImageDefined(name))
		//{
		//	assert(0 && std::string("Imageset::defineImage - An image with the name '" + name + "' already exists in Imageset '" + d_name + "'.").c_str());
		//}

		// get scaling factors
		float hscale = d_autoScale ? d_horzScaling : 1.0f;
		float vscale = d_autoScale ? d_vertScaling : 1.0f;

		// add the Image definition
		d_images[name.c_str()] = Image(this, name, image_rect, render_offset, hscale, vscale);
	}

	void Imageset::draw(const Rect& source_rect, const Rect& dest_rect, const Rect& clip_rect, const ColourRect& colours) const
	{
		// get the rect area that we will actually draw to (i.e. perform clipping)
		Rect final_rect(dest_rect.getIntersection(clip_rect));

		// check if rect was totally clipped
		if (final_rect.getWidth() != 0)
		{
			// Obtain correct scale vlaue from the texture
			const float x_scale = d_texture->getXScale();
			const float y_scale = d_texture->getYScale();

			float tex_per_pix_x = source_rect.getWidth() / dest_rect.getWidth();
			float tex_per_pix_y = source_rect.getHeight() / dest_rect.getHeight();

			// calculate final, clipped, texture co-ordinates
			
			// This makes use of some tricks that we offset 0.5-pixel to each source image rect so that
			// some pixel blending at the edge will be avoid
			Rect  tex_rect((source_rect.d_left + 0.5f + ((final_rect.d_left - dest_rect.d_left) * tex_per_pix_x)) * x_scale,
				(source_rect.d_top + 0.5f + ((final_rect.d_top - dest_rect.d_top) * tex_per_pix_y)) * y_scale,
				(source_rect.d_right + ((final_rect.d_right - dest_rect.d_right) * tex_per_pix_x)) * x_scale,
				(source_rect.d_bottom + ((final_rect.d_bottom - dest_rect.d_bottom) * tex_per_pix_y)) * y_scale);

			final_rect.d_left	= PixelAligned(final_rect.d_left);
			final_rect.d_right	= PixelAligned(final_rect.d_right);
			final_rect.d_top	= PixelAligned(final_rect.d_top);
			final_rect.d_bottom	= PixelAligned(final_rect.d_bottom);

			// queue a quad to be rendered
			System::getSingleton().getRenderer()->addQuad(final_rect, d_texture, tex_rect, colours);
		}
	}

	void Imageset::unload()
	{
		undefineAllImages();

		// cleanup texture
		System::getSingleton().getRenderer()->destroyTexture(d_texture);
		d_texture = 0;
	}

	void Imageset::updateImageScalingFactors()
	{
		float hscale, vscale;

		if (d_autoScale)
		{
			hscale = d_horzScaling;
			vscale = d_vertScaling;
		}
		else
		{
			hscale = vscale = 1.0f;
		}

		ImageRegistry::iterator pos = d_images.begin(), end = d_images.end();
		for(; pos != end; ++pos)
		{
			pos->second.setHorzScaling(hscale);
			pos->second.setVertScaling(vscale);
		}
	}

	void Imageset::setAutoScalingEnabled(bool setting)
	{
		if (setting != d_autoScale)
		{
			d_autoScale = setting;
			updateImageScalingFactors();
		}
	}

	void Imageset::setNativeResolution(const Size& size)
	{
		d_nativeHorzRes = size.d_width;
		d_nativeVertRes = size.d_height;

		// re-calculate scaling factors & notify images as required
		notifyScreenResolution(System::getSingleton().getRenderer()->getSize());
	}

	void Imageset::notifyScreenResolution(const Size& size)
	{
		d_horzScaling = size.d_width / d_nativeHorzRes;
		d_vertScaling = size.d_height / d_nativeVertRes;

		if (d_autoScale)
		{
			updateImageScalingFactors();
		}
	}

	void Imageset::undefineImage(const std::string& name)
	{
		d_images.erase(name.c_str());
	}


	void Imageset::undefineAllImages()
	{
		d_images.clear();
	}
}