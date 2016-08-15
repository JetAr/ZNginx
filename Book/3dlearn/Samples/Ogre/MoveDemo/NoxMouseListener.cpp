#include "stdafx.h"
#include "NoxMouseListener.h"



NoxMouseListener::NoxMouseListener(Ogre::SceneManager* sceneMgr , Ogre::Camera* camera , OgreBites::SdkCameraMan* cameraMan)
 : mCamera(camera)
 , mCameraMan(cameraMan)
 , mSceneMgr(sceneMgr)
 , mVolQuery(0)
 , mSelectionBox(0)
 , bRightDown(false)
{

	mSelectionBox = new SelectionBox("SelectionBox");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mSelectionBox);

	mVolQuery = mSceneMgr->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());

}

NoxMouseListener::~NoxMouseListener(void)
{
}


bool NoxMouseListener::mouseMoved(const OIS::MouseEvent& arg)
{

	if (bRightDown)
	{
		mCameraMan->injectMouseMove(arg);

		return true;
	}

	//let CEGUI know that the mouse moved
	CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);

	CEGUI::MouseCursor* mouse = CEGUI::MouseCursor::getSingletonPtr();
	mStop.x = mouse->getPosition().d_x / float(arg.state.width);
	mStop.y = mouse->getPosition().d_y / float(arg.state.height);

	mSelectionBox->setCorners(mStart, mStop);


	return true;
}

bool NoxMouseListener::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{

	//bRightDown = arg.
	if (id == OIS::MB_Left)
	{
		CEGUI::MouseCursor* mouse = CEGUI::MouseCursor::getSingletonPtr();
		mStart.x = mouse->getPosition().d_x / float(arg.state.width);
		mStart.y = mouse->getPosition().d_y / float(arg.state.height);
		mStop = mStart;

		bSelecting = true;
		mSelectionBox->clear();
		mSelectionBox->setVisible(true);

		bRightDown = false ;
	}
	else if (id == OIS::MB_Right)
	{
		bRightDown = true ;
	}

	return true;

}

bool NoxMouseListener::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{	
	performSelection(mStart, mStop);
	bSelecting = false;
	mSelectionBox->setVisible(false);

	bRightDown = false ;

	return true;
}


void NoxMouseListener::performSelection(const Ogre::Vector2& first, const Ogre::Vector2& second)
{
	float left = first.x, right = second.x;
	float top = first.y, bottom = second.y;

	if(left > right)
	{
		swap(left, right);
	}
	if(top > bottom)
	{
		swap(top, bottom);
	}

	if((right - left) * (bottom - top) < 0.0001)
	{
		return;
	}

	Ogre::Ray topLeft = mCamera->getCameraToViewportRay(left, top);
	Ogre::Ray topRight = mCamera->getCameraToViewportRay(right, top); 
	Ogre::Ray bottomLeft = mCamera->getCameraToViewportRay(left, bottom);
	Ogre::Ray bottomRight = mCamera->getCameraToViewportRay(right, bottom);

	Ogre::PlaneBoundedVolume vol;

	vol.planes.push_back(Ogre::Plane(topLeft.getPoint(3), topRight.getPoint(3), bottomRight.getPoint(3)));
	vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(), topLeft.getPoint(100), topRight.getPoint(100)));
	vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(), bottomLeft.getPoint(100), topLeft.getPoint(100)));
	vol.planes.push_back(Ogre::Plane(bottomLeft.getOrigin(), bottomRight.getPoint(100), bottomLeft.getPoint(100)));
	vol.planes.push_back(Ogre::Plane(topRight.getOrigin(), topRight.getPoint(100), bottomRight.getPoint(100)));

	Ogre::PlaneBoundedVolumeList volList;
	volList.push_back(vol);
	mVolQuery->setVolumes(volList);

	Ogre::SceneQueryResult& result = mVolQuery->execute();
	Ogre::SceneQueryResultMovableList::iterator iter = result.movables.begin();
	deselectObjects();

	for(iter; iter != result.movables.end(); iter++)
	{
		selectObject(*iter);
	}

}

void NoxMouseListener::deselectObjects()
{
	std::list<Ogre::MovableObject*>::iterator iter = mSelected.begin();

	for(iter; iter != mSelected.end(); iter++)
	{
		(*iter)->getParentSceneNode()->showBoundingBox(false);
	}
}

void NoxMouseListener::selectObject(Ogre::MovableObject* obj)
{
	obj->getParentSceneNode()->showBoundingBox(true);
	mSelected.push_back(obj);
}

//Simple function that just swaps float values
void NoxMouseListener::swap(float& x, float& y)
{
	float temp = x;
	x = y;
	y = temp;
}