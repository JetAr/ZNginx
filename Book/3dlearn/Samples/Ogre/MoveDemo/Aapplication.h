/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    ExampleApplication.h
Description: Base class for all the OGRE examples
-----------------------------------------------------------------------------
*/

#ifndef __ExampleApplication_H__
#define __ExampleApplication_H__



#include "NoxFrameListener.h"
#include "NoxMouseListener.h"




using namespace Ogre;


class Application
{
public:
	/// Standard constructor
	Application();
	/// Standard destructor
	virtual ~Application();
	/// Start the example
	virtual void go(void);

protected:
	Root *mRoot;
	Camera* mCamera;
	OgreBites::SdkCameraMan* mCameraMan;       // basic camera controller
	SceneManager* mSceneMgr;
	RenderWindow* mWindow;
	Ogre::String mResourcePath;
	Ogre::String mConfigPath;


	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;



	// These internal methods package up the stages in the startup process
	/** Sets up the application - returns false if the user chooses to abandon configuration. */
	virtual bool setup(void);
	//Adjust mouse clipping area
	virtual void windowResized(RenderWindow* rw);
	/** Configures the application - returns false if the user chooses to abandon configuration. */
	virtual bool configure(void);
	virtual void chooseSceneManager(void);
	virtual void createCamera(void);
	virtual void createListener(void);
	virtual void createScene(void) = 0;    // pure virtual - this has to be overridden

	virtual void destroyScene(void);    // Optional to override this

	virtual void createViewports(void);
	/// Method which will define the source of resources (other than current folder)
	virtual void setupResources(void);
	/// Optional override method where you can create resource listeners (e.g. for loading screens)
	virtual void createResourceListener(void);
	/// Optional override method where you can perform resource group loading
	/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	virtual void loadResources(void);
};

#endif
