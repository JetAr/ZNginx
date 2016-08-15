#include "stdafx.h"
#include "MoveDemoListener.h"
//MoveDemoListener::MoveDemoListener(void)
//{
//}
//
//MoveDemoListener::~MoveDemoListener(void)
//{
//}

MoveDemoListener::MoveDemoListener(RenderWindow* win, Camera* cam, OgreBites::SdkCameraMan* cameraMan , OgreBites::SdkTrayManager* trayMgr 
								   , OIS::Mouse*    mouse,	OIS::Keyboard* keyboard ,  SceneNode *sn, Entity *ent, std::deque<Vector3> &walk
								   )
								   : NoxFrameListener(win, cam , cameraMan , trayMgr ,mouse , keyboard ,  false, false), mNode(sn), mEntity(ent), mWalkList( walk )
{

	//// Set idle animation
	//mAnimationState = ent->getAnimationState("Idle");
	//mAnimationState->setLoop(true);
	//mAnimationState->setEnabled(true);

	// Set default values for variables
	mWalkSpeed = 35.0f*2;
	mDirection = Vector3::ZERO;

} // MoveDemoListener

/* This function is called to start the object moving to the next position
in mWalkList.
*/
bool MoveDemoListener::nextLocation( )
{
	if (mWalkList.empty())
	{
		mWalkList.push_back( Vector3( 550.0f,  0.0f,  50.0f  ) );
		mWalkList.push_back( Vector3(-100.0f,  0.0f, -200.0f ) );
		mWalkList.push_back( Vector3(  0.0f, 0.0f, 25.0f ) );


	}
	//return false;

	mDestination = mWalkList.front();  // this gets the front of the deque
	mWalkList.pop_front();             // this removes the front of the deque
	mDirection = mDestination - mNode->getPosition();
	mDistance = mDirection.normalise();


	//Vector3 src = mNode->getOrientation() * Vector3::UNIT_X;
	//Ogre::Quaternion quat = src.getRotationTo(mDirection);
	//mNode->rotate(quat);
	const Quaternion& orientation = mNode->getOrientation();

	//! 获得了机器人脸的朝向
	Vector3 src =  orientation* Vector3::UNIT_X;
	if ((1.0f + src.dotProduct(mDirection)) < 0.0001f) 
	{
		mNode->yaw(Degree(180));
	}
	else
	{
		Ogre::Quaternion quat = src.getRotationTo(mDirection);
		mNode->rotate(quat);
	} // else


	//Ogre::Quaternion quat = src.getRotationTo(mDirection);

	//mNode->rotate(quat);


	return true;
} // nextLocation( )

bool MoveDemoListener::frameStarted(const FrameEvent &evt)
{

	if (mDirection == Vector3::ZERO) 
	{
		if (nextLocation()) 
		{
			// Set walking animation
			mAnimationState = mEntity->getAnimationState("Walk");
			mAnimationState->setLoop(true);
			mAnimationState->setEnabled(true);
		}
		//else
		//{
		//	mAnimationState = mEntity->getAnimationState("Die");
		//	mAnimationState->setLoop(false);
		//	mAnimationState->setEnabled(false);
		//}

	}
	else
	{
		Real move = mWalkSpeed * evt.timeSinceLastFrame;
		mDistance -= move;
		if (mDistance <= 0.0f)
		{
			mNode->setPosition(mDestination);
			mDirection = Vector3::ZERO;
			// Set animation based on if the robot has another point to walk to. 
			if (! nextLocation())
			{
				// Set Idle animation                     
				mAnimationState = mEntity->getAnimationState("Idle");
				mAnimationState->setLoop(true);
				mAnimationState->setEnabled(true);
			} 
			else
			{
				// Rotation Code will go here later
			}
		}
		else
		{
			mNode->translate(mDirection * move);
		} // else
	} // if


	mAnimationState->addTime(evt.timeSinceLastFrame);


	return NoxFrameListener::frameStarted(evt);
}