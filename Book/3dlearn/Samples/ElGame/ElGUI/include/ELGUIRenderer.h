#ifndef __ELGUIRenderer_H__
#define __ELGUIRenderer_H__

#include "ELGUITexture.h"
#include "ELGUIColourRect.h"
#include "ELGUIRect.h"
#include <string>

namespace ELGUI
{
	class Renderer
	{
	public:
		Renderer();
		virtual ~Renderer();

		virtual	void addQuad(const Rect& dest_rect, const Texture* tex, const Rect& texture_rect, const ColourRect& colours) = 0;
		virtual	void clearRenderList() = 0;
		virtual void doRender() = 0;
		
		virtual	Texture* createTexture(const std::string& filename) = 0;
		virtual	Texture* createTexture() = 0;
		virtual	void destroyTexture(Texture* texture) = 0;

		virtual float getWidth() const = 0;
		virtual float getHeight() const = 0;
		virtual Size getSize() const = 0;
		virtual Rect getRect() const = 0;
	};
}

#endif //__ELGUIRenderer_H__