#include "ElApplicationPCH.h"
#include "ElMaterial.h"
#include "ElDefines.h"

using namespace std;

ElMaterial::ElMaterial()
: mTexture(NULL)
{
	ZeroMemory(&mAmbient, sizeof(D3DCOLORVALUE));
	ZeroMemory(&mDiffuse, sizeof(D3DCOLORVALUE));
	ZeroMemory(&mSpecular, sizeof(D3DCOLORVALUE));

	_needUpdate();
}

ElMaterial::~ElMaterial()
{
	ElSafeDelete(mTexture);

	int numSub = mSubMtls.size();
	for (int i = 0; i < numSub; ++i)
	{
		ElSafeDelete(mSubMtls[i]);
	}
	mSubMtls.clear();
}

void ElMaterial::setAmbient(D3DCOLORVALUE& ambient)
{
	mAmbient = ambient;
	_needUpdate();
}

void ElMaterial::setDiffuse(D3DCOLORVALUE& diffuse)
{
	mDiffuse = diffuse;
	_needUpdate();
}

void ElMaterial::setSpecular(D3DCOLORVALUE& specular)
{
	mSpecular = specular;
	_needUpdate();
}

void ElMaterial::setTexture(LPCSTR filename)
{
	mTexture = ElNew ElTexture;
	if (!mTexture->loadFromFile(filename))
		ElSafeDelete(mTexture);
}

bool ElMaterial::addSubMtl(ElMaterial* mtl)
{
	int numSub = mSubMtls.size();
	for (int i = 0; i < numSub; ++i)
	{
		if (mSubMtls[i] == mtl)
			return false;
	}
	mSubMtls.push_back(mtl);

	return true;
}

D3DCOLORVALUE ElMaterial::getAmbient()
{
	_update();
	return mAmbient;
}

D3DCOLORVALUE ElMaterial::getDiffuse()
{
	_update();
	return mDiffuse;
}

D3DCOLORVALUE ElMaterial::getSpecular()
{
	_update();
	return mSpecular;
}

ElTexture* ElMaterial::getTexture()
{
	return mTexture;
}

D3DMATERIAL9* ElMaterial::getD3DMaterial()
{
	_update();
	return &mD3DMaterial;
}

void ElMaterial::_needUpdate()
{
	mNeedUpdate = true;
}

void ElMaterial::_update()
{
	if (mNeedUpdate)
	{
		ZeroMemory(&mD3DMaterial, sizeof(D3DMATERIAL9));
		mD3DMaterial.Ambient = mAmbient;
		mD3DMaterial.Diffuse = mDiffuse;
		mD3DMaterial.Specular = mSpecular;

		mNeedUpdate = false;
	}
}