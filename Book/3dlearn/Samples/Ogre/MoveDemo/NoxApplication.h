#pragma once
//#include "application.h"
#include "./Aapplication.h"


#include <CEGUI.h>
#include <CEGUISchemeManager.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "MoveDemoListener.h"
#include "NoxWindowListener.h"
#include "NoxKeyboardListener.h"

class NoxApplication :	public Application
{
public:
	NoxApplication(void);

	~NoxApplication(void);

protected:
	Entity *mEntity;                // The entity of the object we are animating
	SceneNode *mRobotNode;               // The SceneNode of the object we are moving
	std::deque<Vector3> mWalkList;  // A deque containing the waypoints

	void createScene(void);

	void createListener(void);


	virtual bool configure(void);

	void createAxes();

	void createGrid();

	void createPath();


	CEGUI::OgreRenderer* mGUIRenderer;

	//Ogre::PlaneBoundedVolumeListSceneQuery* mVolQuery;

	NoxMouseListener* mMouseListener;

	
	NoxFrameListener* mFrameListener;

	NoxKeyboardListener* mKeyboardListener;


	NoxTrayListener* mTrayListener;

	NoxWindowListener* mWindowListener;

	OgreBites::SdkTrayManager* mTrayMgr;


	void createUI();
};
