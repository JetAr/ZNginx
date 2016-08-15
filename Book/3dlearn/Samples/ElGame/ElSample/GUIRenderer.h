#ifndef __ElGUIRenderer_H__
#define __ElGUIRenderer_H__

#include "ELGUIRenderer.h"
#include "ElScreenElement.h"

class ElGUITexture;

class ElGUIRenderer : public ELGUI::Renderer
{
public:
	ElGUIRenderer();
	virtual ~ElGUIRenderer();

	virtual	void addQuad(const ELGUI::Rect& dest_rect, const ELGUI::Texture* tex, const ELGUI::Rect& texture_rect, const ELGUI::ColourRect& colours);
	virtual	void clearRenderList();
	virtual void doRender();

	virtual	ELGUI::Texture* createTexture(const std::string& filename);
	virtual	ELGUI::Texture* createTexture();
	virtual	void destroyTexture(ELGUI::Texture* texture);

	virtual float getWidth() const;
	virtual float getHeight() const;
	virtual ELGUI::Size getSize() const;
	virtual ELGUI::Rect getRect() const;

protected:
	ElScreenElementList			mQuadList;
	unsigned int				mQuadUsedIdx;
};

#endif //__ElGUIRenderer_H__