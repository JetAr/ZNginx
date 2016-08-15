#pragma once

//#include <OgreRenderWindow.h>
//#include <OgreViewport.h>
#include <OgreWindowEventUtilities.h>
#include <OIS.h>

using namespace Ogre;

class NoxWindowListener :
	public Ogre::WindowEventListener
{
public:
	NoxWindowListener(Ogre::RenderWindow* window , OIS::InputManager* inputMgr , OIS::Mouse* mouse , OIS::Keyboard* keyboard);
	virtual ~NoxWindowListener(void);

	// Ogre::WindowEventListener
	//Adjust mouse clipping area
	virtual void windowResized(Ogre::RenderWindow* rw);
	//Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(Ogre::RenderWindow* rw);

	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;

	Ogre::RenderWindow* mWindow;



};
