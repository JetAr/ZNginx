#include "ElApplicationPCH.h"
#include "ElApplication.h"
#include "ElApplicationHeaders.h"
#include <limits>

ElApplication::ElApplication()
: mCamera(NULL)
, mScene(NULL)
, mZDefaultEnable(D3DZB_TRUE)
, mFillMode(D3DFILL_SOLID)
, mLastTime(0)
, mCurrentTime(0)
, mFrameTime(0)
, mAccumTime(0)
, mPause(false)
, mAccumTimeWithPause(0)
, mMinFramePeriod(0)
, mLastFrame(0)
, mOrthoCamera(NULL)
, mReferenceWindow(NULL)
{

}

ElApplication::~ElApplication()
{

}

bool ElApplication::initialize(HWND wnd)
{
	mReferenceWindow = wnd;

	if (!createDevice())
		return false;

	if (!createCamera())
		return false;

	if (!createScene())
		return false;

	if (!_setScreenSpaceCamera())
		return false;

	return true;
}

void ElApplication::terminate()
{
	for (ElScreenElementIterator i = mScreenElements.begin(); i != mScreenElements.end(); ++i)
	{
		ElSafeDelete(*i);
	}
	mScreenElements.clear();

	ElSafeDelete(mScene);

	ElShutDownSingleton(ElNameGenerator);
	ElShutDownSingleton(ElMeshManager);
	ElShutDownSingleton(ElSkeletonManager);
	ElShutDownSingleton(ElMaterialManager);
	ElShutDownSingleton(ElDeviceManager);
}

void ElApplication::process()
{
	// MeasureTime returns false if the frame rate is
	// over the pre-set limit
	if (!_measureTime())
		return;

	beginFrame();

	beginUpdate();
	update();
	endUpdate();

	beginRender();
	render();
	endRender();

	displayFrame();
	endFrame();
}

void ElApplication::beginFrame()
{

}

void ElApplication::beginUpdate()
{

}

void ElApplication::update()
{
	if (mScene)
		mScene->update();
}

void ElApplication::endUpdate()
{

}

void ElApplication::beginRender()
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(192, 192, 192), 1.0f, 0);

	d3dDevice->SetRenderState(D3DRS_ZENABLE, mZDefaultEnable);

	d3dDevice->SetRenderState(D3DRS_FILLMODE, mFillMode);

	// enable alpha blending
	d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
}

void ElApplication::render()
{
	renderScene();
	renderScreenItems();
}

void ElApplication::endRender()
{
	
}

void ElApplication::renderScene()
{
	if (mScene)
		mScene->render(getCurrentTime());
}

void ElApplication::renderScreenItems()
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (SUCCEEDED(d3dDevice->BeginScene()))
	{
		D3DXMATRIX m;
		d3dDevice->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&m));

		// disable lighting to ignore any materials
		d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		// set texture filter
		d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

		d3dDevice->SetFVF(ElScreenElement::VERTEX::FVF);

		_useScreenSpaceCamera();

		_renderScreenElements();

		d3dDevice->EndScene();

		// reset render states
		d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

		d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	}
}

void ElApplication::displayFrame()
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void ElApplication::endFrame()
{

}

void ElApplication::setPause(bool pause)
{
	mPause = pause;
}

bool ElApplication::getPause() const
{
	return mPause;
}

TimeValue ElApplication::getCurrentTime(bool ignorePaused /* = false */) const
{
	return !ignorePaused ? mAccumTimeWithPause : mAccumTime;
}

ElCamera* ElApplication::getCamera()
{
	return mCamera;
}

ElSceneManager* ElApplication::getScene()
{
	return mScene;
}

DWORD ElApplication::getFillMode() const
{
	return mFillMode;
}

void ElApplication::setFillMode(DWORD fm)
{
	mFillMode = fm;
}

void ElApplication::addScreenElement(ElScreenElement* element)
{
	if (element)
		mScreenElements.push_back(element);
}

HWND ElApplication::getReferenceWindow() const
{
	return mReferenceWindow;
}

bool ElApplication::createDevice()
{
	if (!ElDeviceManager::getSingleton().createDevice(mReferenceWindow))
		return false;
}

bool ElApplication::createCamera()
{
	ElSafeDelete(mCamera);
	mCamera = ElNew ElCamera;

	const D3DSURFACE_DESC& backBufferDesc = ElDeviceManager::getSingleton().getBackBufferDesc();
	float aspect = (float)backBufferDesc.Width / (float)backBufferDesc.Height;
	mCamera->setProjectionParameters(ElCamera::PT_PERSPECTIVE, D3DX_PI / 4, aspect, 0.1f, 500.0f);

	return true;
}

bool ElApplication::createScene()
{
	ElSafeDelete(mScene);
	mScene = ElNew ElSceneManager;

	// Set mCamera as the scene's default camera
	mScene->setCamera(mCamera);

	return true;
}

bool ElApplication::_measureTime()
{
	if (mLastTime == 0)
	{
		mLastTime = GetTickCount();
		mAccumTime = 0;
		mAccumTimeWithPause = 0;
	}

	// measure time
	mCurrentTime = GetTickCount();
	TimeValue deltaTime = mCurrentTime - mLastTime;
	if (deltaTime < 0)
		deltaTime = 0;
	mLastTime = mCurrentTime;
	mAccumTime += deltaTime;
	if (!mPause)
		mAccumTimeWithPause += deltaTime;

	// frame rate limiter
	if (mAccumTime < mLastFrame + mMinFramePeriod)
		return false;

	mFrameTime = mAccumTime - mLastFrame;
	mLastFrame = mAccumTime;

	return true;
}

bool ElApplication::_setScreenSpaceCamera()
{
	if (!mOrthoCamera)
		mOrthoCamera = ElNew ElCamera;

	const D3DSURFACE_DESC& backBufferDesc = ElDeviceManager::getSingleton().getBackBufferDesc();
	mOrthoCamera->setProjectionParameters(ElCamera::PT_ORTHOGRAPHIC, backBufferDesc.Width, backBufferDesc.Height, 0.0f, 1.0f);
	mOrthoCamera->setPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	return true;
}

void ElApplication::_useScreenSpaceCamera()
{
	if (mOrthoCamera)
	{
		IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
		d3dDevice->SetTransform(D3DTS_VIEW, &mOrthoCamera->getViewMatrix());
		d3dDevice->SetTransform(D3DTS_PROJECTION, &mOrthoCamera->getProjectionMatrix());
	}
}

void ElApplication::_renderScreenElements()
{
	// render each screen element in the list
	ElScreenElementIterator i, iend;
	iend = mScreenElements.end();
	for (i = mScreenElements.begin(); i != iend; ++i)
		(*i)->render();
}
