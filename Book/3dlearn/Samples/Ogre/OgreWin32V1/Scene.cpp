#include "stdAfx.h"

#include "Scene.h"

#include "resource.h"



CScene::CScene(void)
{
}

CScene::~CScene(void)
{
	// I'm sure I need to be deleting some things, but I'm not 
	// that familiar with Ogre memory management, yet.
}

//
// Name :         CScene::CreateScene(Ogre::SceneManager *pSceneManager)
// Description :  Create an Ogre scene.  I'm creating a simple scene with
//                a robot and a ninja. Both start walking.
//

void CScene::CreateScene(Ogre::SceneManager *pSceneManager)
{

	pSceneManager->setAmbientLight( ColourValue( 1.0f, 1.0f, 1.0f ) );

	m_robotEntity = pSceneManager->createEntity("robot", "robot.mesh");

	// Add entity to the scene node
	// Place and rotate to face the Z direction
	Vector3 robotLoc(30, 0, 0);
	Quaternion robotRot(Degree(-90), Vector3(0, 1, 0));

	pSceneManager->getRootSceneNode()->createChildSceneNode(robotLoc, robotRot)->attachObject(m_robotEntity);

	m_robotWalkState = m_robotEntity->getAnimationState("Walk");
	m_robotWalkState->setEnabled(true);

	// Set the initial state
	m_robotState = ID_ROBOT_WALK;
	m_robotCurrentState = m_robotWalkState;
	m_robotReturnToWalk = false;

	//
	// Create the ninja entity
	//

	Entity *ent = pSceneManager->createEntity("ninja", "ninja.mesh");

	// Add entity to the scene node
	// Place and rotate to face the Z direction
	Vector3 ninjaLoc(-30, 0, 0);
	Quaternion ninjaRot(Degree(180), Vector3(0, 1, 0));

	SceneNode *ninjaNode = pSceneManager->getRootSceneNode()->createChildSceneNode(ninjaLoc, ninjaRot);
	ninjaNode->scale(0.5, 0.5, 0.5);        // He's twice as big as our robot...
	ninjaNode->attachObject(ent);

	m_ninjaWalkState = ent->getAnimationState("Walk");
	m_ninjaWalkState->setEnabled(true);
}

//
// Name :         CScene::frameStarted(const FrameEvent& evt)
// Description :  The callback function to indicate a new frame is started.
//

bool CScene::frameStarted(const FrameEvent& evt)
{
	m_robotCurrentState->addTime(evt.timeSinceLastFrame);

	// Handle possible end of robot state if we the return to walk 
	// flag is set indicating we return to walk mode when the existing mode ends.
	if(m_robotReturnToWalk)
	{
		if(m_robotCurrentState->getTimePosition() >= m_robotCurrentState->getLength())
		{
			SetRobotState(ID_ROBOT_WALK);
		}
	}

	m_ninjaWalkState->addTime(evt.timeSinceLastFrame);

	return true;
}

//
// Name :         CScene::SetRobotState(int p_state)
// Description :  Set the current robot state.  Allows us to select walking or shooting.
//

void CScene::SetRobotState(int p_state)
{   
	// 
	// Determine what we are doing from and what we are going to
	//

	Ogre::AnimationState *oldState = m_robotCurrentState;
	Ogre::AnimationState *newState;

	m_robotState = p_state;
	m_robotReturnToWalk = false;      // Default is not to return to walk

	switch(m_robotState)
	{
	case ID_ROBOT_WALK:
		newState = m_robotWalkState;
		break;

	case ID_ROBOT_SHOOT:
		newState = m_robotEntity->getAnimationState("Shoot");
		newState->setLoop(false);              // We'll not loop this state
		m_robotReturnToWalk = true;         // Return to walking when done
		break;

	case ID_ROBOT_IDLE:
		newState = m_robotEntity->getAnimationState("Idle");
		break;

	case ID_ROBOT_SLUMP:
		newState = m_robotEntity->getAnimationState("Slump");
		newState->setLoop(false);              // We'll not loop this state
		break;

	case ID_ROBOT_DIE:
		newState = m_robotEntity->getAnimationState("Die");
		newState->setLoop(false);              // We'll not loop this state
		break;
	}

	m_robotCurrentState = newState;

	//
	// Disable the old state
	//

	oldState->setEnabled(false);

	// 
	// Enable the new state
	//

	newState->setEnabled(true);
	newState->setTimePosition(0);
}
