#ifndef __ElSkybox_H__
#define __ElSkybox_H__

#include "ElD3DHeaders.h"
#include "ElTexture.h"
#include "ElCamera.h"
#include "ElRenderable.h"

class ElSkybox : public ElRenderable
{
public:
	enum SkyboxPlane
	{
		SKYBOX_PLANE_FRONT,
		SKYBOX_PLANE_RIGHT,
		SKYBOX_PLANE_BACK,
		SKYBOX_PLANE_LEFT,
		SKYBOX_PLANE_TOP,
		SKYBOX_PLANE_BOTTOM
	};

	struct VERTEX
	{
		float x, y, z;
		float u, v;

		VERTEX() {}
		VERTEX(float posX, float posY, float posZ, float texU, float texV) : x(posX), y(posY), z(posZ), u(texU), v(texV) {}

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
	};

public:
	ElSkybox();
	virtual ~ElSkybox();

	bool create(float sizeX, float sizeY, float sizeZ);
	bool setTexture(SkyboxPlane plane, LPCSTR filename);
	virtual void render(ElCamera* camera, TimeValue t);

protected:
	float mSizeX, mSizeY, mSizeZ;
	VERTEX* mVerties;
	ElTexture* mTextures[6];
};

#endif //__ElSkybox_H__