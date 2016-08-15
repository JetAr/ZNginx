#include "ElSamplePCH.h"
#include "GUIRenderer.h"
#include "GUITexture.h"
#include "ElDeviceManager.h"

ElGUIRenderer::ElGUIRenderer()
: mQuadUsedIdx(0)
{

}

ElGUIRenderer::~ElGUIRenderer()
{
	ElScreenElementIterator i, iend;
	iend = mQuadList.end();
	for (i = mQuadList.begin(); i != iend; ++i)
	{
		ElSafeDelete(*i);
	}
	mQuadList.clear();
}

void ElGUIRenderer::addQuad(const ELGUI::Rect& dest_rect, const ELGUI::Texture* tex, const ELGUI::Rect& texture_rect, const ELGUI::ColourRect& colours)
{
	if (mQuadUsedIdx == mQuadList.size())
	{
		ElScreenElement* newQuad = ElNew ElScreenElement;
		mQuadList.push_back(newQuad);
	}

	float rendererWidth = getWidth();
	float rendererHeight = getHeight();

	ElScreenElement* quad = mQuadList[mQuadUsedIdx];
	quad->setRectangle(dest_rect.d_left / rendererWidth, dest_rect.d_top / rendererHeight, dest_rect.getWidth() / rendererWidth, dest_rect.getHeight() / rendererHeight);
	quad->setTextures(texture_rect.d_left, texture_rect.d_top, texture_rect.d_right, texture_rect.d_bottom);
	quad->setColor(D3DXCOLOR(colours.d_top_left.getRed(), colours.d_top_left.getGreen(), colours.d_top_left.getBlue(), colours.d_top_left.getAlpha()),
		D3DXCOLOR(colours.d_top_right.getRed(), colours.d_top_right.getGreen(), colours.d_top_right.getBlue(), colours.d_top_right.getAlpha()),
		D3DXCOLOR(colours.d_bottom_left.getRed(), colours.d_bottom_left.getGreen(), colours.d_bottom_left.getBlue(), colours.d_bottom_left.getAlpha()),
		D3DXCOLOR(colours.d_bottom_right.getRed(), colours.d_bottom_right.getGreen(), colours.d_bottom_right.getBlue(), colours.d_bottom_right.getAlpha()));
	quad->setTexture(((ElGUITexture*)tex)->getTexture());

	++mQuadUsedIdx;
}

void ElGUIRenderer::clearRenderList()
{
	mQuadUsedIdx = 0;
}

void ElGUIRenderer::doRender()
{
	// render each screen element in the list
	for (int i = 0; i < mQuadUsedIdx; ++i)
	{
		mQuadList[i]->render();
	}
}

ELGUI::Texture* ElGUIRenderer::createTexture(const std::string& filename)
{
	ELGUI::Texture* tex = new ElGUITexture;
	tex->loadFromFile(filename);
	return tex;
}

ELGUI::Texture* ElGUIRenderer::createTexture()
{
	ELGUI::Texture* tex = new ElGUITexture;
	return tex;
}

void ElGUIRenderer::destroyTexture(ELGUI::Texture* texture)
{
	delete texture;
}

float ElGUIRenderer::getWidth() const
{
	return ElDeviceManager::getSingleton().getBackBufferDesc().Width;
}

float ElGUIRenderer::getHeight() const
{
	return ElDeviceManager::getSingleton().getBackBufferDesc().Height;
}

ELGUI::Size ElGUIRenderer::getSize() const
{
	return ELGUI::Size(getWidth(), getHeight());
}

ELGUI::Rect ElGUIRenderer::getRect() const
{
	return ELGUI::Rect(0.0f, 0.0f, getWidth(), getHeight());
}