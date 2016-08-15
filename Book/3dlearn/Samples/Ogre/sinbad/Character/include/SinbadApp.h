#ifndef __Character_H__
#define __Character_H__

#include "..\..\common\Framework.h"
#include "SinbadCharacterController.h"

using namespace Ogre;
using namespace XRENREN;


#define BUILDING_HEIGHT 5          // height of character's center of mass above ground


class _OgreSampleClassExport SinbadApp : public Framework
{
	SinbadCharacterController* mCharacter;
public:
	SinbadApp();
	virtual bool frameRenderingQueued(const FrameEvent& evt);
	virtual bool keyPressed(const OIS::KeyEvent& evt);
	virtual bool keyReleased(const OIS::KeyEvent& evt);

	virtual bool mouseMoved(const OIS::MouseEvent& evt);
	virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

protected:

	void setupContent();
	void cleanupContent();

	void createAxes();
	void createGrid();

	void createPath();

	void setupBuilding(SceneManager* sceneMgr);


	ParamsPanel* mCharInfoPanel;
	void setupCharInfoPanel();

	ParamsPanel* mDebugInfoPanel;
	void setupDebugInfoPanel();



	XRENREN::Label* mLab1;
	XRENREN::Label* mLab2;
	XRENREN::Label* mLab3;
	XRENREN::Label* mLab4;


	XRENREN::TextBox* mTextBox;
	XRENREN::Widget* mWidget;
	XRENREN::Button* mMutton;



	void setupOverlay();

	virtual void buttonHit(Button* button);


	//Ogre::Overlay * overlayCatainer;
	//Ogre::OverlayElement* overEle;

};

#endif
