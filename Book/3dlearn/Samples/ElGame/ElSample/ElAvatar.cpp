#include "ElSamplePCH.h"
#include "ElAvatar.h"
#include "ElDeviceManager.h"
#include "ElEntity.h"
#include "ElSkeletonManager.h"
#include "ElSceneNode.h"
#include "ELGUI.h"
#include "GUITexture.h"

#define EL_SATYR_MESH_FILE					"..\\Media\\Satyr\\Satyr.ELM"
#define EL_SATYR_SKELETON_FILE				"..\\Media\\Satyr\\Satyr.ELS"
#define EL_SATYR_SKEANIM_IDLE_FILE			"..\\Media\\Satyr\\Satyr_Idle.ESA"

ElAvatar::ElAvatar()
: mScene(NULL)
, mTexture(NULL)
, mSurface(NULL)
, mOldSurface(NULL)
{

}

ElAvatar::~ElAvatar()
{
	// should be called out somewhere
	//clearup();
}

bool ElAvatar::creat(unsigned int width, unsigned int height)
{
	using namespace ELGUI;

	// setup texture
	D3DXCreateTexture(ElDeviceManager::getDevice(), width, height, 1,
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT, &mTexture);
	mTexture->GetSurfaceLevel(0, &mSurface);

	// setup imageset
	ElGUITexture* gui_texture = (ElGUITexture*)System::getSingleton().getRenderer()->createTexture();
	gui_texture->setTexture(mTexture);
	Imageset* imageset = ImagesetManager::getSingleton().createImageset(ElAvatarImageset, gui_texture);
	Rect rect = Rect(Point(0.0f, 0.0f), Size(width, height));
	imageset->defineImage(ElAvatarImage, rect, Point(0.0f, 0.0f));

	// setup scene
	if (!_createScene())
		return false;

	// setup scene camera
	ElCamera* sceneCamera = ElNew ElCamera;
	float aspect = (float)width / (float)height;
	//sceneCamera->setPosition(D3DXVECTOR3(0.0f, 1.0f, -5.0f));
	sceneCamera->setPosition(D3DXVECTOR3(0.0f, 2.175f, -0.5f));
	sceneCamera->setProjectionParameters(ElCamera::PT_PERSPECTIVE, D3DX_PI / 4, aspect, 0.1f, 500.0f);
	mScene->setCamera(sceneCamera);

	return true;
}

void ElAvatar::update()
{
	if (mScene)
		mScene->update();
}

void ElAvatar::render(TimeValue t)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();

	// set render target
	d3dDevice->GetRenderTarget(0, &mOldSurface);
	d3dDevice->SetRenderTarget(0, mSurface);
	d3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 192, 192, 192), 1.0f, 0);

	if (mScene)
		mScene->render(t);

	d3dDevice->SetRenderTarget(0, mOldSurface);
	d3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(192, 192, 192), 1.0f, 0);
	ElSafeRelease(mOldSurface);
}

IDirect3DTexture9* ElAvatar::getTexture()
{
	return mTexture;
}

void ElAvatar::clearup()
{
	ElSafeRelease(mSurface);
	ElSafeRelease(mOldSurface);
	ElSafeDelete(mScene);
}

bool ElAvatar::_createScene()
{
	// create scene manager
	mScene = ElNew ElSceneManager;

	// setup entities
	ElSkeleton* skeleton = ElSkeletonManager::getSingleton().createSkeleton(EL_SATYR_SKELETON_FILE);
	skeleton->createAnimation(EL_SATYR_SKEANIM_IDLE_FILE, "Idle");

	ElEntity* satyr = ElEntity::createEntity(EL_SATYR_MESH_FILE);
	satyr->attachSkeleton(skeleton);

	ElSceneNode* node = mScene->getRootSceneNode()->createChildSceneNode("Spirit_Satyr");
	node->yaw(D3DX_PI / 2.5);
	node->attachObject(satyr);

	satyr->setAnimation("Idle");
	satyr->playAnimation(0);
	satyr->setAnimationSpeed(5.0f);

	// setup lights
	D3DLIGHT9 lt; 
	ZeroMemory(&lt, sizeof(D3DLIGHT9)); 
	lt.Type = D3DLIGHT_DIRECTIONAL;
	lt.Diffuse.r = 1.0f;
	lt.Diffuse.g = 1.0f;
	lt.Diffuse.b = 1.0f;
	lt.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	lt.Range = 1000.0f;
	mScene->addLight(lt);

	mScene->setAmbientLightColor(D3DCOLOR_ARGB(255, 255, 255, 255));

	return true;
}