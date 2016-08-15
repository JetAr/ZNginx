#include "stdafx.h"
#include "NoxWindowListener.h"


NoxWindowListener::NoxWindowListener(Ogre::RenderWindow* window , OIS::InputManager* inputMgr 
									 , OIS::Mouse* mouse , OIS::Keyboard* keyboard)
 : mWindow(window)
 , mInputManager(inputMgr)
 , mMouse(mouse)
 , mKeyboard(keyboard)
{


}

NoxWindowListener::~NoxWindowListener(void)
{
}

//Adjust mouse clipping area
void NoxWindowListener::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void NoxWindowListener::windowClosed(Ogre::RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if( rw == mWindow )
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject( mMouse );
			mInputManager->destroyInputObject( mKeyboard );

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}
