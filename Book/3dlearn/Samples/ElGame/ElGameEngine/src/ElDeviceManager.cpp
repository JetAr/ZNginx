#include "ElApplicationPCH.h"
#include "ElDeviceManager.h"
#include "ElDefines.h"

ElImplementSingleton(ElDeviceManager);

ElDeviceManager::ElDeviceManager()
: mDevice(NULL)
{
	ZeroMemory(&mBackBufferDesc, sizeof(mBackBufferDesc));
	ZeroMemory(&mCaps, sizeof(mCaps));
}

ElDeviceManager::~ElDeviceManager()
{
	ElSafeRelease(mDevice);
}

bool ElDeviceManager::createDevice(HWND wnd)
{
	IDirect3D9* d3d;

	if (NULL == (d3d = Direct3DCreate9(D3D9b_SDK_VERSION)))
		return false;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE; 
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
		D3DCREATE_MIXED_VERTEXPROCESSING,
		&d3dpp, &mDevice)))
	{
		return false;
	}

	_setBackBufferDesc();
	_setDeviceCaps();

	return true;
}

const D3DSURFACE_DESC& ElDeviceManager::getBackBufferDesc() const
{
	return mBackBufferDesc;
}

const D3DCAPS9& ElDeviceManager::getDeviceCaps() const
{
	return mCaps;
}

IDirect3DDevice9* ElDeviceManager::getDevice()
{
	return msSingleton->mDevice;
}

IDirect3DDevice9* ElDeviceManager::_getDevice()
{
	return mDevice;
}

void ElDeviceManager::_setBackBufferDesc()
{
	IDirect3DSurface9* backBuffer = NULL;
	mDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
	backBuffer->GetDesc(&mBackBufferDesc);
	ElSafeRelease(backBuffer);
}

void ElDeviceManager::_setDeviceCaps()
{
	mDevice->GetDeviceCaps(&mCaps);
}