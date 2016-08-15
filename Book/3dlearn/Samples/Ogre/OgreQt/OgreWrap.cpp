#include "OgreWrap.h"

using namespace Ogre;
OgreWrap::OgreWrap(void)
{
	m_OgreRoot = 0;         // Until we know otherwise
	m_SceneManager = 0;
	m_Camera = 0;
	m_Window = 0;

}

OgreWrap::~OgreWrap(void)
{
}

void OgreWrap::SetupOgre(HWND hWnd)
{
	// Obtain pointer to the Ogre root object
	m_OgreRoot = Ogre::Root::getSingletonPtr();

	//
	// Create a render window
	// This window should be the current ChildView window using the externalWindowHandle
	// value pair option.
	//

	Ogre::NameValuePairList parms;
	parms["externalWindowHandle"] = Ogre::StringConverter::toString((long)hWnd);

	RECT   rect;
	::GetClientRect(hWnd , &rect);

	m_Window = m_OgreRoot->createRenderWindow("MFC Window", rect.right - rect.left, rect.bottom - rect.top, false, &parms);

	//
	// Choose a scene manager
	//
	CreateSceneManager();

	// Create the camera
	CreateCamera();

	//
	// Create a viewport
	//
	CreateViewPort(rect);

	//
	// Load resources
	//
	loadResources();

	//
	// Create the scene
	//

	CreateScene();

	// Make window active and post an update
	m_Window->setActive(true);
	m_Window->update();
}
// =============================================================================
// CChildView::CreateSceneManager(void)
// -----------------------------------------------------------------------------
///
// =============================================================================
void OgreWrap::CreateSceneManager(void)
{
	m_SceneManager = m_OgreRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");
}
// =============================================================================
// CChildView::CreateCamera(void)
// -----------------------------------------------------------------------------
///
// =============================================================================
void OgreWrap::CreateCamera(void)
{
	///´´½¨ÉãÏñ»ú
	m_Camera = m_SceneManager->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	m_Camera->setPosition(Vector3(0,0,500));
	// Look back along -Z
	m_Camera->lookAt(Vector3(0,0,-300));
	m_Camera->setNearClipDistance(1);
}
// =============================================================================
// CChildView::CreateViewPort(void)
// -----------------------------------------------------------------------------
///
// =============================================================================
void OgreWrap::CreateViewPort(RECT& rect)
{
	// Create one viewport, entire window
	Ogre::Viewport* vp = m_Window->addViewport(m_Camera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,1));

	// Alter the camera aspect ratio to match the viewport
	float aspact = (rect.right - rect.left)/(rect.bottom - rect.top);
	m_Camera->setAspectRatio( aspact );

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
}
// =============================================================================
// CChildView::loadResources(void)
// -----------------------------------------------------------------------------
///
// =============================================================================
void OgreWrap::loadResources()
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
// =============================================================================
// CChildView::CreateScene(void)
// -----------------------------------------------------------------------------
///Create a scene in our Ogre system.
// =============================================================================
void OgreWrap::CreateScene(void)
{
	// Setup animation default
	Animation::setDefaultInterpolationMode(Animation::IM_LINEAR);
	Animation::setDefaultRotationInterpolationMode(Animation::RIM_LINEAR);

	// Set ambient light
	m_SceneManager->setAmbientLight(ColourValue(0.5, 0.5, 1));

	// Create the robot scene
	m_scene.CreateScene(m_SceneManager);

	// Give it a little ambience with lights
	Light* l;
	l = m_SceneManager->createLight("BlueLight");
	l->setPosition(-0,100,100);
	l->setDiffuseColour(0.5, 0.5, 1.0);

	l = m_SceneManager->createLight("GreenLight");
	l->setPosition(0,0,-100);
	l->setDiffuseColour(0.5, 1.0, 0.5);

	// Position the camera
	m_Camera->setPosition(50, 150, 200);
	m_Camera->lookAt(0, 50, 0);

	// Add the frame listenter
	m_OgreRoot->addFrameListener(&m_scene);
}


void OgreWrap::SetRobotState(int state)
{
	m_scene.SetRobotState(state);

}

void OgreWrap::OnSize()
{
	if (m_Window) 
	{ 
		//RECT rect; 
		//GetClientRect(&rect); 

		// notify "render window" instance 
		m_Window->windowMovedOrResized(); 

		// Adjust camera's aspect ratio, too 
		if (/*rect.Height() != 0 &&*/ m_Camera != 0) 
			m_Camera->setAspectRatio((Ogre::Real)m_Window->getWidth() / (Ogre::Real)m_Window->getHeight()); 

		m_Camera->yaw(Radian(0));
	} 
}
