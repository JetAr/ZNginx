#pragma once
//#include "application.h"
#include "./Aapplication.h"

#include "MoveDemoListener.h"

class MoveDemoApplication :	public ExampleApplication
{
public:
	MoveDemoApplication(void);

	~MoveDemoApplication(void);

protected:
	Entity *mEntity;                // The entity of the object we are animating
	SceneNode *mNode;               // The SceneNode of the object we are moving
	std::deque<Vector3> mWalkList;  // A deque containing the waypoints

	void createScene(void);

	void createFrameListener(void);


	virtual bool configure(void)
	{
		// Show the configuration dialog and initialise the system
		// You can skip this and use root.restoreConfig() to load configuration
		// settings if you were sure there are valid ones saved in ogre.cfg
		if(mRoot->restoreConfig() || mRoot->showConfigDialog())
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


	void createAxes();

	void createGrid();

	void createPath();

};
