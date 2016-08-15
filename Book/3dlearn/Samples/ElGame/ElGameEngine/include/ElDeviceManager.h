#ifndef __ElDeviceManager_H__
#define __ElDeviceManager_H__

#include "ElD3DHeaders.h"
#include "ElSingleton.h"
#include <string>

class ElDeviceManager
{
	ElDeclareSingleton(ElDeviceManager);

public:
	ElDeviceManager();
	virtual ~ElDeviceManager();

	bool						createDevice(HWND wnd);
	const D3DSURFACE_DESC&		getBackBufferDesc() const;
	const D3DCAPS9&				getDeviceCaps() const;

	static IDirect3DDevice9*	getDevice();

protected:
	IDirect3DDevice9*			_getDevice();
	void						_setBackBufferDesc();
	void						_setDeviceCaps();

protected:
	IDirect3DDevice9*			mDevice;
	D3DSURFACE_DESC				mBackBufferDesc;
	D3DCAPS9					mCaps;
};

#endif //__ElDeviceManager_H__