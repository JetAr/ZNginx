#include "SinbadCharacterController.h"




SinbadCharacterController::SinbadCharacterController(Camera* cam)
{
	setupBody(cam->getSceneManager());
	setupCamera(cam);
	setupAnimations();

	mFreeLook = false;
	mLeftMouseDown = false;
	mRightMouseDown = false;

	mRotX=mRotY=0;


}

void SinbadCharacterController::addTime(Real deltaTime)
{
	if (!mFreeLook)
	{
		updateBody(deltaTime);
		updateAnimations(deltaTime);
		updateCamera(deltaTime);
	}
	else
	{
		mAnims[ANIM_RUN_BASE]->setEnabled(true);
		mAnims[ANIM_RUN_BASE]->setLoop(true);
		mAnims[ANIM_RUN_BASE]->addTime(deltaTime);

		mAnims[ANIM_RUN_TOP]->setEnabled(true);
		mAnims[ANIM_RUN_TOP]->setLoop(true);
		mAnims[ANIM_RUN_TOP]->addTime(deltaTime);


		//updateAnimations(deltaTime);

		//mCameraNode->translate(mCameraTranslate);
		//mCameraNode->yaw(mRotX);
		//mCameraNode->pitch(mRotY);

		mRotX = 0;
		mRotY = 0;
		mCameraTranslate = Ogre::Vector3::ZERO;


		// place the camera pivot roughly at the character's shoulder
		mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3::UNIT_Y * CAM_HEIGHT);
		// move the camera smoothly to the goal
		//Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
		//mCameraNode->translate(goalOffset * deltaTime * 9.0f);
		mCameraNode->setPosition(Vector3(0 , 30 , 40)  );
		// always look at the pivot
		//mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Node::TS_WORLD);

	}
	mRotX=mRotY=0;


	//SceneNode* mBodyNode;
	//SceneNode* mCameraPivot;
	//SceneNode* mCameraGoal;
	//SceneNode* mCameraNode;



}

void SinbadCharacterController::injectKeyDown(const OIS::KeyEvent& evt)
{
	if (evt.key == OIS::KC_Q && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		// take swords out (or put them back, since it's the same animation but reversed)
		setTopAnimation(ANIM_DRAW_SWORDS, true);
		mTimer = 0;
	}
	else if (evt.key == OIS::KC_E && !mSwordsDrawn)
	{
		if (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)
		{
			// start dancing
			setBaseAnimation(ANIM_DANCE, true);
			setTopAnimation(ANIM_NONE);
			// disable hand animation because the dance controls hands
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(false);
		}
		else if (mBaseAnimID == ANIM_DANCE)
		{
			// stop dancing
			setBaseAnimation(ANIM_IDLE_BASE);
			setTopAnimation(ANIM_IDLE_TOP);
			// re-enable hand animation
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
		}
	}

	// keep track of the player's intended direction
	else if (evt.key == OIS::KC_W) mKeyDirection.z = -1;
	else if (evt.key == OIS::KC_A) mKeyDirection.x = -1;
	else if (evt.key == OIS::KC_S) mKeyDirection.z = 1;
	else if (evt.key == OIS::KC_D) mKeyDirection.x = 1;
	else if (evt.key == OIS::KC_K)
	{
		mKeyDirection.z = mKeyDirection.z==-1?0:-1;
	}
	else if (evt.key == OIS::KC_L)
	{
		mFreeLook = !mFreeLook;
	}

	else if (evt.key == OIS::KC_SPACE && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		// jump if on ground
		setBaseAnimation(ANIM_JUMP_START, true);
		setTopAnimation(ANIM_NONE);
		mTimer = 0;
	}

	if (!mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_IDLE_BASE)
	{
		// start running if not already moving and the player wants to move
		setBaseAnimation(ANIM_RUN_BASE, true);
		if (mTopAnimID == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
	}
}

void SinbadCharacterController::injectKeyUp(const OIS::KeyEvent& evt)
{
	// keep track of the player's intended direction
	if (evt.key == OIS::KC_W && mKeyDirection.z == -1) mKeyDirection.z = 0;
	else if (evt.key == OIS::KC_A && mKeyDirection.x == -1) mKeyDirection.x = 0;
	else if (evt.key == OIS::KC_S && mKeyDirection.z == 1) mKeyDirection.z = 0;
	else if (evt.key == OIS::KC_D && mKeyDirection.x == 1) mKeyDirection.x = 0;

	if (mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_RUN_BASE)
	{
		// stop running if already moving and the player doesn't want to move
		setBaseAnimation(ANIM_IDLE_BASE);
		if (mTopAnimID == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
	}
}


void SinbadCharacterController::injectMouseMove(const OIS::MouseEvent& evt)
{
	//updateCameraGoal(-0.05f * evt.state.X.rel, -0.05f * evt.state.Y.rel, -0.0005f * evt.state.Z.rel);

	if (!mFreeLook)
	{
		//update camera goal based on mouse movement
		updateCameraGoal(-0.05f * evt.state.X.rel, -0.05f * evt.state.Y.rel, -0.0005f * evt.state.Z.rel);
	}
	else
	{
		if ( mLeftMouseDown)
		{
			mRotX = Degree(-evt.state.X.rel * 0.13);
			mRotY = Degree(-evt.state.Y.rel * 0.13);
			mCameraNode->yaw(mRotX);
			mCameraNode->pitch(mRotY);
		}
		else if (mRightMouseDown)
		{
			mCameraTranslate.x += evt.state.X.rel * 0.5;
			mCameraTranslate.y -= evt.state.X.rel * 0.5;

		}
		//mCameraNode->moveRelative(mCameraTranslate);

	}
}

void SinbadCharacterController::injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	//bool mLeftMouseDown;
	//bool mRightMouseDown;
	if (id == OIS::MB_Left) mLeftMouseDown =  true;
	else if (id == OIS::MB_Right)  mRightMouseDown =  true;

	if (mSwordsDrawn && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		// if swords are out, and character's not doing something weird, then SLICE!
		if (id == OIS::MB_Left) setTopAnimation(ANIM_SLICE_VERTICAL, true);
		else if (id == OIS::MB_Right) setTopAnimation(ANIM_SLICE_HORIZONTAL, true);
		mTimer = 0;
	}
}
void SinbadCharacterController::injectMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	mLeftMouseDown = false;
	mRightMouseDown = false;
}


void SinbadCharacterController::setupBody(SceneManager* sceneMgr)
{
	// create main model
	mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode("SinbadBody" , Vector3::UNIT_Y * CHAR_HEIGHT);
	mBodyEnt = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
	mBodyNode->attachObject(mBodyEnt);
	//mBodyNode->showBoundingBox(true);

	//! campass
	//Entity* campass = sceneMgr->createEntity("axes.mesh");
	//SceneNode* node = mBodyNode->createChildSceneNode("campassnode");
	//node->attachObject(campass);
	//node->translate( 0 ,  0 ,5 );
	//node->scale(.2,.2,.2);
	//MovableText* mtCampass = new MovableText("campass", "campass");
	//node->attachObject(mtCampass);



	// create swords and attach to sheath
	mSword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
	mSword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
	mBodyEnt->attachObjectToBone("Sheath.L", mSword1);
	mBodyEnt->attachObjectToBone("Sheath.R", mSword2);

	// create a couple of ribbon trails for the swords, just for fun
	NameValuePairList params;
	params["numberOfChains"] = "2";
	params["maxElements"] = "80";
	mSwordTrail = (RibbonTrail*)sceneMgr->createMovableObject("RibbonTrail", &params);
	mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
	mSwordTrail->setTrailLength(20);
	mSwordTrail->setVisible(false);
	sceneMgr->getRootSceneNode()->attachObject(mSwordTrail);


	for (int i = 0; i < 2; i++)
	{
		mSwordTrail->setInitialColour(i, 1, 0.8, 0);
		mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
		mSwordTrail->setWidthChange(i, 1);
		mSwordTrail->setInitialWidth(i, 0.5);
	}

	mKeyDirection = Vector3::ZERO;
	mVerticalVelocity = 0;



	//! 电子牌
	MovableText* msg = new MovableText("TXT_001", "Sinbad");
	msg->setTextAlignment(MovableText::H_CENTER, MovableText::V_CENTER); // Center horizontally and display above the node
	//msg->setCharacterHeight( 0.0f ); //msg->setAdditionalHeight( ei.getRadius() )
	mBodyNode->attachObject(msg);
	msg->setColor(ColourValue::Blue);
	//const AxisAlignedBox& aabbmt = msg->getBoundingBox();


	const AxisAlignedBox& aabb_body = mBodyEnt->getBoundingBox();
	Vector3 size3D = aabb_body.getSize();
	msg->setLocalTranslation(Vector3(0 , size3D.y/2 , 0));


}



void SinbadCharacterController::setupAnimations()
{
	// this is very important due to the nature of the exported animations
	mBodyEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

	String animNames[] =
	{"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
	"SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

	// populate our animation list
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		mAnims[i] = mBodyEnt->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// start off in the idle state (top and bottom together)
	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);

	// relax the hands since we're not holding anything
	mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

	mSwordsDrawn = false;
}

void SinbadCharacterController::setupCamera(Camera* cam)
{
	// create a pivot at roughly the character's shoulder
	mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	// this is where the camera should be soon, and it spins around the pivot
	mCameraGoal = mCameraPivot->createChildSceneNode(Vector3(0, 0, 15));


	//Entity* axes1 = cam->getSceneManager()->createEntity("axes.mesh");
	//Entity* axes2 = cam->getSceneManager()->createEntity("axes.mesh");
	//Entity* axes3 = cam->getSceneManager()->createEntity("axes.mesh");

	//mCameraGoal->attachObject(axes1);
	//mCameraGoal->showBoundingBox(true);
	//mCameraGoal->scale(.2 , .2 , .2);
	//mCameraPivot->attachObject(axes2);
	//mCameraPivot->scale(.2 , .2 , .2);
	//mCameraGoal->showBoundingBox(true);
	// this is where the camera actually is
	mCameraNode = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mCameraNode->setPosition(mCameraPivot->getPosition() + mCameraGoal->getPosition());
	//mCameraNode->attachObject(axes3);
	//mCameraNode->scale(.2 , .2 , .2);



	//Vector3 vPivotPos = mCameraPivot->getPosition();
	//Vector3 vGoalPos = mCameraGoal->getPosition();
	//Vector3 vNodePos = mCameraNode->getPosition();


	mCameraPivot->setFixedYawAxis(true);
	mCameraGoal->setFixedYawAxis(true);
	mCameraNode->setFixedYawAxis(true);

	// our model is quite small, so reduce the clipping planes
	cam->setNearClipDistance(0.1);
	cam->setFarClipDistance(100);
	cam->setPosition(0 , 5 , 20 );
	mCameraNode->attachObject(cam);

	// mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Node::TS_WORLD);

	//mCameraPivot->lookAt(mBodyNode->_getDerivedPosition() , Node::TS_WORLD);

	mPivotPitch = 0;
	{
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   		//MovableText* pivotNode = new MovableText("pivotNode", "pivotNode");
		//MovableText* goalNode = new MovableText("goalNode", "goalNode");
		//MovableText* camNode = new MovableText("camNode", "camNode");

		//mCameraPivot->attachObject(pivotNode);
		//mCameraGoal->attachObject(goalNode);
		//mCameraNode->attachObject(camNode);

		//pivotNode->setColor(ColourValue::Red);
		//goalNode->setColor(ColourValue::Green);
		//camNode->setColor(ColourValue::Blue);
		

		
	}


}


void SinbadCharacterController::updateBody(Real deltaTime)
{
	mGoalDirection = Vector3::ZERO;   // we will calculate this

	if ( mBaseAnimID != ANIM_DANCE)
	{

		Quaternion q = mCameraNode->getOrientation();
		Vector3 v1 = q.xAxis();
		Vector3 v2 = q.yAxis();
		Vector3 v3 = q.zAxis();
		Vector3 v4 = v1*v2;


		// calculate actually goal direction in world based on player's key directions
		mGoalDirection += mKeyDirection.z * mCameraNode->getOrientation().zAxis();
		mGoalDirection += mKeyDirection.x * mCameraNode->getOrientation().xAxis();

		mGoalDirection.y = 0;
		mGoalDirection.normalise();

		//! 移动条件:自由模式、或者受控模式下按键方向不为0
		if(mFreeLook || (mKeyDirection != Vector3::ZERO))
		{
			//! 角色的正前方
			Vector3 charFront = mBodyNode->getOrientation().zAxis();
			Quaternion toGoal = charFront.getRotationTo(mGoalDirection);

			// calculate how much the character has to turn to face goal direction
			Real yawToGoal = toGoal.getYaw().valueDegrees();
			// this is how much the character CAN turn this frame
			Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
			// reduce "turnability" if we're in midair
			if (mBaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

			// turn as much as we can, but not more than we need to
			if (yawToGoal < 0) 
			{
				yawToGoal = std::min<Real>(0, std::max<Real>(yawToGoal, yawAtSpeed)); 
				//yawToGoal = Math::Clamp<Real>(yawToGoal, yawAtSpeed, 0);
			}
			else if (yawToGoal > 0)
			{
				yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal, yawAtSpeed)); 
				//yawToGoal = Math::Clamp<Real>(yawToGoal, 0, yawAtSpeed);
			}
			

			mBodyNode->yaw(Degree(yawToGoal));

			// move in current body direction (not the goal direction)
			mBodyNode->translate(0, 0, deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight(),Node::TS_LOCAL);

		}

	}

	if (mBaseAnimID == ANIM_JUMP_LOOP)
	{
		// if we're jumping, add a vertical offset too, and apply gravity
		mBodyNode->translate(0, mVerticalVelocity * deltaTime, 0, Node::TS_LOCAL);
		mVerticalVelocity -= GRAVITY * deltaTime;

		Vector3 pos = mBodyNode->getPosition();
		if (pos.y <= CHAR_HEIGHT)
		{
			// if we've hit the ground, change to landing state
			pos.y = CHAR_HEIGHT;
			mBodyNode->setPosition(pos);
			setBaseAnimation(ANIM_JUMP_END, true);
			mTimer = 0;
		}
	}


}

void SinbadCharacterController::updateAnimations(Real deltaTime)
{
	Real baseAnimSpeed = 1;
	Real topAnimSpeed = 1;

	mTimer += deltaTime;

	if (mTopAnimID == ANIM_DRAW_SWORDS)
	{
		// flip the draw swords animation if we need to put it back
		topAnimSpeed = mSwordsDrawn ? -1 : 1;

		// half-way through the animation is when the hand grasps the handles...
		if (mTimer >= mAnims[mTopAnimID]->getLength() / 2 &&
			mTimer - deltaTime < mAnims[mTopAnimID]->getLength() / 2)
		{
			// so transfer the swords from the sheaths to the hands
			mBodyEnt->detachAllObjectsFromBone();
			mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.L" : "Handle.L", mSword1);
			mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.R" : "Handle.R", mSword2);
			// change the hand state to grab or let go
			mAnims[ANIM_HANDS_CLOSED]->setEnabled(!mSwordsDrawn);
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(mSwordsDrawn);

			// toggle sword trails
			if (mSwordsDrawn)
			{
				mSwordTrail->setVisible(false);
				mSwordTrail->removeNode(mSword1->getParentNode());
				mSwordTrail->removeNode(mSword2->getParentNode());
			}
			else
			{
				mSwordTrail->setVisible(true);
				mSwordTrail->addNode(mSword1->getParentNode());
				mSwordTrail->addNode(mSword2->getParentNode());
			}
		}

		if (mTimer >= mAnims[mTopAnimID]->getLength())
		{
			// animation is finished, so return to what we were doing before
			if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
			else
			{
				setTopAnimation(ANIM_RUN_TOP);
				mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
			}
			mSwordsDrawn = !mSwordsDrawn;
		}
	}
	else if (mTopAnimID == ANIM_SLICE_VERTICAL || mTopAnimID == ANIM_SLICE_HORIZONTAL)
	{
		if (mTimer >= mAnims[mTopAnimID]->getLength())
		{
			// animation is finished, so return to what we were doing before
			if (mBaseAnimID == ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
			else
			{
				setTopAnimation(ANIM_RUN_TOP);
				mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
			}
		}

		// don't sway hips from side to side when slicing. that's just embarrasing.
		if (mBaseAnimID == ANIM_IDLE_BASE) baseAnimSpeed = 0;
	}
	else if (mBaseAnimID == ANIM_JUMP_START)
	{
		if (mTimer >= mAnims[mBaseAnimID]->getLength())
		{
			// takeoff animation finished, so time to leave the ground!
			setBaseAnimation(ANIM_JUMP_LOOP, true);
			// apply a jump acceleration to the character
			mVerticalVelocity = JUMP_ACCEL;
		}
	}
	else if (mBaseAnimID == ANIM_JUMP_END)
	{
		if (mTimer >= mAnims[mBaseAnimID]->getLength())
		{
			// safely landed, so go back to running or idling
			if (mKeyDirection == Vector3::ZERO)
			{
				setBaseAnimation(ANIM_IDLE_BASE);
				setTopAnimation(ANIM_IDLE_TOP);
			}
			else
			{
				setBaseAnimation(ANIM_RUN_BASE, true);
				setTopAnimation(ANIM_RUN_TOP, true);
			}
		}
	}

	// increment the current base and top animation times
	if (mBaseAnimID != ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	if (mTopAnimID != ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

	// apply smooth transitioning between our animations
	fadeAnimations(deltaTime);
}

void SinbadCharacterController::fadeAnimations(Real deltaTime)
{
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		if (mFadingIn[i])
		{
			// slowly fade this animation in until it has full weight
			Real newWeight = mAnims[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight >= 1) mFadingIn[i] = false;
		}
		else if (mFadingOut[i])
		{
			// slowly fade this animation out until it has no weight, and then disable it
			Real newWeight = mAnims[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if (newWeight <= 0)
			{
				mAnims[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}

void SinbadCharacterController::updateCamera(Real deltaTime)
{
	// place the camera pivot roughly at the character's shoulder
	mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3::UNIT_Y * CAM_HEIGHT);
	
	/*
	//! 将pivot对准角色的正前方，注意此时相机的+Z必须和角色的+Z相反，因为相机时从+Z看向-Z的
	//! 这样修改后，就完成了一个第一人称的相机，和魔兽世界类似
	//! W键始终让角色往自身正前方走，而不是相机的正前方
	Vector3 front = mCameraPivot->getOrientation().zAxis();
	Vector3 goal  = -mBodyNode->getOrientation().zAxis();
	Quaternion toGoal = front.getRotationTo(goal);
	Real yawToGoal = toGoal.getYaw().valueDegrees();
	mCameraPivot->yaw(Degree(yawToGoal) , Node::TS_WORLD );
	*/

	// move the camera smoothly to the goal
	Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();

	mCameraNode->translate(goalOffset * deltaTime * 1.0f);
	// always look at the pivot
	mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Node::TS_WORLD);
}

void SinbadCharacterController::updateCameraGoal(Real deltaYaw, Real deltaPitch, Real deltaZoom)
{
	mCameraPivot->yaw(Degree(deltaYaw*10), Node::TS_WORLD);

	// bound the pitch
	if (!(mPivotPitch + deltaPitch > 25 && deltaPitch > 0) &&
		!(mPivotPitch + deltaPitch < -60 && deltaPitch < 0))
	{
		mCameraPivot->pitch(Degree(deltaPitch), Node::TS_LOCAL);
		mPivotPitch += deltaPitch;
	}

	Real dist = mCameraGoal->_getDerivedPosition().distance(mCameraPivot->_getDerivedPosition());

	Real distChange = deltaZoom * dist;

	// bound the zoom
	if (!(dist + distChange < 8 && distChange < 0) &&
		!(dist + distChange > 25 && distChange > 0))
	{
		mCameraGoal->translate(0, 0, distChange, Node::TS_LOCAL);
	}
}

void SinbadCharacterController::setBaseAnimation(AnimID id, bool reset/* = false*/)
{
	if (mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}

	mBaseAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

void SinbadCharacterController::setTopAnimation(AnimID id, bool reset/* = false*/)
{
	if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}


bool SinbadCharacterController::IsPostion()
{
	const Vector3& vDestPos = GameMap::Instance()->GetLocPos(m_Location);
	const Vector3& curPos =  mBodyNode->getPosition();

	double distance = curPos.distance(vDestPos);

	return distance<10;

}

void SinbadCharacterController::move()
{
	//mAnims[ANIM_RUN_BASE]->setEnabled(true);
	//mAnims[ANIM_RUN_TOP]->setEnabled(true);

	//setBaseAnimation(ANIM_RUN_BASE);
	//setTopAnimation(ANIM_RUN_TOP);
	//setBaseAnimation(ANIM_RUN_BASE, true);
	//setTopAnimation(ANIM_RUN_TOP, true);




	if (!mFreeLook)
	{
		return;
	}
	const Vector3& vDestPos = GameMap::Instance()->GetLocPos(m_Location);
	const Vector3& curPos =  mBodyNode->getPosition();
	Vector3 direction = vDestPos - curPos;
	direction.normalise();
	direction.y = 0 ;


	const Quaternion& orientation = mBodyNode->getOrientation();
	Vector3 src =  orientation.zAxis();
	if ((1.0f + src.dotProduct(direction)) < 0.0001f) 
	{
		mBodyNode->yaw(Degree(180));
	}
	else
	{
		Ogre::Quaternion quat = src.getRotationTo(direction);
		Radian rad = quat.getYaw();
		mBodyNode->yaw(rad);
	}

	mBodyNode->translate(direction);


}


const Vector3& SinbadCharacterController::getKeyDirection() const
{
	return mKeyDirection;
}

const Vector3& SinbadCharacterController::getGoaldirection() const
{
	return mGoalDirection;
}