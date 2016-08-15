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
Filename:    ExampleFrameListener.h
Description: Defines an example frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Mouse:    Freelook
W or Up:  Forward
S or Down:Backward
A:        Step left
D:        Step right
PgUp:     Move upwards
PgDown:   Move downwards
F:        Toggle frame rate stats on/off
R:        Render mode
T:        Cycle texture filtering
Bilinear, Trilinear, Anisotropic(8)
P:        Toggle on/off display of camera position / orientation
F2:	   Set the main viewport material scheme to default material manager scheme.
F3:	   Set the main viewport material scheme to shader generator default scheme.
F4:	   Toggle default shader generator lighting model from per vertex to per pixel.
-----------------------------------------------------------------------------
*/

#pragma once


#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"



//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>
#include "NoxTrayListener.h"
#include <SdkCameraMan.h>

using namespace Ogre;

#ifdef USE_RTSHADER_SYSTEM
#include "OgreRTShaderSystem.h"
#endif

class NoxFrameListener: public FrameListener, public WindowEventListener
{

public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	NoxFrameListener(RenderWindow* win, Camera* cam, OgreBites::SdkCameraMan* cameraMan , OgreBites::SdkTrayManager* trayMgr
		,OIS::Mouse*    mouse,	OIS::Keyboard* keyboard ,  bool bufferedKeys = false
		, bool bufferedMouse = false,	bool bufferedJoy = false ) 
		: mCamera(cam), mWindow(win)
		, mStatsOn(true), mNumScreenShots(0), mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0)
		, mFiltering(TFO_BILINEAR),	mAniso(1), mSceneDetailIndex(0), mMoveSpeed(200), mRotateSpeed(36)
		, mDebugOverlay(0),	mInputManager(0), mMouse(mouse), mKeyboard(keyboard)
		, mTrayMgr(trayMgr), mDetailsPanel(0), mMsgPanel(0)
		, mCameraMan(cameraMan)
	{
		mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");


		showDebugOverlay(true);

		// create a params panel for displaying sample details
		Ogre::StringVector items;
		items.push_back("cam.pX");
		items.push_back("cam.pY");
		items.push_back("cam.pZ");
		items.push_back("");
		items.push_back("cam.oW");
		items.push_back("cam.oX");
		items.push_back("cam.oY");
		items.push_back("cam.oZ");
		items.push_back("");
		items.push_back("Filtering");
		items.push_back("Poly Mode");

		mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 150, items);
		mDetailsPanel->setParamValue(9, "Bilinear");
		mDetailsPanel->setParamValue(10, "Solid");
		//mDetailsPanel->hide();

		//!
		Ogre::StringVector items_msg;
		items_msg.push_back("x");
		items_msg.push_back("y");
		items_msg.push_back("z");
		items_msg.push_back("num");
		items_msg.push_back("mouse_x");//4
		items_msg.push_back("mouse_y");//5

		mMsgPanel = mTrayMgr->createParamsPanel(OgreBites::TL_BOTTOMRIGHT, "MessagePanel", 120, items_msg);
		//mMsgPanel->setParamValue(0, "hello");
	}


	virtual ~NoxFrameListener()
	{		
		if (mTrayMgr)
			delete mTrayMgr;

		//Remove ourself as a Window listener
		WindowEventUtilities::removeWindowEventListener(mWindow, this);
		windowClosed(mWindow);
	}

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
	{
		//		Real moveScale = mMoveScale;
		//		if(mKeyboard->isKeyDown(OIS::KC_LSHIFT))
		//			moveScale *= 10;
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_A))
		//			mTranslateVector.x = -moveScale;	// Move camera left
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_D))
		//			mTranslateVector.x = moveScale;	// Move camera RIGHT
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) )
		//			mTranslateVector.z = -moveScale;	// Move camera forward
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
		//			mTranslateVector.z = moveScale;	// Move camera backward
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_PGUP))
		//			mTranslateVector.y = moveScale;	// Move camera up
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_PGDOWN))
		//			mTranslateVector.y = -moveScale;	// Move camera down
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_RIGHT))
		//			mCamera->yaw(-mRotScale);
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_LEFT))
		//			mCamera->yaw(mRotScale);
		//
		//		if( mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q) )
		//			return false;
		//
		//		if( mKeyboard->isKeyDown(OIS::KC_F) && mTimeUntilNextToggle <= 0 )
		//		{
		//			mStatsOn = !mStatsOn;
		//			showDebugOverlay(mStatsOn);
		//			mTimeUntilNextToggle = 1;
		//		}
		//
		//		if( mKeyboard->isKeyDown(OIS::KC_T) && mTimeUntilNextToggle <= 0 )
		//		{
		//			switch(mFiltering)
		//			{
		//			case TFO_BILINEAR:
		//				mFiltering = TFO_TRILINEAR;
		//				mAniso = 1;
		//				break;
		//			case TFO_TRILINEAR:
		//				mFiltering = TFO_ANISOTROPIC;
		//				mAniso = 8;
		//				break;
		//			case TFO_ANISOTROPIC:
		//				mFiltering = TFO_BILINEAR;
		//				mAniso = 1;
		//				break;
		//			default: break;
		//			}
		//			MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
		//			MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);
		//
		//			showDebugOverlay(mStatsOn);
		//			mTimeUntilNextToggle = 1;
		//		}
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_SYSRQ) && mTimeUntilNextToggle <= 0)
		//		{
		//			std::ostringstream ss;
		//			ss << "screenshot_" << ++mNumScreenShots << ".png";
		//			mWindow->writeContentsToFile(ss.str());
		//			mTimeUntilNextToggle = 0.5;
		//			mDebugText = "Saved: " + ss.str();
		//		}
		//
		//		if(mKeyboard->isKeyDown(OIS::KC_R) && mTimeUntilNextToggle <=0)
		//		{
		//			mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		//			switch(mSceneDetailIndex) {
		//case 0 : mCamera->setPolygonMode(PM_SOLID); break;
		//case 1 : mCamera->setPolygonMode(PM_WIREFRAME); break;
		//case 2 : mCamera->setPolygonMode(PM_POINTS); break;
		//			}
		//			mTimeUntilNextToggle = 0.5;
		//		}
		//
		//		static bool displayCameraDetails = false;
		//		if(mKeyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0)
		//		{
		//			displayCameraDetails = !displayCameraDetails;
		//			mTimeUntilNextToggle = 0.5;
		//			if (!displayCameraDetails)
		//				mDebugText = "";
		//		}
		//
		//		// Print camera details
		//		if(displayCameraDetails)
		//			mDebugText = "P: " + StringConverter::toString(mCamera->getDerivedPosition()) +
		//			" " + "O: " + StringConverter::toString(mCamera->getDerivedOrientation());

		// Return true to continue rendering
		return true;
	}

	virtual bool processUnbufferedMouseInput(const FrameEvent& evt)
	{

		//// Rotation factors, may not be used if the second mouse button is pressed
		//// 2nd mouse button - slide, otherwise rotate
		//const OIS::MouseState &ms = mMouse->getMouseState();
		//if (ms.buttonDown(OIS::MB_Left))
		//{

		//}
		//else if( ms.buttonDown( OIS::MB_Right ) )
		//{
		//	mTranslateVector.x += ms.X.rel * 0.13;
		//	mTranslateVector.y -= ms.Y.rel * 0.13;
		//}
		//else
		//{
		//	mRotX = Degree(-ms.X.rel * 0.13);
		//	mRotY = Degree(-ms.Y.rel * 0.13);
		//}

		return true;
	}

	virtual void moveCamera()
	{
		// Make all the changes to the camera
		// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
		//(e.g. airplane)
		//mCamera->yaw(mRotX);
		//mCamera->pitch(mRotY);
		//mCamera->moveRelative(mTranslateVector);
	}

	virtual void showDebugOverlay(bool show)
	{
		if (mDebugOverlay)
		{
			if (show)
				mDebugOverlay->show();
			else
				mDebugOverlay->hide();
		}
	}

	// Override frameRenderingQueued event to process that (don't care about frameEnded)
	bool frameRenderingQueued(const FrameEvent& evt)
	{
		if(mWindow->isClosed())	return false;

		mSpeedLimit = mMoveScale * evt.timeSinceLastFrame;

		//Need to capture/update each device
		mKeyboard->capture();
		mMouse->capture();

		mTrayMgr->frameRenderingQueued(evt);

		if (!mTrayMgr->isDialogVisible())
		{
			mCameraMan->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera
			if (mDetailsPanel->isVisible())   // if details panel is visible, then update its contents
			{
				mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
				mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
				mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
				mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().w));
				mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().x));
				mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().y));
				mDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().z));
			}
		}

		return true;
	}

	bool frameEnded(const FrameEvent& evt)
	{
		updateStats();
		return true;
	}

protected:
	virtual void updateStats(void)
	{
		static String currFps = "Current FPS: ";
		static String avgFps = "Average FPS: ";
		static String bestFps = "Best FPS: ";
		static String worstFps = "Worst FPS: ";
		static String tris = "Triangle Count: ";
		static String batches = "Batch Count: ";

		// update stats when necessary
		try {
			OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
			OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
			OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
			OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

			const RenderTarget::FrameStats& stats = mWindow->getStatistics();
			guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
			guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
			guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
				+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
			guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
				+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

			OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
			guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

			OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
			guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

			OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
			guiDbg->setCaption(mDebugText);
		}
		catch(...) { /* ignore */ }
	}

protected:
	Camera* mCamera;
	OgreBites::SdkCameraMan* mCameraMan;
	RenderWindow* mWindow;
	bool mStatsOn;

	String mDebugText;

	unsigned int mNumScreenShots;
	float mMoveScale;
	float mSpeedLimit;
	Degree mRotScale;
	// just to stop toggles flipping too fast
	Real mTimeUntilNextToggle ;
	Radian mRotX, mRotY;
	TextureFilterOptions mFiltering;
	int mAniso;

	int mSceneDetailIndex ;
	Real mMoveSpeed;
	Degree mRotateSpeed;
	Overlay* mDebugOverlay;



	OgreBites::SdkTrayManager* mTrayMgr;
	OgreBites::ParamsPanel* mDetailsPanel;     // sample details panel
	OgreBites::ParamsPanel* mMsgPanel;


	//NoxTrayListener* mTrayListener;


	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;
	OIS::InputManager* mInputManager;


};

