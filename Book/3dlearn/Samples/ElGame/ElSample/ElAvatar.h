#ifndef __ElAvatar_H__
#define __ElAvatar_H__

#include "ElD3DHeaders.h"
#include "ElSceneManager.h"
#include "ElCamera.h"

#define ElAvatarImageset	"ElAvatarImageset"
#define ElAvatarImage		"ElAvatarImage"
#define ElAvatarImageValue	"set:ElAvatarImageset image:ElAvatarImage"

class ElAvatar
{
public:
	ElAvatar();
	virtual ~ElAvatar();

	bool creat(unsigned int width, unsigned int height);
	void update();
	void render(TimeValue t);
	IDirect3DTexture9* getTexture();

	void clearup();

protected:
	bool _createScene();

protected:
	ElSceneManager*			mScene;

	IDirect3DTexture9*		mTexture;
	IDirect3DSurface9*		mSurface;
	IDirect3DSurface9*		mOldSurface;
};

#endif //__ElAvatar_H__