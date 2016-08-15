#ifndef __ElSceneManager_H__
#define __ElSceneManager_H__

#include "ElDefines.h"
#include "ElSingleton.h"
#include "ElSceneNode.h"
#include "ElLight.h"
#include "ElCamera.h"
#include "ElSkybox.h"
#include "ElTerrain.h"
#include "ElWater.h"
#include "ElRaySceneQuery.h"
#include "ElRenderQueue.h"
#include <vector>

class ElSceneManager
{
public:
	ElSceneManager();
	virtual ~ElSceneManager();

	ElSceneNode*	getRootSceneNode();
	
	void			setCamera(ElCamera* camera);
	ElCamera*		getCamera();

	void			setSkybox(ElSkybox* skyBox);
	ElSkybox*		getSkybox();

	void			setTerrain(ElTerrain* terrain);
	ElTerrain*		getTerrain();

	void			setWater(ElWater* water);
	ElWater*		getWater();

	void			setFog(DWORD mode = D3DFOG_NONE, ColorValue color = D3DCOLOR_RGBA(255, 255, 255, 0), float linearStart = 100.0f, float linearEnd = 160.0f, float expDensity = 0.1f);
	void			useFog(bool use = true);

	void			setAmbientLightColor(const ColorValue& color);
	const ColorValue& getAmbientLightColor() const;
	bool			useAmbientLight();

	void			setLights(const ElLightList& lts);
	void			addLight(const D3DLIGHT9& lt);
	const ElLightList& getLights() const;
	void			usetLights();

	void			update();
	void			render(TimeValue t);

	void			intersects(ElRaySceneQuery& raySceneQuery);

	virtual ElRenderQueue* getRenderQueue();

protected:
	void			setD3D9Light(DWORD index, const D3DLIGHT9* lt);

	virtual void	_prepareRenderQueue();
	virtual void	_findVisibleObjects(ElCamera * cam);
	virtual void	_renderVisibleObjects(TimeValue t);

protected:
	// Camera in progress
	ElCamera*		mCamera;

	// Root scene node
	ElSceneNode*	mSceneRoot;

	// Scene elements
	ElSkybox*		mSkybox;
	ElTerrain*		mTerrain;
	ElWater*		mWater;
	bool			mReflectMovables;

	// Current ambient light
	ColorValue		mAmbientLight;

	// Current light list
	ElLightList		mLightList;
	unsigned short	mCurrentLights;

	// Fog
	DWORD			mFogMode;
	ColorValue		mFogColour;
	float			mFogStart;
	float			mFogEnd;
	float			mFogDensity;

	// Queue of objects for rendering
	ElRenderQueue*	mRenderQueue;
};

#endif //__ElSceneManager_H__