#include "stdafx.h"
#include "NoxApplication.h"

NoxApplication::NoxApplication(void)
: mGUIRenderer(NULL)
//, mVolQuery(NULL)
, mMouseListener(NULL)
, mFrameListener(NULL)
{

}

NoxApplication::~NoxApplication(void)
{
	if (mFrameListener)
		delete mFrameListener;
	//if (mVolQuery)
	//	delete mVolQuery;
	if (mMouseListener)
		delete mMouseListener;
	//if (mGUIRenderer)
	//	delete mGUIRenderer;
	if (mTrayListener)
		delete mTrayListener;


}

bool NoxApplication::configure(void)
{
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if(/*mRoot->restoreConfig() || */mRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true);
		return true;
	}
	else
	{
		return false;
	}
}

void NoxApplication::createListener(void)
{
	LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem( pl );

	//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

	//! mouse listener
	mMouseListener = new NoxMouseListener(mSceneMgr ,mCamera , mCameraMan );
	mMouse->setEventCallback(mMouseListener);

	//! tray
	mTrayListener = new NoxTrayListener;
	mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mMouse, mTrayListener);
	mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	//mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
	mTrayMgr->hideCursor();

	//! keyboard listener
	mKeyboardListener = new NoxKeyboardListener(mWindow , mCamera , mCameraMan , mTrayMgr  , NULL );
	mKeyboard->setEventCallback(mKeyboardListener);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//! window event
	mWindowListener = new NoxWindowListener(mWindow , mInputManager , mMouse  , mKeyboard );
	WindowEventUtilities::addWindowEventListener(mWindow, mWindowListener);	

	//! frame listener
	mFrameListener= new MoveDemoListener(mWindow, mCamera, mCameraMan , mTrayMgr ,mMouse , mKeyboard ,  mRobotNode, mEntity, mWalkList);
	mFrameListener->showDebugOverlay(true);
	mRoot->addFrameListener(mFrameListener);




}


void NoxApplication::createUI()
{

}



void NoxApplication::createScene(void)
{
	// Set the default lighting.
	mSceneMgr->setAmbientLight( ColourValue( 1.0f, 1.0f, 1.0f ) );

	createAxes();
	createGrid();
	createPath();

	// Create the entity
	mEntity = mSceneMgr->createEntity( "Robot", "robot.mesh" );
	mRobotNode = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "RobotNode", Vector3( 0.0f, 0.0f, 25.0f ) );
	mRobotNode->attachObject( mEntity );
	//mRobotNode->showBoundingBox(true);


	// Create the walking list
	mWalkList.push_back( Vector3( 550.0f,  0.0f,  50.0f  ) );
	mWalkList.push_back( Vector3(-100.0f,  0.0f, -200.0f ) );
	mWalkList.push_back( Vector3(  0.0f, 0.0f, 25.0f ) );


	// Create objects so we can see movement
	Entity *ent;
	SceneNode *node;

	ent = mSceneMgr->createEntity( "ninja", "ninja.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "ninja", Vector3(  50.0f, 0.0f,  -50.0f ) );
	node->attachObject( ent );
	node->yaw( Degree(-180) );


	ent = mSceneMgr->createEntity( "Knot1", "axes.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot1Node",
		Vector3(  0.0f, 0.0f,  25.0f ) );
	node->attachObject( ent );
	//node->setScale( 0.1f, 0.1f, 0.1f );

	ent = mSceneMgr->createEntity( "Knot2", "axes.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot2Node",
		Vector3( 550.0f, -0.0f,  50.0f ) );
	node->attachObject( ent );
	//node->setScale( 0.1f, 0.1f, 0.1f );

	ent = mSceneMgr->createEntity( "Knot3", "axes.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot3Node",
		Vector3(-100.0f, -0.0f,-200.0f ) );
	node->attachObject( ent );
	//node->setScale( 0.1f, 0.1f, 0.1f );


	// Set the camera to look at our handywork
	mCamera->setPosition( 90.0f, 280.0f, 535.0f );
	mCamera->pitch( Degree(-30.0f) );
	mCamera->yaw( Degree(-15.0f) );


	//! TEST ONLY

	//AnimationState *mAnimationState; // The current animation state of the object

	//mAnimationState = mEntity->getAnimationState("Idle");
	//mAnimationState->setLoop(true);
	//mAnimationState->setEnabled(true);


	//!
	//set up CEGUI
	mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

	CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"TaharezLook.scheme");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");


	//mVolQuery = mSceneMgr->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());



}


void NoxApplication::createPath()
{
	Ogre::ManualObject* pManualObject = mSceneMgr-> createManualObject( "PathObj"); 


	if (pManualObject)
	{
		pManualObject-> begin( "",   Ogre::RenderOperation::OT_LINE_LIST); 
		pManualObject-> colour(Ogre::ColourValue(1.0 , 1.0 , 0 )); 

		pManualObject-> position(Vector3( 0.0f, 1.0f, 25.0f )); 
		pManualObject-> position(Vector3( 550.0f,  1.0f,  50.0f  )); 

		pManualObject-> position(Vector3( 550.0f,  1.0f,  50.0f  )); 
		pManualObject-> position(Vector3(-100.0f,  1.0f, -200.0f ) ); 

		pManualObject-> position(Vector3(-100.0f,  1.0f, -200.0f ) ); 
		pManualObject-> position(Vector3( 0.0f, 1.0f, 25.0f )); 

		pManualObject-> end(); 

		pManualObject->getSection(0)->getTechnique()->getPass(0)->setLightingEnabled(false);

		Ogre::SceneNode*pSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "PathNode "); 
		if(pSceneNode) 
			pSceneNode-> attachObject(pManualObject);

	}
}


void NoxApplication::createGrid()
{

	Ogre::ManualObject*   pManualObject   =   mSceneMgr-> createManualObject( "TestManualObject "); 
	if   (NULL   ==   pManualObject) 
		return   ; 

	pManualObject-> begin( " ",   Ogre::RenderOperation::OT_LINE_LIST); 
	{ 
		int startX=-1000 ; 
		int startZ = -1000;
		int endX=1000 ; 
		int endZ = 1000;
		int step = 50;
		//int nLength = 160 ;

		pManualObject-> colour(Ogre::ColourValue(0.5,0.5,0.5) ); 

		for (int idx = 0 ; idx< (endX - startX)/step ; ++idx )
		{
			pManualObject-> position(startX+step*idx,   0,   startZ); 
			pManualObject-> position(startX+step*idx,   0,   endZ); 
		}

		for (int idx = 0 ; idx< (endZ - startZ)/step ; ++idx )
		{
			pManualObject-> position(startX,   0,   startZ+step*idx ); 
			pManualObject-> position(endX,   0,   startZ+step*idx); 
		}

	} 
	pManualObject-> end(); 

	pManualObject->getSection(0)->getTechnique()->getPass(0)->setLightingEnabled(false);

	Ogre::SceneNode*   pSceneNode   =   mSceneMgr->getRootSceneNode()->createChildSceneNode( "TestSceneNode "); 
	if   (NULL   ==   pSceneNode) 
		return   ; 
	pSceneNode-> attachObject(pManualObject);


}


void NoxApplication::createAxes()
{

	Ogre::ManualObject* pManualObject = mSceneMgr-> createManualObject( "ManualGridObj "); 
	if (pManualObject)
	{
		int nLength = 160 ;
		pManualObject-> begin( "",   Ogre::RenderOperation::OT_LINE_LIST); 
		pManualObject-> position(0,   0,   0); 
		pManualObject-> colour(Ogre::ColourValue::Red); 
		pManualObject-> position(nLength*4,   0,   0); 

		pManualObject-> position(0,   0,   0); 
		pManualObject-> colour(Ogre::ColourValue::Green); 
		pManualObject-> position(0,   nLength,   0); 

		pManualObject-> position(0,   0,   0); 
		pManualObject-> colour(Ogre::ColourValue::Blue); 
		pManualObject-> position(0,   0,   nLength); 
		pManualObject-> end(); 

		pManualObject->getSection(0)->getTechnique()->getPass(0)->setLightingEnabled(false);

		Ogre::SceneNode*pSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "ManualGridNode "); 
		if(pSceneNode) 
			pSceneNode-> attachObject(pManualObject);

	}

}