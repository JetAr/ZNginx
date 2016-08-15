#include "MoveDemoApplication.h"

MoveDemoApplication::MoveDemoApplication(void)
{

}

MoveDemoApplication::~MoveDemoApplication(void)
{
}

void MoveDemoApplication::createFrameListener(void)
{
	mFrameListener= new MoveDemoListener(mWindow, mCamera, mNode, mEntity, mWalkList);
	mFrameListener->showDebugOverlay(true);
	mRoot->addFrameListener(mFrameListener);
}


void MoveDemoApplication::createScene(void)
{
	// Set the default lighting.
	mSceneMgr->setAmbientLight( ColourValue( 1.0f, 1.0f, 1.0f ) );

	createAxes();
	createGrid();
	createPath();

	// Create the entity
	mEntity = mSceneMgr->createEntity( "Robot", "robot.mesh" );
	mNode = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "RobotNode", Vector3( 0.0f, 0.0f, 25.0f ) );
	mNode->attachObject( mEntity );
	mNode->showBoundingBox(true);


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


	ent = mSceneMgr->createEntity( "Knot1", "knot.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot1Node",
		Vector3(  0.0f, -10.0f,  25.0f ) );
	node->attachObject( ent );
	node->setScale( 0.1f, 0.1f, 0.1f );

	ent = mSceneMgr->createEntity( "Knot2", "knot.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot2Node",
		Vector3( 550.0f, -10.0f,  50.0f ) );
	node->attachObject( ent );
	node->setScale( 0.1f, 0.1f, 0.1f );

	ent = mSceneMgr->createEntity( "Knot3", "knot.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot3Node",
		Vector3(-100.0f, -10.0f,-200.0f ) );
	node->attachObject( ent );
	node->setScale( 0.1f, 0.1f, 0.1f );


	// Set the camera to look at our handywork
	mCamera->setPosition( 90.0f, 280.0f, 535.0f );
	mCamera->pitch( Degree(-30.0f) );
	mCamera->yaw( Degree(-15.0f) );


	//! TEST ONLY

	//AnimationState *mAnimationState; // The current animation state of the object

	//mAnimationState = mEntity->getAnimationState("Idle");
	//mAnimationState->setLoop(true);
	//mAnimationState->setEnabled(true);


}


void MoveDemoApplication::createPath()
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


void MoveDemoApplication::createGrid()
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


void MoveDemoApplication::createAxes()
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