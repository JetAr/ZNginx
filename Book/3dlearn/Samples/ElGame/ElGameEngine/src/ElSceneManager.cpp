#include "ElApplicationPCH.h"
#include "ElSceneManager.h"
#include "ElDeviceManager.h"

ElSceneManager::ElSceneManager()
: mSceneRoot(NULL)
, mCamera(NULL)
, mSkybox(NULL)
, mTerrain(NULL)
, mWater(NULL)
, mReflectMovables(false)
, mAmbientLight(D3DCOLOR_RGBA(128, 128, 128, 0))
, mCurrentLights(0)
, mFogMode(D3DFOG_NONE)
, mFogColour(D3DCOLOR_RGBA(255, 255, 255, 255))
, mFogStart(100.0f)
, mFogEnd(160.0f)
, mFogDensity(0.1f)
, mRenderQueue(NULL)
{

}

ElSceneManager::~ElSceneManager()
{
	ElSafeDelete(mRenderQueue);

	ElSafeDelete(mSkybox);
	ElSafeDelete(mTerrain);
	ElSafeDelete(mWater);
	
	if (mCamera && !mCamera->getParentNode())
	{
		ElSafeDelete(mCamera);
	}

	if (mSceneRoot)
		mSceneRoot->destroy();
	ElSafeDelete(mSceneRoot);
}

ElSceneNode* ElSceneManager::getRootSceneNode()
{
	if (!mSceneRoot)
	{
		// Create root scene node
		mSceneRoot = ElNew ElSceneNode("ElSceneRoot");
	}

	return mSceneRoot;
}

void ElSceneManager::setCamera(ElCamera* camera)
{
	assert(camera);
	mCamera = camera;
}

ElCamera* ElSceneManager::getCamera()
{
	return mCamera;
}

void ElSceneManager::setSkybox(ElSkybox* skyBox)
{
	mSkybox = skyBox;
}

ElSkybox* ElSceneManager::getSkybox()
{
	return mSkybox;
}

void ElSceneManager::setTerrain(ElTerrain* terrain)
{
	mTerrain = terrain;
}

ElTerrain* ElSceneManager::getTerrain()
{
	return mTerrain;
}

void ElSceneManager::setWater(ElWater* water)
{
	mWater = water;
}

ElWater* ElSceneManager::getWater()
{
	return mWater;
}

void ElSceneManager::setFog(DWORD mode, ColorValue color, float linearStart, float linearEnd, float expDensity)
{
	mFogMode = mode;
	mFogColour = color;
	mFogStart = linearStart;
	mFogEnd = linearEnd;
	mFogDensity = expDensity;
}

void ElSceneManager::useFog(bool use /* = true */)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (use)
	{
		if (mFogMode != D3DFOG_NONE)
		{
			d3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
			d3dDevice->SetRenderState(D3DRS_FOGCOLOR, mFogColour);
			d3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, mFogMode);
			if (mFogMode == D3DFOG_LINEAR)
			{
				d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)&mFogStart);
				d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)&mFogEnd);
			}
			else
				d3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&mFogDensity);
		}
	}
	else
		d3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

void ElSceneManager::setAmbientLightColor(const ColorValue& color)
{
	mAmbientLight = color;
}

const ColorValue& ElSceneManager::getAmbientLightColor() const
{
	return mAmbientLight;
}

bool ElSceneManager::useAmbientLight()
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (FAILED(d3dDevice->SetRenderState(D3DRS_AMBIENT, mAmbientLight)))
		return false;
	return true;
}

void ElSceneManager::setLights(const ElLightList& lts)
{
	mLightList = lts;
}

void ElSceneManager::addLight(const D3DLIGHT9& lt)
{
	mLightList.push_back(lt);
}

const ElLightList& ElSceneManager::getLights() const
{
	return mLightList;
}

void ElSceneManager::usetLights()
{
	ElLightConstIterator i, iend;
	iend = mLightList.end();
	DWORD num = 0;
	for (i = mLightList.begin(); i != iend; ++i, ++num)
	{
		setD3D9Light(num, &(*i));
	}
	// Disable extra lights
	for (; num < mCurrentLights; ++num)
	{
		setD3D9Light(num, NULL);
	}
	mCurrentLights = mLightList.size();
}

void ElSceneManager::update()
{
	if (mSceneRoot)
		mSceneRoot->update();

	if (mTerrain)
	{
		// for now, we only use the first directional light in the lightList
		// for the terrain lighting
		D3DLIGHT9* diffuse = NULL;
		ElLightIterator i, iend;
		iend = mLightList.end();
		for (i = mLightList.begin(); i != iend; ++i)
		{
			if (i->Type == D3DLIGHT_DIRECTIONAL)
			{
				diffuse = &(*i);
				break;
			}
		}
		mTerrain->update(diffuse, mAmbientLight);
	}
}

void ElSceneManager::render(TimeValue t)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();

	// Set world transformation
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	d3dDevice->SetTransform(D3DTS_WORLD, &world);

	// Issue view / projection changes if any
	if (mCamera)
	{
		d3dDevice->SetTransform(D3DTS_VIEW, &mCamera->getViewMatrix());
		d3dDevice->SetTransform(D3DTS_PROJECTION, &mCamera->getProjectionMatrix());
	}

	// Set light hash so even if light list is the same, we refresh the content every frame
	usetLights();

	// Tell params about current ambient light
	useAmbientLight();

	// Set fog states
	useFog();

	// Render skyBox, terrain, movable objects(if mReflectMovables set)
	// onto water environment texture, if there is one
	if (mWater)
	{
		mWater->beginScene(mCamera);
		_prepareRenderQueue();
		if (mSkybox)
			getRenderQueue()->getQueueGroup(RENDER_QUEUE_SKIES_EARLY)->addRenderable(mSkybox);
		if (mTerrain)
			getRenderQueue()->getQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_1)->addRenderable(mTerrain);
		if (mReflectMovables && mSceneRoot)
			mSceneRoot->_findVisibleObjects(mCamera, getRenderQueue());
		_renderVisibleObjects(t);
		mWater->endScene(mCamera);
	}
	
	// Prepare render queue for receiving new objects
	_prepareRenderQueue();

	// Parse the scene and tag visibles
	_findVisibleObjects(mCamera);

	// Render scene content
	_renderVisibleObjects(t);
}

void ElSceneManager::intersects(ElRaySceneQuery& raySceneQuery)
{
	if (mSceneRoot)
		mSceneRoot->intersects(raySceneQuery);
}

ElRenderQueue* ElSceneManager::getRenderQueue()
{
	if (!mRenderQueue)
	{
		mRenderQueue = ElNew ElRenderQueue();
	}
	return mRenderQueue;
}

void ElSceneManager::setD3D9Light(DWORD index, const D3DLIGHT9* lt)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (!lt)
		d3dDevice->LightEnable(index, FALSE);
	else
	{
		d3dDevice->SetLight(index, lt);
		d3dDevice->LightEnable(index, TRUE);
	}
}

void ElSceneManager::_prepareRenderQueue()
{
	// Clear the render queue
	getRenderQueue()->clear();
}

void ElSceneManager::_findVisibleObjects(ElCamera * cam)
{
	// Queue skies, terrain, etc.
	if (mSkybox)
		getRenderQueue()->getQueueGroup(RENDER_QUEUE_SKIES_EARLY)->addRenderable(mSkybox);
	if (mTerrain)
		getRenderQueue()->getQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_1)->addRenderable(mTerrain);
	if (mWater)
		getRenderQueue()->getQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_2)->addRenderable(mWater);

	// Tell nodes to find, cascade down all nodes
	if (mSceneRoot)
		mSceneRoot->_findVisibleObjects(cam, getRenderQueue());
}

void ElSceneManager::_renderVisibleObjects(TimeValue t)
{
	ElRenderQueue* queue = getRenderQueue();

	// Sort transparent-queue first
	queue->getQueueGroup(RENDER_QUEUE_TRANSPARENT_1)->sort(mCamera);

	// Render according to skies-solid-transparent... sequence
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();

	{
		useFog(false);
		queue->getQueueGroup(RENDER_QUEUE_SKIES_EARLY)->renderObjectsList(mCamera, t);
		useFog();
	}

	for (unsigned int groupId = RENDER_QUEUE_SKIES_EARLY + 1; groupId < RENDER_QUEUE_TRANSPARENT_1; ++groupId)
	{
		queue->getQueueGroup(groupId)->renderObjectsList(mCamera, t);
	}

	{
		d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		queue->getQueueGroup(RENDER_QUEUE_TRANSPARENT_1)->renderObjectsList(mCamera, t);
		d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	for (unsigned int groupId = RENDER_QUEUE_TRANSPARENT_1 + 1; groupId < RENDER_QUEUE_TRANSPARENT_1; ++groupId)
	{
		queue->getQueueGroup(groupId)->renderObjectsList(mCamera, t);
	}
}