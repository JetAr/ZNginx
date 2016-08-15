#pragma once


#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>


class NoxKeyboardListener :
	public OIS::KeyListener
{
public:
	NoxKeyboardListener(Ogre::RenderWindow* window , Ogre::Camera* camera
		, OgreBites::SdkCameraMan* cameraMan , 	OgreBites::SdkTrayManager* trayMgr , OgreBites::ParamsPanel* detailsPanel);
	virtual ~NoxKeyboardListener(void);

	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );


	// OgreBites
	Ogre::RenderWindow* mWindow;
	Ogre::Camera* mCamera;
	OgreBites::SdkCameraMan* mCameraMan;       // basic camera controller
	OgreBites::SdkTrayManager* mTrayMgr;
	OgreBites::ParamsPanel* mDetailsPanel;     // sample details panel

};
