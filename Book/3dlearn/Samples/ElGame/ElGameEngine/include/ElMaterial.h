#ifndef __ElMaterial_H__
#define __ElMaterial_H__

#include "ElD3DHeaders.h"
#include "ElTexture.h"
#include <vector>

class ElMaterial
{
	typedef std::vector<ElMaterial*> SubMtlList;
	typedef std::vector<ElMaterial*>::iterator SubMtlIterator;	

public:
	ElMaterial();
	virtual ~ElMaterial();

	void			setAmbient(D3DCOLORVALUE& ambient);
	void			setDiffuse(D3DCOLORVALUE& diffuse);
	void			setSpecular(D3DCOLORVALUE& specular);
	void			setTexture(LPCSTR filename);
	bool			addSubMtl(ElMaterial* mtl);

	D3DCOLORVALUE	getAmbient();
	D3DCOLORVALUE	getDiffuse();
	D3DCOLORVALUE	getSpecular();
	ElTexture*		getTexture();

	bool			hasSubMtl() { return !mSubMtls.empty(); }
	int				getSubNum() { return mSubMtls.size(); }
	ElMaterial*		getSubMtl(int i) { return mSubMtls[i]; }

	D3DMATERIAL9*	getD3DMaterial();

protected:
	void			_needUpdate();
	void			_update();

protected:
	bool				mNeedUpdate;

	D3DCOLORVALUE		mAmbient;
	D3DCOLORVALUE		mDiffuse;
	D3DCOLORVALUE		mSpecular;
	D3DMATERIAL9		mD3DMaterial;
	ElTexture*			mTexture;

	SubMtlList			mSubMtls;
};

typedef std::vector<ElMaterial*> ElMaterialList;
typedef std::vector<ElMaterial*>::iterator ElMaterialIterator;

#endif //__ElMaterial_H__