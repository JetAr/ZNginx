#include "ElSamplePCH.h"
#include "ElSampleApplication.h"
#include "GUIRenderer.h"
#include "GUITrueTypeFont.h"
#include "GUIHelper.h"
#include <time.h>

#define EL_ANIMBOX_MESH_FILE			"..\\Media\\SampleBox\\SampleBox_3.ELM"
#define EL_ANIMBOX_MESHANIM_FILE		"..\\Media\\SampleBox\\SampleBox_3.EMA"
#define EL_ANIMBOX_MESHANIM_FRAG_FILE	"..\\Media\\SampleBox\\SampleBox_3_Fragment.EMA"
#define EL_TIGER_MESH_FILE				"..\\Media\\Tiger\\Tiger.ELM"
#define EL_SATYR_MESH_FILE				"..\\Media\\Satyr\\Satyr.ELM"
#define EL_SATYR_SKELETON_FILE			"..\\Media\\Satyr\\Satyr.ELS"
#define EL_SATYR_SKEANIM_DIE_FILE		"..\\Media\\Satyr\\Satyr_Die.ESA"
#define EL_SATYR_SKEANIM_ATTACK_FILE	"..\\Media\\Satyr\\Satyr_Attack.ESA"
#define EL_SKEBOX_MESH_FILE				"..\\Media\\SkeletonBox\\SkeletonBox.ELM"
#define EL_SKEBOX_SKELETON_FILE			"..\\Media\\SkeletonBox\\SkeletonBox.ELS"
#define EL_SKEBOX_SKEANIM_FILE			"..\\Media\\SkeletonBox\\SkeletonBox.ESA"
#define EL_SKYBOX_TEXTURE				"..\\Media\\Skybox\\Skybox_%d.JPG"
#define EL_TERRAIN_HEIGHTMAP_257x257	"..\\Media\\Terrain\\HeightMap_257x257.raw"
#define EL_TERRAIN_DETAILMAP_0			"..\\Media\\Terrain\\TerrainDetailMap_0.tga"
#define EL_TERRAIN_DETAILMAP_1			"..\\Media\\Terrain\\TerrainDetailMap_1.tga"
#define EL_TERRAIN_DETAILMAP_2			"..\\Media\\Terrain\\TerrainDetailMap_2.tga"
#define EL_TERRAIN_DETAILMAP_3			"..\\Media\\Terrain\\TerrainBlendMap.tga"
#define EL_RIBBONTRAIL_TEXTURE			"..\\Media\\Particles\\ribbon.dds"

#define EL_RPGCAM_HEIGHT				5
#define EL_RPGCAM_FAR					15

#define ElDefaultScreenSaveDirectory	"..\\Screenshots\\"

#define ElGUISubscribeEvent(window, eventname, func) window->subscribeEvent(eventname, new ELGUI::MemberFunctionSlot<ElSampleApplication>(&func, this));

ElSampleApplication::ElSampleApplication()
: mTerrainIntersectedPoint(0.0f, 0.0f, 0.0f)
, mSceneIntersectedObject(NULL)
, mSaveScreenAfterRendered(false)
, mGUIRenderer(NULL)
, mGUISystem(NULL)
, mAvatarWindow(NULL)
, mFpsWindow(NULL)
{

}

void ElSampleApplication::terminate()
{
	_cleanupContent();

	ElApplication::terminate();
}

void ElSampleApplication::update()
{
	ElApplication::update();

	_updateRPGCamera();

	if (mAvatarWindow && mAvatarWindow->isVisible())
		mAvatar.update();

	if (mFpsWindow && mFpsWindow->isVisible())
	{
		mFps.update(getCurrentTime(true));

		wchar_t text[EL_MAX_PATH] = {0};
		swprintf(text, L"FPS: %.2f", mFps.getFps());

		// set text every frame will cause a terrible frame lose, because it
		// will notify the whole gui-system to redraw, so we just do this
		// once per second
		static int accumTime = 1000;
		accumTime += mFrameTime;
		if (accumTime > 1000)
		{
			mFpsWindow->setText(text);
			accumTime = 0;
		}
	}
}

void ElSampleApplication::renderScene()
{
	if (mAvatarWindow && mAvatarWindow->isVisible())
		mAvatar.render(getCurrentTime());
	
	ElApplication::renderScene();
	
	mBrush.render(mTerrainIntersectedPoint);
}

void ElSampleApplication::endRender()
{
	ElApplication::endRender();

	// do screen saving after frame rendering is done
	if (mSaveScreenAfterRendered)
	{
		_saveScreenImp(ElDefaultScreenSaveDirectory);
		mSaveScreenAfterRendered = false;
	}
}

void ElSampleApplication::pickTerrain(float screenx, float screeny)
{
	// get a ray to pick with from screen position
	ElRay mouseRay = mScene->getCamera()->getCameraToViewportRay(screenx, screeny);

	// terrain pick
	ElRayTerrainQuery* rayTerrainQuery = ElNew ElRayTerrainQuery(mScene->getTerrain());
	rayTerrainQuery->setRay(mouseRay);
	ElRayTerrainQueryResult& result2 = rayTerrainQuery->execute();
	if (!result2.empty())
		mTerrainIntersectedPoint = result2[0].singleIntersection;
	ElSafeDelete(rayTerrainQuery);
}

void ElSampleApplication::pickEntities(float screenx, float screeny)
{
	using namespace ELGUI;

	// get a ray to pick with from screen position
	ElRay mouseRay = mScene->getCamera()->getCameraToViewportRay(screenx, screeny);

	// scene node intersect detection, show its bounding box if picked one
	ElRaySceneQuery* raySceneQuery = ElNew ElRaySceneQuery(mScene);
	raySceneQuery->setRay(mouseRay);
	ElRaySceneQueryResult& result = raySceneQuery->execute();
	if (mSceneIntersectedObject)
		mSceneIntersectedObject->getParentNode()->showBoundingBox(false);
	if (!result.empty())
	{
		mSceneIntersectedObject = result[0].movable;
		mSceneIntersectedObject->getParentNode()->showBoundingBox(true);
		mSceneIntersectedObject->getParentNode()->setBoundingBoxColor(D3DCOLOR_XRGB(255, 255, 0));

		Tooltip* tooltip = (Tooltip*)WindowManager::getSingleton().getWindow("Tooltip2");
		tooltip->setVisible(true);
		std::wstring text = L"节点: " + ElGUIHelper::s2ws(mSceneIntersectedObject->getParentNode()->getName());
		if (text != tooltip->getText())
			tooltip->setText(text);
	}
	else
	{
		Tooltip* tooltip = (Tooltip*)WindowManager::getSingleton().getWindow("Tooltip2");
		tooltip->setVisible(false);
	}
	ElSafeDelete(raySceneQuery);
}

void ElSampleApplication::toggleFillMode()
{
	mFillMode = (mFillMode == D3DFILL_SOLID) ? D3DFILL_WIREFRAME : D3DFILL_SOLID;
}

void ElSampleApplication::toggleCamera()
{
	if (mScene)
	{
		if (mScene->getCamera() == mCamera)
			mScene->setCamera(mRPGCamera);
		else
			mScene->setCamera(mCamera);
	}
}

void ElSampleApplication::saveScreen()
{
	mSaveScreenAfterRendered = true;
}

bool ElSampleApplication::OnGUIEventSheetMouseMove(const ELGUI::EventArgs& args)
{
	using namespace ELGUI;

	const MouseEventArgs& ma = static_cast<const MouseEventArgs&>(args);
	Window* sheet = System::getSingleton().getGUISheet();
	Size sz = sheet->getPixelSize();
	
	// do terrain pick
	pickTerrain(ma.position.d_x / sz.d_width, ma.position.d_y / sz.d_height);

	// do entities pick
	pickEntities(ma.position.d_x / sz.d_width, ma.position.d_y / sz.d_height);

	return true;
}

bool ElSampleApplication::OnGUIEventSheetLButtonDown(const ELGUI::EventArgs& args)
{
	return true;
}

bool ElSampleApplication::OnGUIEventToggleFpsWnd(const ELGUI::EventArgs& args)
{
	if (mFpsWindow)
		mFpsWindow->setVisible(!mFpsWindow->isVisible());

	return true;
}

bool ElSampleApplication::OnGUIEventToggleSatyrPortrait(const ELGUI::EventArgs& args)
{
	using namespace ELGUI;

	Window* satyr_window = WindowManager::getSingleton().getWindow("SatyrPortrait");
	satyr_window->setVisible(!satyr_window->isVisible());

	return true;
}

bool ElSampleApplication::OnGUIEventToggleOrcWnd(const ELGUI::EventArgs& args)
{
	using namespace ELGUI;

	Window* orc_window = WindowManager::getSingleton().getWindow("OrcWindow");
	orc_window->setVisible(!orc_window->isVisible());

	return true;
}

bool ElSampleApplication::OnGUIEventPause(const ELGUI::EventArgs& args)
{
	setPause(true);
	ELGUI::WindowManager::getSingleton().getWindow("MainBar_Pause")->setVisible(false);
	ELGUI::WindowManager::getSingleton().getWindow("MainBar_Resume")->setVisible(true);
	
	return true;
}

bool ElSampleApplication::OnGUIEventResume(const ELGUI::EventArgs& args)
{
	setPause(false);
	ELGUI::WindowManager::getSingleton().getWindow("MainBar_Pause")->setVisible(true);
	ELGUI::WindowManager::getSingleton().getWindow("MainBar_Resume")->setVisible(false);

	return true;
}

bool ElSampleApplication::OnGUIEventMouseEnter(const ELGUI::EventArgs& args)
{
	using namespace ELGUI;

	Tooltip* tooltip = (Tooltip*)WindowManager::getSingleton().getWindow("Tooltip");

	const MouseEventArgs& ma = static_cast<const MouseEventArgs&>(args);
	std::string window_name = ma.window->getName();
	if (window_name.compare("MainBar_Char") == 0)
		tooltip->setText(L"显示/隐藏头像");
	else if (window_name.compare("MainBar_Sprite") == 0)
		tooltip->setText(L"显示/隐藏说明窗口");
	else if (window_name.compare("MainBar_Log") == 0)
		tooltip->setText(L"显示/隐藏FPS");
	else if (window_name.compare("MainBar_Pause") == 0)
		tooltip->setText(L"暂停游戏");
	else if (window_name.compare("MainBar_Resume") == 0)
		tooltip->setText(L"继续游戏");

	tooltip->setVisible(true);
	tooltip->positionSelf();

	return true;
}

bool ElSampleApplication::OnGUIEventMouseLeave(const ELGUI::EventArgs& args)
{
	using namespace ELGUI;

	Tooltip* tooltip = (Tooltip*)WindowManager::getSingleton().getWindow("Tooltip");
	tooltip->setVisible(false);

	return true;
}

bool ElSampleApplication::createScene()
{
	if (!ElApplication::createScene())
		return false;

	if (!_setupContent())
		return false;

	if (!_setupGUI())
		return false;

	return true;
}

bool ElSampleApplication::_setupContent()
{
	// setup skyBox
	ElSkybox* sky = ElNew ElSkybox;
	if (!sky->create(32, 32, 32))
		return false;
	
	char fileName[EL_MAX_PATH] = {0};
	for (int i = 0; i < 6; ++i)
	{
		sprintf(fileName, EL_SKYBOX_TEXTURE, i);
		sky->setTexture((ElSkybox::SkyboxPlane)i, fileName);
	}

	mScene->setSkybox(sky);

	// setup terrain
	ElTerrain* terrain = ElNew ElTerrain;
	if (!terrain->create(8))
		return false;

	terrain->setHeightMap(EL_TERRAIN_HEIGHTMAP_257x257);
	terrain->setLayerMap(0, EL_TERRAIN_DETAILMAP_0);
	terrain->setLayerMap(1, EL_TERRAIN_DETAILMAP_1);
	terrain->setLayerMap(2, EL_TERRAIN_DETAILMAP_2);
	terrain->setLayerMap(3, EL_TERRAIN_DETAILMAP_3);

	mScene->setTerrain(terrain);

	// setup water
	ElWater* water = ElNew ElWater;
	if (!water->create(D3DXVECTOR3(0.0f, 0.2f, 256.0f), 256.0f, 256.0f))
		return false;
	mScene->setWater(water);

	// setup scene objects
	if (!_setupEntities())
		return false;

	// setup rpg-camera
	if (!_setupRPGCamera())
		return false;

	// setup fog
	mScene->setFog(D3DFOG_LINEAR, D3DCOLOR_XRGB(128, 128, 128), 80.0f, 200.0f);
	
	// setup lights
	D3DLIGHT9 lt; 
	ZeroMemory(&lt, sizeof(D3DLIGHT9)); 
	lt.Type = D3DLIGHT_DIRECTIONAL;
	lt.Diffuse.r = 1.0f;
	lt.Diffuse.g = 1.0f;
	lt.Diffuse.b = 1.0f;
	lt.Diffuse.a = 1.0f;
	lt.Direction = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	lt.Range = 1000.0f;
	mScene->addLight(lt);

	mScene->setAmbientLightColor(D3DCOLOR_ARGB(255, 255, 255, 255));

	// setup brush
	if (!mBrush.create(terrain, 65, 1.0f, 3.0f))
		return false;

	return true;
}

void ElSampleApplication::_cleanupContent()
{
	ElSafeDelete(mGUISystem);
	ElSafeDelete(mGUIRenderer);

	mAvatar.clearup();

	mScene->setCamera(mCamera);
	if (mRPGCamera && mRPGCamera->getParentNode() == NULL)
	{
		ElSafeDelete(mRPGCamera);
	}
}

bool ElSampleApplication::_setupEntities()
{
	// entities create
	const int numObjs = 5;
	ElEntity* obj[numObjs];
	for (int o = 0; o < numObjs; ++o)
		obj[o] = ElEntity::createEntity(EL_SATYR_MESH_FILE);
	ElEntity* tiger = ElEntity::createEntity(EL_TIGER_MESH_FILE);
	ElEntity* box = ElEntity::createEntity(EL_ANIMBOX_MESH_FILE);
	ElEntity* satyr = ElEntity::createEntity(EL_SATYR_MESH_FILE);
	ElEntity* objectAttachedToDummy = ElEntity::createEntity(EL_SATYR_MESH_FILE);
	ElEntity* skeletalBox = ElEntity::createEntity(EL_SKEBOX_MESH_FILE);
	
	if (!mSatyrNameboard.create(L"征服者 写真集\n<El>", "华文楷体", 48, 0x2AFF3B))
		return false;
	float width = mSatyrNameboard.getWidth() / 150.0f;
	float height = mSatyrNameboard.getHeight() / 150.0f;
	ElBillboard* satyr_billboard = ElNew ElBillboard;
	satyr_billboard->setRectangle(width * 0.5f, height * 0.5f, width, height);
	satyr_billboard->setTexture(mSatyrNameboard.getTexture());

	if (!mTigerNameboard.create(L"祖利安猛虎", "华文楷体", 48, 0xE00722))
		return false;
	width = mTigerNameboard.getWidth() / 150.0f;
	height = mTigerNameboard.getHeight() / 150.0f;
	ElBillboard* tiger_billboard = ElNew ElBillboard;
	tiger_billboard->setRectangle(width * 0.5f, height * 0.5f, width, height);
	tiger_billboard->setTexture(mTigerNameboard.getTexture());

	// animation creat an attach, include mesh animation and skeletal animation both
	box->getMesh()->createAnimation(EL_ANIMBOX_MESHANIM_FILE, "Full");
	box->getMesh()->createAnimation(EL_ANIMBOX_MESHANIM_FRAG_FILE, "Fragment");

	ElSkeleton* skeleton = ElSkeletonManager::getSingleton().createSkeleton(EL_SATYR_SKELETON_FILE);
	skeleton->createAnimation(EL_SATYR_SKEANIM_DIE_FILE, "Die");
	skeleton->createAnimation(EL_SATYR_SKEANIM_ATTACK_FILE, "Attack");
	for (int o = 0; o < numObjs; ++o)
		obj[o]->attachSkeleton(skeleton);
	satyr->attachSkeleton(skeleton);
	objectAttachedToDummy->attachSkeleton(skeleton);

	skeleton = ElSkeletonManager::getSingleton().createSkeleton(EL_SKEBOX_SKELETON_FILE);
	skeleton->createAnimation(EL_SKEBOX_SKEANIM_FILE, "Swing");
	skeletalBox->attachSkeleton(skeleton);

	// animation set
	TimeValue currentTime = getCurrentTime();

	for (int o = 0; o < numObjs; ++o)
	{
		obj[o]->setAnimation("Attack");
		obj[o]->playAnimation(currentTime);
		obj[o]->setAnimationSpeed(o % 5 + 1);
	}

	satyr->setAnimation("Die");
	satyr->playAnimation(currentTime);
	satyr->setAnimationSpeed(5.0f);

	objectAttachedToDummy->setAnimation("Attack");
	objectAttachedToDummy->playAnimation(currentTime);
	objectAttachedToDummy->setAnimationSpeed(6.0f);

	box->setAnimation("Full");
	box->playAnimation(currentTime);
	box->setAnimationSpeed(2.0f);

	skeletalBox->setAnimation("Swing");
	skeletalBox->playAnimation(currentTime);
	skeletalBox->setAnimationSpeed(3.0f);

	// set scene nodes to build a scene tree
	ElSceneNode* node = NULL;
	for (int o = 0; o < numObjs; ++o)
	{
		char name[EL_MAX_PATH] = {0};
		sprintf(name, "Obj_%d", o);
		node = mScene->getRootSceneNode()->createChildSceneNode(name);
		node->scale(0.8f, 0.8f, 0.8f);
		node->setPosition(D3DXVECTOR3(0.0f, 0.0f, o * 3 + 3));
		node->attachObject(obj[o]);
	}

	node = mScene->getRootSceneNode()->createChildSceneNode("Satyr_1");
	node->scale(2.0f, 2.0f, 2.0f);
	node->attachObject(satyr);

	ElSceneNode* billboardNode = ElNew ElSceneNode("SatyrBillboard");
	billboardNode->setPosition(D3DXVECTOR3(0.0f, 3.0f, 0.0f));
	billboardNode->attachObject(satyr_billboard);
	node->addChild(billboardNode);

	node = mScene->getRootSceneNode()->createChildSceneNode("Tiger");
	node->setPosition(D3DXVECTOR3(7.0f, 0.0f, 12.0f));
	node->scale(0.5f, 0.5f, 0.5f);
	node->yaw(D3DX_PI / 2);
	node->attachObject(tiger);

	// set tiger as the first-person-view node
	mFirstPersonNode = node;
	
	billboardNode = ElNew ElSceneNode("TigerBillboard");
	billboardNode->setPosition(D3DXVECTOR3(0.0f, 7.0f, 0.0f));
	billboardNode->attachObject(tiger_billboard);
	node->addChild(billboardNode);

	node = mScene->getRootSceneNode()->createChildSceneNode("Box");
	node->setPosition(D3DXVECTOR3(14.0f, 0.0f, 10.0f));
	node->scale(0.1f, 0.1f, 0.1f);
	node->attachObject(box);

	node = mScene->getRootSceneNode()->createChildSceneNode("SkeletonBox");
	node->setPosition(D3DXVECTOR3(14.0f, 0.0f, 5.0f));
	node->scale(0.1f, 0.1f, 0.1f);
	node->attachObject(skeletalBox);
	skeletalBox->setAlpha(0.5f);

	ElSceneNode* dummyNode = ElNew ElSceneNode();
	dummyNode->scale(10.0f, 10.0f, 10.0f);
	dummyNode->attachObject(objectAttachedToDummy);
	node = skeletalBox->getChildObject("Dummy01");
	node->addChild(dummyNode);

	ElRibbonTrail* trail = ElNew ElRibbonTrail;
	IDirect3DTexture9* trailTexture = NULL;
	D3DXCreateTextureFromFile(ElDeviceManager::getDevice(), EL_RIBBONTRAIL_TEXTURE, &trailTexture);
	trail->setMaxChainElements(20);
	trail->setTrailLength(20);
	trail->setTrailWidth(0.2f);
	trail->setTexture(trailTexture);
	trail->setTextureCoordMode(ElBillboardChain::TextCoord_CustomY);
	trail->attachNode(dummyNode);
	node = mScene->getRootSceneNode();
	node->attachObject(trail);

	//ElBillboardChain* billboardChain = ElNew ElBillboardChain;
	//billboardChain->setMaxChainElements(5);
	//billboardChain->setTexture(trailTexture);
	//billboardChain->setTextureCoordMode(ElBillboardChain::TextCoord_CustomY);
	//billboardChain->addChainElement(ElBillboardChain::Element(D3DXVECTOR3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f, D3DCOLOR_ARGB(255, 255, 255, 255)));
	//billboardChain->addChainElement(ElBillboardChain::Element(D3DXVECTOR3(8.0f, 1.0f, 8.0f), 1.0f, 0.25f, D3DCOLOR_ARGB(255, 255, 255, 255)));
	//billboardChain->addChainElement(ElBillboardChain::Element(D3DXVECTOR3(16.0f, 1.0f, 16.0f), 1.0f, 0.5f, D3DCOLOR_ARGB(255, 255, 255, 255)));
	//billboardChain->addChainElement(ElBillboardChain::Element(D3DXVECTOR3(24.0f, 1.0f, 24.0f), 1.0f, 0.75f, D3DCOLOR_ARGB(255, 255, 255, 255)));
	//billboardChain->addChainElement(ElBillboardChain::Element(D3DXVECTOR3(32.0f, 1.0f, 32.0f), 1.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 0, 0)));
	//node = mScene->getRootSceneNode();
	//node->attachObject(billboardChain);

	return true;
}

bool ElSampleApplication::_setupRPGCamera()
{
	// create a pivot at roughly the character's shoulder
	mCameraPivot = ElNew ElSceneNode("CameraPivot");
	mCameraPivot->setPosition(D3DXVECTOR3(0.0f, 1.0f, 0.0f) * EL_RPGCAM_HEIGHT);
	mFirstPersonNode->addChild(mCameraPivot);
	//mScene->getRootSceneNode()->createChildSceneNode("CameraPivot");
	// this is where the camera should be soon, and it spins around the pivot
	mCameraGoal = mCameraPivot->createChildSceneNode("CameraGoal");
	mCameraGoal->setPosition(D3DXVECTOR3(0, 0, -EL_RPGCAM_FAR));
	// this is where the camera actually is
	mCameraNode = mScene->getRootSceneNode()->createChildSceneNode("RPGCameraNode");

	// create rpg-camera
	ElSafeDelete(mRPGCamera);
	mRPGCamera = ElNew ElCamera;
	
	const D3DSURFACE_DESC& backBufferDesc = ElDeviceManager::getSingleton().getBackBufferDesc();
	float aspect = (float)backBufferDesc.Width / (float)backBufferDesc.Height;
	mRPGCamera->setProjectionParameters(ElCamera::PT_PERSPECTIVE, D3DX_PI / 4, aspect, 0.1f, 500.0f);
	mRPGCamera->setPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	mCameraNode->attachObject(mRPGCamera);

	return true;
}

void ElSampleApplication::_updateRPGCamera()
{
	// move the camera smoothly to the goal
	D3DXVECTOR3 goalOffset = mCameraGoal->getDerivedPosition() - mCameraNode->getPosition();
	mCameraNode->setPosition(mCameraNode->getPosition() + goalOffset/* * mFrameTime * 9.0f*/);
	// always look at the pivot
	mRPGCamera->lookAt(mCameraPivot->getDerivedPosition());
}

bool ElSampleApplication::_setupGUI()
{
	using namespace ELGUI;

	// create gui system
	mGUIRenderer = ElNew ElGUIRenderer;
	mGUISystem = ElNew System(mGUIRenderer);

	// create fonts
	ElTrueTypeFont* font = new ElTrueTypeFont("宋体", "Sum", 14);
	font->load();
	FontManager::getSingleton().registFont("Sum", font);

	font = new ElTrueTypeFont("华文楷体", "Kt", 15);
	font->load();
	FontManager::getSingleton().registFont("Kt", font);

	font = new ElTrueTypeFont("Arial", "Arial", 14, true);
	font->load();
	FontManager::getSingleton().registFont("Arial", font);

	// create imagesets
	Texture* tex = (Texture*)mGUIRenderer->createTexture("..\\Media\\Texture\\ogredance_0.png");
	Imageset* imageset = ImagesetManager::getSingleton().createImageset("ogredanceImageset_0", tex);
	Rect rect = Rect(Point(0.0f, 0.0f), Size(32.0f, 32.0f));
	imageset->defineImage("ogredance", rect, Point(0.0f, 0.0f));

	tex = (Texture*)mGUIRenderer->createTexture("..\\Media\\GUI\\ui1.dds");
	imageset = ImagesetManager::getSingleton().createImageset("ui1", tex);
	rect = Rect(Point(0.0f, 2.0f), Size(380.0f, 121.0f));
	imageset->defineImage("mainbar_left", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(407.0f, 60.0f), Size(40.0f, 83.0f));
	imageset->defineImage("mainbar_hp", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(448.0f, 60.0f), Size(40.0f, 83.0f));
	imageset->defineImage("mainbar_mp", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(245.0f, 163.0f), Size(24.0f, 22.0f));
	imageset->defineImage("mainbar_inventory", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(277.0f, 163.0f), Size(24.0f, 22.0f));
	imageset->defineImage("mainbar_char", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(307.0f, 163.0f), Size(24.0f, 22.0f));
	imageset->defineImage("mainbar_sprite", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(338.0f, 163.0f), Size(24.0f, 22.0f));
	imageset->defineImage("mainbar_quest", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(369.0f, 163.0f), Size(24.0f, 22.0f));
	imageset->defineImage("mainbar_pet", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(400.0f, 163.0f), Size(24.0f, 22.0f));
	imageset->defineImage("mainbar_log", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(432.0f, 163.0f), Size(24.0f, 22.0f));
	imageset->defineImage("mainbar_map", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(462.0f, 163.0f), Size(24.0f, 22.0f));
	imageset->defineImage("mainbar_settings", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(442.0f, 339.0f), Size(33.0f, 33.0f));
	imageset->defineImage("mainbar_pause", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(1.0f, 430.0f), Size(41.0f, 42.0f));
	imageset->defineImage("mainbar_resume_hover", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(43.0f, 430.0f), Size(41.0f, 42.0f));
	imageset->defineImage("mainbar_resume_normal", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(0.0f, 220.0f), Size(21.0f, 21.0f));
	imageset->defineImage("tooltip_TL", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(25.0f, 220.0f), Size(5.0f, 21.0f));
	imageset->defineImage("tooltip_TM", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(34.0f, 220.0f), Size(21.0f, 21.0f));
	imageset->defineImage("tooltip_TR", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(0.0f, 245.0f), Size(21.0f, 5.0f));
	imageset->defineImage("tooltip_ML", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(25.0f, 245.0f), Size(5.0f, 5.0f));
	imageset->defineImage("tooltip_MM", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(34.0f, 245.0f), Size(21.0f, 5.0f));
	imageset->defineImage("tooltip_MR", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(0.0f, 254.0f), Size(21.0f, 20.0f));
	imageset->defineImage("tooltip_BL", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(25.0f, 254.0f), Size(5.0f, 20.0f));
	imageset->defineImage("tooltip_BM", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(34.0f, 254.0f), Size(21.0f, 20.0f));
	imageset->defineImage("tooltip_BR", rect, Point(0.0f, 0.0f));

	tex = (Texture*)mGUIRenderer->createTexture("..\\Media\\GUI\\ui2.dds");
	imageset = ImagesetManager::getSingleton().createImageset("ui2", tex);
	rect = Rect(Point(0.0f, 384.0f), Size(380.0f, 121.0f));
	imageset->defineImage("mainbar_right", rect, Point(0.0f, 0.0f));
	rect = Rect(Point(68.0f, 151.0f), Size(60.0f, 60.0f));
	imageset->defineImage("portrait", rect, Point(0.0f, 0.0f));

	tex = (Texture*)mGUIRenderer->createTexture("..\\Media\\GUI\\dropdown.dds");
	imageset = ImagesetManager::getSingleton().createImageset("dropdown", tex);
	rect = Rect(Point(63.0f, 131.0f), Size(281.0f, 381.0f));
	imageset->defineImage("window_without_chains", rect, Point(0.0f, 0.0f));

	// setup windows
	_setupGUIWidgets();

	// setup cursor
	_setupGUICursor();

	return true;
}

bool ElSampleApplication::_setupGUIWidgets()
{
	using namespace ELGUI;

	WindowManager& winMgr = WindowManager::getSingleton();

	// set gui sheet
	Window* sheet = winMgr.createWindow("DefaultWindow", "Sheet");
	sheet->setSize(UVector2(elgui_reldim(1.0f), elgui_reldim(1.0f)));
	sheet->setPosition(UVector2(elgui_reldim(0.0f), elgui_reldim(0.0f)));
	mGUISystem->setGUISheet(sheet);
	
	ElGUISubscribeEvent(sheet, ELGUI::Window::EventMouseMove, ElSampleApplication::OnGUIEventSheetMouseMove);
	ElGUISubscribeEvent(sheet, ELGUI::Window::EventMouseButtonDown, ElSampleApplication::OnGUIEventSheetLButtonDown);

	// set fps windows
	mFpsWindow = (StaticText*)winMgr.createWindow("StaticText", "FpsWindow");
	mFpsWindow->setSize(UVector2(elgui_absdim(250.0f), elgui_absdim(300.0f)));
	mFpsWindow->setPosition(UVector2(UDim(0.0f, 10.0f), UDim(1.0f, -20.0f)));
	mFpsWindow->setFont("Arial");
	mFpsWindow->setTextColours(ColourRect(0xFFFF7200));
	mFpsWindow->setMousePassThroughEnabled(true);
	sheet->addChildWindow(mFpsWindow);

	// set satyr portrait
	StaticImage* satyr_portrait = (StaticImage*)winMgr.createWindow("StaticImage", "SatyrPortrait");
	satyr_portrait->setSize(UVector2(elgui_absdim(60.0f), elgui_absdim(60.0f)));
	satyr_portrait->setPosition(UVector2(UDim(0.0f, 10.0f), UDim(0.0f, 10.0f)));
	satyr_portrait->setImage(Helper::stringToImage("set:ui2 image:portrait"));
	sheet->addChildWindow(satyr_portrait);

	mAvatarWindow = (StaticImage*)winMgr.createWindow("StaticImage", "SatyrAvatar");
	mAvatarWindow->setSize(UVector2(elgui_absdim(42.0f), elgui_absdim(42.0f)));
	mAvatarWindow->setPosition(UVector2(UDim(0.0f, 9.0f), UDim(0.0f, 9.0f)));
	satyr_portrait->addChildWindow(mAvatarWindow);

	Size sz = mAvatarWindow->getPixelSize();
	mAvatar.creat(sz.d_width, sz.d_height);
	mAvatarWindow->setImage(Helper::stringToImage(ElAvatarImageValue));

	// set main bar
	Window* main_bar = winMgr.createWindow("DefaultWindow", "MainBar");
	main_bar->setSize(UVector2(elgui_absdim(760.0f), elgui_absdim(121.0f)));
	main_bar->setPosition(UVector2(UDim(0.5f, -380.0f), UDim(1.0f, -121.0f)));
	sheet->addChildWindow(main_bar);

	// main bar left
	StaticImage* main_bar_left = (StaticImage*)winMgr.createWindow("StaticImage", "MainBar_Left");
	main_bar_left->setSize(UVector2(elgui_absdim(380.0f), elgui_absdim(121.0f)));
	main_bar_left->setPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
	main_bar_left->setImage(Helper::stringToImage("set:ui1 image:mainbar_left"));
	main_bar->addChildWindow(main_bar_left);

	StaticImage* main_bar_hp = (StaticImage*)winMgr.createWindow("StaticImage", "MainBar_Hp");
	main_bar_hp->setSize(UVector2(elgui_absdim(40.0f), elgui_absdim(83.0f)));
	main_bar_hp->setPosition(UVector2(UDim(0.0f, 335.0f), UDim(0.0f, 11.0f)));
	main_bar_hp->setImage(Helper::stringToImage("set:ui1 image:mainbar_hp"));
	main_bar_left->addChildWindow(main_bar_hp);

	ImageButton* main_bar_inventory = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Inventory");
	main_bar_inventory->setSize(UVector2(elgui_absdim(24.0f), elgui_absdim(22.0f)));
	main_bar_inventory->setPosition(UVector2(UDim(0.0f, 224.0f), UDim(0.0f, 25.0f)));
	main_bar_inventory->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_inventory"));
	main_bar_inventory->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_inventory"));
	main_bar_left->addChildWindow(main_bar_inventory);

	ImageButton* main_bar_char = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Char");
	main_bar_char->setSize(UVector2(elgui_absdim(24.0f), elgui_absdim(22.0f)));
	main_bar_char->setPosition(UVector2(UDim(0.0f, 253.0f), UDim(0.0f, 25.0f)));
	main_bar_char->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_char"));
	main_bar_char->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_char"));
	main_bar_left->addChildWindow(main_bar_char);

	ImageButton* main_bar_sprite = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Sprite");
	main_bar_sprite->setSize(UVector2(elgui_absdim(24.0f), elgui_absdim(22.0f)));
	main_bar_sprite->setPosition(UVector2(UDim(0.0f, 280.0f), UDim(0.0f, 25.0f)));
	main_bar_sprite->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_sprite"));
	main_bar_sprite->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_sprite"));
	main_bar_left->addChildWindow(main_bar_sprite);

	ImageButton* main_bar_quest = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Quest");
	main_bar_quest->setSize(UVector2(elgui_absdim(24.0f), elgui_absdim(22.0f)));
	main_bar_quest->setPosition(UVector2(UDim(0.0f, 308.0f), UDim(0.0f, 25.0f)));
	main_bar_quest->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_quest"));
	main_bar_quest->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_quest"));
	main_bar_left->addChildWindow(main_bar_quest);

	ImageButton* main_bar_pause = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Pause");
	main_bar_pause->setSize(UVector2(elgui_absdim(33.0f), elgui_absdim(33.0f)));
	main_bar_pause->setPosition(UVector2(UDim(0.0f, 278.0f), UDim(0.0f, 54.0f)));
	main_bar_pause->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_pause"));
	main_bar_pause->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_pause"));
	main_bar_left->addChildWindow(main_bar_pause);

	ImageButton* main_bar_resume = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Resume");
	main_bar_resume->setSize(UVector2(elgui_absdim(41.0f), elgui_absdim(42.0f)));
	main_bar_resume->setPosition(UVector2(UDim(0.0f, 273.0f), UDim(0.0f, 49.0f)));
	main_bar_resume->setNormalImage(Helper::stringToImage("set:ui1 image:mainbar_resume_normal"));
	main_bar_resume->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_resume_hover"));
	main_bar_resume->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_resume_hover"));
	main_bar_left->addChildWindow(main_bar_resume);
	main_bar_resume->setVisible(false);

	// main bar right
	StaticImage* main_bar_right = (StaticImage*)winMgr.createWindow("StaticImage", "MainBar_Right");
	main_bar_right->setSize(UVector2(elgui_absdim(380.0f), elgui_absdim(121.0f)));
	main_bar_right->setPosition(UVector2(UDim(0.0f, 380.0f), UDim(0.0f, 0.0f)));
	main_bar_right->setImage(Helper::stringToImage("set:ui2 image:mainbar_right"));
	main_bar->addChildWindow(main_bar_right);
	
	StaticImage* main_bar_mp = (StaticImage*)winMgr.createWindow("StaticImage", "MainBar_Mp");
	main_bar_mp->setSize(UVector2(elgui_absdim(40.0f), elgui_absdim(83.0f)));
	main_bar_mp->setPosition(UVector2(UDim(0.0f, 5.0f), UDim(0.0f, 11.0f)));
	main_bar_mp->setImage(Helper::stringToImage("set:ui1 image:mainbar_mp"));
	main_bar_right->addChildWindow(main_bar_mp);

	ImageButton* main_bar_pet = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Pet");
	main_bar_pet->setSize(UVector2(elgui_absdim(24.0f), elgui_absdim(22.0f)));
	main_bar_pet->setPosition(UVector2(UDim(0.0f, 48.0f), UDim(0.0f, 25.0f)));
	main_bar_pet->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_pet"));
	main_bar_pet->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_pet"));
	main_bar_right->addChildWindow(main_bar_pet);

	ImageButton* main_bar_log = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Log");
	main_bar_log->setSize(UVector2(elgui_absdim(24.0f), elgui_absdim(22.0f)));
	main_bar_log->setPosition(UVector2(UDim(0.0f, 76.0f), UDim(0.0f, 25.0f)));
	main_bar_log->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_log"));
	main_bar_log->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_log"));
	main_bar_right->addChildWindow(main_bar_log);

	ImageButton* main_bar_map = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Map");
	main_bar_map->setSize(UVector2(elgui_absdim(24.0f), elgui_absdim(22.0f)));
	main_bar_map->setPosition(UVector2(UDim(0.0f, 105.0f), UDim(0.0f, 25.0f)));
	main_bar_map->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_map"));
	main_bar_map->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_map"));
	main_bar_right->addChildWindow(main_bar_map);

	ImageButton* main_bar_settings = (ImageButton*)winMgr.createWindow("ImageButton", "MainBar_Settings");
	main_bar_settings->setSize(UVector2(elgui_absdim(24.0f), elgui_absdim(22.0f)));
	main_bar_settings->setPosition(UVector2(UDim(0.0f, 132.0f), UDim(0.0f, 25.0f)));
	main_bar_settings->setHoverImage(Helper::stringToImage("set:ui1 image:mainbar_settings"));
	main_bar_settings->setPushedImage(Helper::stringToImage("set:ui1 image:mainbar_settings"));
	main_bar_right->addChildWindow(main_bar_settings);
	
	ElGUISubscribeEvent(main_bar_char, ELGUI::Window::EventMouseButtonDown, ElSampleApplication::OnGUIEventToggleSatyrPortrait);
	ElGUISubscribeEvent(main_bar_char, ELGUI::Window::EventMouseEnters, ElSampleApplication::OnGUIEventMouseEnter);
	ElGUISubscribeEvent(main_bar_char, ELGUI::Window::EventMouseLeaves, ElSampleApplication::OnGUIEventMouseLeave);
	ElGUISubscribeEvent(main_bar_sprite, ELGUI::Window::EventMouseButtonDown, ElSampleApplication::OnGUIEventToggleOrcWnd);
	ElGUISubscribeEvent(main_bar_sprite, ELGUI::Window::EventMouseEnters, ElSampleApplication::OnGUIEventMouseEnter);
	ElGUISubscribeEvent(main_bar_sprite, ELGUI::Window::EventMouseLeaves, ElSampleApplication::OnGUIEventMouseLeave);
	ElGUISubscribeEvent(main_bar_log, ELGUI::Window::EventMouseButtonDown, ElSampleApplication::OnGUIEventToggleFpsWnd);
	ElGUISubscribeEvent(main_bar_log, ELGUI::Window::EventMouseEnters, ElSampleApplication::OnGUIEventMouseEnter);
	ElGUISubscribeEvent(main_bar_log, ELGUI::Window::EventMouseLeaves, ElSampleApplication::OnGUIEventMouseLeave);
	ElGUISubscribeEvent(main_bar_pause, ELGUI::Window::EventMouseButtonDown, ElSampleApplication::OnGUIEventPause);
	ElGUISubscribeEvent(main_bar_pause, ELGUI::Window::EventMouseEnters, ElSampleApplication::OnGUIEventMouseEnter);
	ElGUISubscribeEvent(main_bar_pause, ELGUI::Window::EventMouseLeaves, ElSampleApplication::OnGUIEventMouseLeave);
	ElGUISubscribeEvent(main_bar_resume, ELGUI::Window::EventMouseButtonDown, ElSampleApplication::OnGUIEventResume);
	ElGUISubscribeEvent(main_bar_resume, ELGUI::Window::EventMouseEnters, ElSampleApplication::OnGUIEventMouseEnter);
	ElGUISubscribeEvent(main_bar_resume, ELGUI::Window::EventMouseLeaves, ElSampleApplication::OnGUIEventMouseLeave);

	// set orc windows
	FrameWindow* orc_window = (FrameWindow*)winMgr.createWindow("FrameWindow", "OrcWindow");
	orc_window->setSize(UVector2(elgui_absdim(281.0f), elgui_absdim(381.0f)));
	orc_window->setPosition(UVector2(UDim(0.5f, -140.0f), elgui_reldim(0.15f)));
	orc_window->setSectionImage(FrameWindow::TopLeft, Helper::stringToImage("set:dropdown image:window_without_chains"));
	sheet->addChildWindow(orc_window);
	orc_window->setVisible(false);

	StaticText* orc_title = (StaticText*)winMgr.createWindow("StaticText", "OrcTitle");
	orc_title->setSize(UVector2(elgui_absdim(96.0f), elgui_absdim(25.0f)));
	orc_title->setPosition(UVector2(elgui_absdim(90.0f), elgui_absdim(25.0f)));
	orc_title->setFont("Sum");
	orc_title->setText(L"说明");
	orc_title->setTextColours(ColourRect(0xFFFFFF00));
	orc_title->setTextFormatting(WordWrapCentred);
	orc_window->addChildWindow(orc_title);

	StaticText* orc_des = (StaticText*)winMgr.createWindow("StaticText", "OrcDes");
	orc_des->setSize(UVector2(elgui_absdim(185.0f), elgui_absdim(150.0f)));
	orc_des->setPosition(UVector2(elgui_absdim(49.0f), elgui_absdim(80.0f)));
	orc_des->setFont("Kt");
	orc_des->setTextColours(ColourRect(0xFFFFFFFF));
	orc_window->addChildWindow(orc_des);
	
	const std::wstring controls = L"'WSAD': Camera Move\n"
		L"RMouse: Camera Rotate\n"
		L"'['/']': Camera Rise/Down\n"
		L"'H': FillMode\n"
		L"'C': FirstPersonView\n"
		L"'='/'-': Terrain LOD Factor1\n"
		L"'0'/'9': Terrain LOD Factor2\n"
		L"'PrtScr': Save Screen";
	orc_des->setText(controls);

	StaticImage* orc_static_image = (StaticImage*)winMgr.createWindow("StaticImage", "OrcStaticImage");
	orc_static_image->setSize(UVector2(elgui_absdim(128.0f), elgui_absdim(110.0f)));
	orc_static_image->setPosition(UVector2(UDim(0.5f, -64.0f), UDim(0.0f, 220.0f)));
	orc_static_image->setImage(Helper::stringToImage("set:ogredanceImageset_0 image:ogredance"));
	orc_window->addChildWindow(orc_static_image);

	// tooltip window
	Tooltip* tooltip = (Tooltip*)winMgr.createWindow("Tooltip", "Tooltip");
	tooltip->setPosition(UVector2(UDim(0.5f, 0.0f), UDim(0.5f, 0.0f)));
	tooltip->setSectionImage(FrameWindow::TopLeft, Helper::stringToImage("set:ui1 image:tooltip_TL"));
	tooltip->setSectionImage(FrameWindow::TopMiddle, Helper::stringToImage("set:ui1 image:tooltip_TM"));
	tooltip->setSectionImage(FrameWindow::TopRight, Helper::stringToImage("set:ui1 image:tooltip_TR"));
	tooltip->setSectionImage(FrameWindow::MiddleLeft, Helper::stringToImage("set:ui1 image:tooltip_ML"));
	tooltip->setSectionImage(FrameWindow::MiddleMiddle, Helper::stringToImage("set:ui1 image:tooltip_MM"));
	tooltip->setSectionImage(FrameWindow::MiddleRight, Helper::stringToImage("set:ui1 image:tooltip_MR"));
	tooltip->setSectionImage(FrameWindow::BottomLeft, Helper::stringToImage("set:ui1 image:tooltip_BL"));
	tooltip->setSectionImage(FrameWindow::BottomMiddle, Helper::stringToImage("set:ui1 image:tooltip_BM"));
	tooltip->setSectionImage(FrameWindow::BottomRight, Helper::stringToImage("set:ui1 image:tooltip_BR"));
	tooltip->setFont("Kt");
	sheet->addChildWindow(tooltip);
	tooltip->setVisible(false);

	// tooltip2 window
	Tooltip* tooltip2 = (Tooltip*)winMgr.createWindow("Tooltip", "Tooltip2");
	tooltip2->setPosition(UVector2(UDim(0.8f, 0.0f), UDim(0.8f, 0.0f)));
	tooltip2->setSectionImage(FrameWindow::TopLeft, Helper::stringToImage("set:ui1 image:tooltip_TL"));
	tooltip2->setSectionImage(FrameWindow::TopMiddle, Helper::stringToImage("set:ui1 image:tooltip_TM"));
	tooltip2->setSectionImage(FrameWindow::TopRight, Helper::stringToImage("set:ui1 image:tooltip_TR"));
	tooltip2->setSectionImage(FrameWindow::MiddleLeft, Helper::stringToImage("set:ui1 image:tooltip_ML"));
	tooltip2->setSectionImage(FrameWindow::MiddleMiddle, Helper::stringToImage("set:ui1 image:tooltip_MM"));
	tooltip2->setSectionImage(FrameWindow::MiddleRight, Helper::stringToImage("set:ui1 image:tooltip_MR"));
	tooltip2->setSectionImage(FrameWindow::BottomLeft, Helper::stringToImage("set:ui1 image:tooltip_BL"));
	tooltip2->setSectionImage(FrameWindow::BottomMiddle, Helper::stringToImage("set:ui1 image:tooltip_BM"));
	tooltip2->setSectionImage(FrameWindow::BottomRight, Helper::stringToImage("set:ui1 image:tooltip_BR"));
	tooltip2->setFont("Kt");
	sheet->addChildWindow(tooltip2);
	tooltip2->setVisible(false);

	return true;
}

bool ElSampleApplication::_setupGUICursor()
{
	if (ELGUI::System::getSingletonPtr())
	{
		long width = 0;
		long height = 0;

		HCURSOR hCursor = GetCursor();

		ICONINFO info;
		GetIconInfo(hCursor, &info);

		BITMAP bm;   
		if (GetObject(info.hbmColor, sizeof(bm), &bm) != 0)
		{
			width = bm.bmWidth;
			height = bm.bmHeight;
		}
		// 如果光标色图不存在，则计算掩图
		else if (GetObject(info.hbmMask, sizeof(bm), &bm) != 0)
		{
			width = bm.bmWidth;
			height = bm.bmHeight;
		}

		DeleteObject(info.hbmColor);
		DeleteObject(info.hbmMask);

		ELGUI::System::getSingleton().injectMouseSize(width, height);
	}

	return true;
}

void ElSampleApplication::_cleanupGUI()
{
	ElSafeDelete(mGUISystem);
	ElSafeDelete(mGUIRenderer);
}

void ElSampleApplication::_renderScreenElements()
{
	ElApplication::_renderScreenElements();
	mGUISystem->renderGUI();
}

void ElSampleApplication::_saveScreenImp(LPCSTR directory)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	IDirect3DSurface9* backBuffer = NULL;
	if (SUCCEEDED(d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)))
	{
		// check if the screen shots directory exists, if not, create one
		if (!ElPath::isFileExist(directory))
		{
			if (!ElPath::createDirectory(directory))
				return;
		}

		// construct file name according to the current system time
		char filename[EL_MAX_PATH] = {0};
		char path[EL_MAX_PATH] = {0};

		SYSTEMTIME systm;
		GetSystemTime(&systm);

		sprintf(filename, "ScrnShot_%02d%02d%d_%02d%02d%02d%d.jpg", systm.wDay, systm.wMonth, systm.wYear, systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds);
		sprintf(path, "%s%s", directory, filename);

		// save rendered backBuffer to file
		D3DXSaveSurfaceToFile(path, D3DXIFF_JPG, backBuffer, NULL, NULL);

		ElSafeRelease(backBuffer);
	}
}

