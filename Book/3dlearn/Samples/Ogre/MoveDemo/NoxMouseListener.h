#pragma once

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <OgreSceneQuery.h>

#include <SdkCameraMan.h>

#include "SelectionBox.h"

class NoxMouseListener :
	public OIS::MouseListener
{
public:
	NoxMouseListener(Ogre::SceneManager* sceneMgr , Ogre::Camera* camera , OgreBites::SdkCameraMan* cameraMan );
	virtual ~NoxMouseListener(void);
protected:
	virtual bool mouseMoved(const OIS::MouseEvent& arg);
	virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

private:
	void performSelection(const Ogre::Vector2& first, const Ogre::Vector2& second);
	void deselectObjects();
	void selectObject(Ogre::MovableObject* obj);
private:
	Ogre::Vector2 mStart, mStop;
	Ogre::PlaneBoundedVolumeListSceneQuery* mVolQuery;
	std::list<Ogre::MovableObject*> mSelected;
	bool bSelecting;
	//CEGUI::OgreRenderer* mGUIRenderer;
	SelectionBox* mSelectionBox;

	static void swap(float& x, float& y);

	Ogre::Camera* mCamera;
	OgreBites::SdkCameraMan* mCameraMan;       // basic camera controller

	Ogre::SceneManager* mSceneMgr;

	//! left mouse is down
	bool bRightDown;


};
