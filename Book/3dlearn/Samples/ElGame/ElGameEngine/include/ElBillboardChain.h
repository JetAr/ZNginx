#ifndef __ElBillboardChain_H__
#define __ElBillboardChain_H__

#include "ElMovableObject.h"
#include "ElRenderable.h"
#include "ElArray.h"
#include <vector>

class ElBillboardChain : public ElMovableObject, public ElRenderable
{
public:
	struct VERTEX
	{
		D3DXVECTOR3			pos;
		DWORD				color;
		D3DXVECTOR2			tex;

		static const DWORD	FVF =  D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	};

	enum TextureCoordMode
	{
		TextCoord_CustomX,
		TextCoord_CustomY
	};

	class Element
	{
	public:
		Element();
		Element(D3DXVECTOR3 position,
			float width,
			float texCoord,
			ColorValue colour);

		D3DXVECTOR3 position;
		float width;
		float texCoord;
		ColorValue colour;
	};
	typedef std::vector<Element> ElementList;

public:
	ElBillboardChain();
	virtual ~ElBillboardChain();

	virtual void setMaxChainElements(size_t maxElements);

	virtual void addChainElement(const Element& billboardChainElement);
	virtual void removeChainElement(size_t elementIndex);
	virtual size_t getNumChainElements() const;
	virtual void updateChainElement(size_t elementIndex, const Element& billboardChainElement);
	virtual const Element& getChainElement(size_t elementIndex) const;
	virtual void clearChainElements();

	virtual void setTexture(IDirect3DTexture9* texture);
	virtual void setTextureCoordMode(unsigned char mode);

	virtual void setUseChainDirection(bool setting);

	void needUpdate();

	virtual void update();
	virtual void render(ElCamera* camera, TimeValue t);

	virtual void _updateRenderQueue(ElRenderQueue* queue, ElCamera* cam);
	virtual float getSquaredViewDepth(ElCamera* cam) const;

	virtual const ElAxisAlignedBox& getBoundingBox();

protected:
	virtual void setupChainContainers();
	void _setupVertices(ElCamera* cam);
	void _setupBoundingBox();

protected:
	bool			mNeedUpdateBounding;
	bool			mNeedUpdateVertices;

	size_t			mMaxElements;

	// The list holding the chain elements
	ElementList		mChainElementList;

	IDirect3DTexture9* mTexture;
	unsigned char	mTexCoordMode;
	ElArray<VERTEX>	mVertices;

	bool			mUseChainDirection;

	ElAxisAlignedBox mAABB;
};

#endif //__ElBillboardChain_H__