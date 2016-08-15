#ifndef __ELGUITexture_H__
#define __ELGUITexture_H__

#include <string>

namespace ELGUI
{
	class Texture
	{
	public:
		enum PixelFormat
		{
			PF_RGB,
			PF_RGBA
		};

		Texture();
		virtual ~Texture();

		virtual void loadFromFile(const std::string& filename) = 0;
		virtual void loadFromMemory(const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, PixelFormat pixelFormat) = 0;

		virtual	unsigned short getWidth() const = 0;
		virtual	unsigned short getHeight() const = 0;

		virtual unsigned short getOriginalWidth() const
		{
			return getWidth();
		}

		virtual float getXScale() const
		{
			return 1.0f / static_cast<float>(getOriginalWidth());
		}

		virtual unsigned short getOriginalHeight() const
		{
			return getHeight();
		}

		virtual float getYScale() const
		{
			return 1.0f / static_cast<float>(getOriginalHeight());
		}
	};
}

#endif //__ELGUITexture_H__