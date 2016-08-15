#include <Common\xSamplePlugin.h>
#include "sinbadapp.h"
#include <strstream>


using namespace std;

using namespace Ogre;
using namespace XRENREN;


#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new SinbadApp;
	sp = OGRE_NEW SamplePlugin(s->getInfo()["Title"] + " Sample");
	sp->addSample(s);
	Root::getSingleton().installPlugin(sp);
}

extern "C" _OgreSampleExport void dllStopPlugin()
{
	Root::getSingleton().uninstallPlugin(sp); 
	OGRE_DELETE sp;
	delete s;
}

#endif


SinbadApp::SinbadApp()
{
	mInfo["Title"] = "Character";
	mInfo["Description"] = "A demo showing 3rd-person character control and use of TagPoints.";
	mInfo["Thumbnail"] = "thumb_char.png";
	mInfo["Category"] = "Animation";
	mInfo["Help"] = "Use the WASD keys to move Sinbad, and the space bar to jump. "
		"Use mouse to look around and mouse wheel to zoom. Press Q to take out or put back "
		"Sinbad's swords. With the swords equipped, you can left click to slice vertically or "
		"right click to slice horizontally. When the swords are not equipped, press E to "
		"start/stop a silly dance routine.";
}


bool SinbadApp::frameRenderingQueued(const FrameEvent& evt)
{
	// let character update animations and camera
	mCharacter->UpdateState(evt.timeSinceLastFrame);
	mCharacter->addTime(evt.timeSinceLastFrame);

	if (mCharInfoPanel->isVisible())
	{

		//! current postion
		SceneNode* node = mSceneMgr->getSceneNode("SinbadBody");
		const Vector3& curPos =  node->getPosition();

		location_type loc =  mCharacter->Location();
		const Vector3& vDestPos = GameMap::Instance()->GetLocPos(loc);

		Real distance = curPos.distance(vDestPos);

	
		mCharInfoPanel->setParamValue(0, Ogre::StringConverter::toString( evt.timeSinceLastFrame ) );
		mCharInfoPanel->setParamValue(1, Ogre::StringConverter::toString( mCharacter->m_iGoldCarried ) );
		mCharInfoPanel->setParamValue(2, Ogre::StringConverter::toString( mCharacter->m_iMoneyInBank ) );
		mCharInfoPanel->setParamValue(3, Ogre::StringConverter::toString( mCharacter->m_iThirst ) );
		mCharInfoPanel->setParamValue(4, Ogre::StringConverter::toString( mCharacter->m_iFatigue ) );
		//! current postion
		mCharInfoPanel->setParamValue(5, Ogre::StringConverter::toString( Ogre::Math::ICeil(curPos.x-0.5) ));
		mCharInfoPanel->setParamValue(6, Ogre::StringConverter::toString( Ogre::Math::ICeil(curPos.y-0.5) ));
		mCharInfoPanel->setParamValue(7, Ogre::StringConverter::toString( Ogre::Math::ICeil(curPos.z-0.5) ));
		//! destination
		mCharInfoPanel->setParamValue(8, Ogre::StringConverter::toString( Ogre::Math::ICeil(vDestPos.x-0.5) ));
		mCharInfoPanel->setParamValue(9, Ogre::StringConverter::toString( Ogre::Math::ICeil(vDestPos.y-0.5) ));
		mCharInfoPanel->setParamValue(10,Ogre::StringConverter::toString( Ogre::Math::ICeil(vDestPos.z-0.5) ));
		//! distance
		Ogre::String str = Ogre::StringConverter::toString( (int)loc);
		str+=":";
		str+=Ogre::StringConverter::toString( Ogre::Math::ICeil(distance-0.5) );
		mCharInfoPanel->setParamValue(11,   str);

		//!
		const Vector3& keyDirection  = mCharacter->getKeyDirection();
		const Vector3& goalDirection  =  mCharacter->getGoaldirection();

		Ogre::String debuginfo;
		debuginfo+="key:[";
		debuginfo+=Ogre::StringConverter::toString( keyDirection.x );
		debuginfo+=",";
		debuginfo+=Ogre::StringConverter::toString( keyDirection.y );
		debuginfo+=",";
		debuginfo+=Ogre::StringConverter::toString( keyDirection.z );

		debuginfo+="] goal:[";
		debuginfo+=Ogre::StringConverter::toString( goalDirection.x );
		debuginfo+=",";
		debuginfo+=Ogre::StringConverter::toString( goalDirection.y );
		debuginfo+=",";
		debuginfo+=Ogre::StringConverter::toString( goalDirection.z );
		debuginfo+="]";
		mDebugInfoPanel->setParamValue(0 , debuginfo.c_str() );



		//! 相机最后变换
		//Ogre::StringStream oss;
		//oss<<"key:"<<setprecision(6)<<keyDirection.x<<" "<<keyDirection.y<<" "<<keyDirection.z<<" ";

		//Vector3 goalOffset = mCharacter->mCameraGoal->_getDerivedPosition() - mCharacter->mCameraNode->getPosition();
		//Ogre::StringStream ssGoal;
		//ssGoal<<"goal:"<<(int)goalOffset.x<<" "<<(int)goalOffset.y<<" "<<(int)goalOffset.z;
		//mDebugInfoPanel->setParamValue(0 , ssGoal.str() );

		//Ogre::StringUtil::trim(str);
		//Ogre::StringStream ssss;
		//ssss<<"";
		//! character infomation
		{
			//SceneNode* mBodyNode;
			//SceneNode* mCameraPivot;
			//SceneNode* mCameraGoal;
			//SceneNode* mCameraNode;
			Vector3 v1 ,v2;
			StringStream ss;

			v1 = mCharacter->mBodyNode->getPosition();
			v2 = mCharacter->mBodyNode->_getDerivedPosition();
			ss<<"mBodyNode:["<<(int)v1.x<<","<<(int)v1.y<<","<<(int)v1.z<<"] derived";
			ss<<"["<<(int)v2.x<<","<<(int)v2.y<<","<<(int)v2.z<<"]";
			mLab1->setCaption(ss.str());
			ss.str("");

			v1 = mCharacter->mCameraPivot->getPosition();
			v2 = mCharacter->mCameraPivot->_getDerivedPosition();
			ss<<"mCameraPivot:["<<(int)v1.x<<","<<(int)v1.y<<","<<(int)v1.z<<"] derived";
			ss<<"["<<(int)v2.x<<","<<(int)v2.y<<","<<(int)v2.z<<"]";
			mLab2->setCaption(ss.str());
			ss.str("");

			v1 = mCharacter->mCameraGoal->getPosition();
			v2 = mCharacter->mCameraGoal->_getDerivedPosition();
			ss<<"mCameraGoal:["<<(int)v1.x<<","<<(int)v1.y<<","<<(int)v1.z<<"] derived";
			ss<<"["<<(int)v2.x<<","<<(int)v2.y<<","<<(int)v2.z<<"]";
			mLab3->setCaption(ss.str());
			ss.str("");

			v1 = mCharacter->mCameraNode->getPosition();
			v2 = mCharacter->mCameraNode->_getDerivedPosition();
			ss<<"mCameraNode:["<<(int)v1.x<<","<<(int)v1.y<<","<<(int)v1.z<<"] derived";
			ss<<"["<<(int)v2.x<<","<<(int)v2.y<<","<<(int)v2.z<<"]";
			mLab4->setCaption(ss.str());
			ss.str("");
		}
		
	}


	return Framework::frameRenderingQueued(evt);
}



bool SinbadApp::keyPressed(const OIS::KeyEvent& evt)
{
	// relay input events to character controller
	if (!mTrayMgr->isDialogVisible()) mCharacter->injectKeyDown(evt);
	return Framework::keyPressed(evt);
}

bool SinbadApp::keyReleased(const OIS::KeyEvent& evt)
{
	// relay input events to character controller
	if (!mTrayMgr->isDialogVisible()) mCharacter->injectKeyUp(evt);
	return Framework::keyReleased(evt);
}

bool SinbadApp::mouseMoved(const OIS::MouseEvent& evt)
{
	//StringStream ss;
	//ss<<evt.state.X.abs<<" ";
	//ss<<evt.state.Y.abs<<" ";
	//ss<<evt.state.Z.abs;
	//mLab1->setCaption(ss.str());

	// relay input events to character controller
	if (!mTrayMgr->isDialogVisible())
		mCharacter->injectMouseMove(evt);
	return Framework::mouseMoved(evt);
}

bool SinbadApp::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	// relay input events to character controller
	if (!mTrayMgr->isDialogVisible()) mCharacter->injectMouseDown(evt, id);
	return Framework::mousePressed(evt, id);
}


bool SinbadApp::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	// relay input events to character controller
	//if (!mTrayMgr->isDialogVisible()) mChara->injectMouseReleased(evt, id);

	if (mTrayMgr->injectMouseUp(evt , id)) return true;

	return Framework::mousePressed(evt, id);
}

void SinbadApp::buttonHit(Button* button)
{
	Ogre::String strCaption = button->getCaption();
	Ogre::String strName = button->getName();

	if (strCaption == "test" && strName=="button_test")
	{
		//exit
	}



	int i = 0 ;

}


void SinbadApp::cleanupContent()
{
	// clean up character controller and the floor mesh
	if (mCharacter) delete mCharacter;
	MeshManager::getSingleton().remove("floor");
}

void SinbadApp::createPath()
{
	Ogre::ManualObject* pManualObject = mSceneMgr-> createManualObject( "PathObj"); 


	if (pManualObject)
	{
		pManualObject-> begin( "",   Ogre::RenderOperation::OT_LINE_LIST); 
		pManualObject-> colour(Ogre::ColourValue(1.0 , 1.0 , 0 )); 

		pManualObject-> position(Vector3( 0.0f, 1.0f, 25.0f )); 
		pManualObject-> position(Vector3( 110.0f,  1.0f,  50.0f  )); 

		pManualObject-> position(Vector3( 110.0f,  1.0f,  50.0f  )); 
		pManualObject-> position(Vector3(-50.0f,  1.0f, -100.0f ) ); 

		pManualObject-> position(Vector3(-50.0f,  1.0f, -100.0f ) ); 
		pManualObject-> position(Vector3( 0.0f, 1.0f, 25.0f )); 

		pManualObject-> end(); 

		pManualObject->getSection(0)->getTechnique()->getPass(0)->setLightingEnabled(false);

		Ogre::SceneNode*pSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "PathNode "); 
		if(pSceneNode) 
			pSceneNode-> attachObject(pManualObject);

	}
}


void SinbadApp::createGrid()
{

	Ogre::ManualObject*   pManualObject   =   mSceneMgr-> createManualObject( "TestManualObject "); 
	if   (NULL   ==   pManualObject) 
		return   ; 

	pManualObject-> begin( " ",   Ogre::RenderOperation::OT_LINE_LIST); 
	{ 
		int startX=-100 ; 
		int startZ = -100;
		int endX=100 ; 
		int endZ = 100;
		int step = 10;
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

	pSceneNode->translate(0 , 0.5 , 0 );


}


void SinbadApp::createAxes()
{

	Ogre::ManualObject* pManualObject = mSceneMgr-> createManualObject( "ManualGridObj "); 
	if (pManualObject)
	{
		int nLength = 160*10 ;
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

		pSceneNode->translate(0 , 1 , 0 );


		//dragon.mesh
		Ogre::Entity* entAxes = mSceneMgr->createEntity("axes.mesh");
		Ogre::SceneNode * nodeAxes = mSceneMgr->getRootSceneNode()->createChildSceneNode("axes");
		nodeAxes->attachObject(entAxes);


	}

}


void SinbadApp::setupContent()
{   
	// set background and some fog
	mViewport->setBackgroundColour(ColourValue(0.3f, 0.3f, 0.8f));
	//mSceneMgr->setFog(Ogre::FOG_LINEAR, ColourValue(1.0f, 1.0f, 0.8f), 0, 15, 100);

	// set shadow properties
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowTextureSize(1024);
	mSceneMgr->setShadowTextureCount(1);

	// disable default camera control so the character can do its own
	mCameraMan->setStyle(CS_MANUAL);

	// use a small amount of ambient lighting
	//mSceneMgr->setAmbientLight(ColourValue(0.6, 0.6, 0.6));

	// add a bright light above the scene
	Light* light = mSceneMgr->createLight();
	light->setType(Light::LT_POINT);
	light->setPosition(-10, 80, 20);
	light->setSpecularColour(ColourValue::White);

	MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Plane(Vector3::UNIT_Y, 0), 200, 200, 10, 10, true, 1, 10, 10, Vector3::UNIT_Z);

	// create a floor entity, give it a material, and place it at the origin
	Entity* floor = mSceneMgr->createEntity("Floor", "floor");
	floor->setMaterialName("Examples/Rockwall");
	floor->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(floor);

	// create our character controller
	mCharacter = new SinbadCharacterController(mCamera);

	mTrayMgr->toggleAdvancedFrameStats();

	//StringVector items;
	//items.push_back("Help");
	//ParamsPanel* help = mTrayMgr->createParamsPanel(TL_TOPLEFT, "HelpMessage", 100, items);
	//help->setParamValue("Help", "H / F1");

	//! debug panel
	setupDebugInfoPanel();

	//! 角色信息
	setupCharInfoPanel();


	createAxes();
	//createGrid();
	//createPath();

	//! 

	//setupBuilding(mSceneMgr);


	setupOverlay();

}


void SinbadApp::setupOverlay()
{


	mLab1 = mTrayMgr->createLabel(XRENREN::TL_TOPLEFT , "lable1" , "caption:lab1" , 300);
	mLab2 = mTrayMgr->createLabel(XRENREN::TL_TOPLEFT , "lable2" , "caption:lab2" , 300);
	mLab3 = mTrayMgr->createLabel(XRENREN::TL_TOPLEFT , "lable3" , "caption:lab3" , 300);
	mLab4 = mTrayMgr->createLabel(XRENREN::TL_TOPLEFT , "lable4" , "caption:lab4" , 300);


	//mLab2->show();
	//mLab1->show();

	mTextBox = mTrayMgr->createTextBox(TL_RIGHT , "textbox" , "test___" , 150 , 100);
	mTextBox->setText("HELLO WORLD1!");
	mWidget  = mTrayMgr->createDecorWidget(TL_RIGHT , "pic2" , "Xrenren/Logo" );

	mMutton = mTrayMgr->createButton(TL_RIGHT , "button_test" , "test");


	//overlayCatainer = Ogre::OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	//overlayCatainer->show();  
	//overEle = Ogre::OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
	//overEle->setCaption("e ewfwefwef");



}


void SinbadApp::setupBuilding(SceneManager* sceneMgr)
{
	SceneNode * pBuildingNode;
	Entity* pBuildingEnt;
	MovableText* msg = NULL;
	Vector3 size3D;
	const Real buildingHeight = 2 ;
	//pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y /** BUILDING_HEIGHT*/);
	//pBuildingEnt = sceneMgr->createEntity("axis", "axis.mesh");
	//pBuildingNode->attachObject(pBuildingEnt);
	//pBuildingNode->translate(0 , 0 , 0);
	//! 电子牌
	//shack,
	//goldmine,
	//bank,
	//saloon


	//!木屋
	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y /** BUILDING_HEIGHT*/);
	pBuildingEnt = sceneMgr->createEntity("highlanderhouse01", "highlanderhouse.01.mesh");
	pBuildingNode->attachObject(pBuildingEnt);
	pBuildingNode->translate(-50 , 0 , -50);

	msg = new MovableText("shack", "shack");
	msg->setTextAlignment(MovableText::H_CENTER, MovableText::V_CENTER); // Center horizontally and display above the node
	msg->setColor(ColourValue::Black);
	size3D = pBuildingEnt->getBoundingBox().getSize();
	msg->setLocalTranslation(Vector3(0 , size3D.y/2 , 0));
	pBuildingNode->attachObject(msg);
	msg->setCharacterHeight(buildingHeight);

	//! 金矿
	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y /** BUILDING_HEIGHT*/);
	pBuildingEnt = sceneMgr->createEntity("rock05", "rock.05.mesh");
	pBuildingNode->attachObject(pBuildingEnt);
	pBuildingNode->translate(50 , 0 , -50);

	msg = new MovableText("goldmine", "goldmine");
	msg->setTextAlignment(MovableText::H_CENTER, MovableText::V_CENTER); // Center horizontally and display above the node
	msg->setColor(ColourValue::Black);
	size3D = pBuildingEnt->getBoundingBox().getSize();
	msg->setLocalTranslation(Vector3(0 , size3D.y/2 , 0));
	pBuildingNode->attachObject(msg);
	msg->setCharacterHeight(buildingHeight);

	//! 酒吧
	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y);
	pBuildingEnt = sceneMgr->createEntity("tudorhouse02", "highlanderhouse.01.mesh");
	pBuildingNode->attachObject(pBuildingEnt);
	pBuildingNode->scale(5 , 2.5 , 2.5);
	pBuildingNode->translate(50 , 0 , 50);

	msg = new MovableText("saloon", "saloon");
	msg->setTextAlignment(MovableText::H_CENTER, MovableText::V_CENTER); // Center horizontally and display above the node
	msg->setColor(ColourValue::Black);
	size3D = pBuildingEnt->getBoundingBox().getSize();
	msg->setLocalTranslation(Vector3(0 , size3D.y/2 , 0));
	pBuildingNode->attachObject(msg);
	msg->setCharacterHeight(buildingHeight);


	//! 银行
	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y /** BUILDING_HEIGHT*/);
	pBuildingEnt = sceneMgr->createEntity("sibenik", "sibenik.mesh");
	pBuildingNode->attachObject(pBuildingEnt);
	pBuildingNode->scale(0.5 , 0.5 , 0.5);
	pBuildingNode->translate(-50 , 0 , 50);

	msg = new MovableText("bank", "bank");
	msg->setTextAlignment(MovableText::H_CENTER, MovableText::V_CENTER); // Center horizontally and display above the node
	msg->setColor(ColourValue::Black);
	size3D = pBuildingEnt->getBoundingBox().getSize();
	msg->setLocalTranslation(Vector3(0 , size3D.y/2 , 0));
	pBuildingNode->attachObject(msg);
	msg->setCharacterHeight(buildingHeight);




	//skeltorch.mesh
	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y /** BUILDING_HEIGHT*/);
	pBuildingEnt = sceneMgr->createEntity("skeltorch", "skeltorch.mesh");
	pBuildingNode->attachObject(pBuildingEnt);
	pBuildingNode->scale(5 , 5 , 5);
	//pBuildingNode->translate(-50 , 0 , 50);


	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y /** BUILDING_HEIGHT*/);
	pBuildingEnt = sceneMgr->createEntity("tree.05", "tree.05.mesh");
	pBuildingNode->attachObject(pBuildingEnt);
	pBuildingNode->scale(5 , 5 , 5);
	pBuildingNode->translate(-50 , 0 , 30);


	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y /** BUILDING_HEIGHT*/);
	pBuildingEnt = sceneMgr->createEntity("tree.07", "tree.07.mesh");
	pBuildingNode->attachObject(pBuildingEnt);
	pBuildingNode->scale(5 , 5 , 5);
	pBuildingNode->translate(-40 , 0 , -40);

	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y /** BUILDING_HEIGHT*/);
	pBuildingEnt = sceneMgr->createEntity("tree.09", "tree.09.mesh");
	pBuildingNode->attachObject(pBuildingEnt);
	pBuildingNode->scale(5 , 5 , 5);
	pBuildingNode->translate(60 , 0 , 20);


	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y /** BUILDING_HEIGHT*/);
	pBuildingEnt = sceneMgr->createEntity("ogrehead", "ogrehead.mesh");
	pBuildingNode->attachObject(pBuildingEnt);
	pBuildingNode->scale(0.5 , .5 , .5);
	pBuildingNode->translate(0 , 10 , 80);
	Ogre::Quaternion q(Degree(180) ,Vector3(0,1,0) );
	pBuildingNode->rotate(q);


	//! sword
	pBuildingNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * 20);
	pBuildingEnt = sceneMgr->createEntity("Sword", "Sword.mesh");
	pBuildingNode->attachObject(pBuildingEnt);

	pBuildingNode->pitch(Degree(90) );
	pBuildingNode->scale(3 , 3 , 3);
	pBuildingNode->translate(0 , 0 , 10);
	//Ogre::Quaternion q(Degree(180) ,Vector3(0,1,0) );
	//pBuildingNode->rotate(q);


}


void SinbadApp::setupCharInfoPanel()
{
	// create a params panel for displaying sample details
	//inf<<"miner ";
	//inf<<"gold:"<<m_iGoldCarried<<" ";
	//inf<<"money:"<<m_iMoneyInBank<<" ";
	//inf<<"thirst:"<<m_iThirst<<" ";
	//inf<<"fatigue:"<<m_iFatigue<<" ";

	Ogre::StringVector items;
	items.push_back("miner");
	items.push_back("gold");
	items.push_back("money");
	items.push_back("thirst");
	items.push_back("fatigue");
	//! current postion
	items.push_back("cx");
	items.push_back("cy");
	items.push_back("cz");
	//! destination
	items.push_back("dx");
	items.push_back("dy");
	items.push_back("dz");
	//! distance
	items.push_back("distance");
	//! info



	mCharInfoPanel = mTrayMgr->createParamsPanel(TL_RIGHT, "CharInfoPanel", 120, items);
	mCharInfoPanel->show();

	mCharInfoPanel->setParamValue(0, "0000");
	mCharInfoPanel->setParamValue(1, "0000");
	mCharInfoPanel->setParamValue(2, "0000");
	mCharInfoPanel->setParamValue(3, "0000");
	mCharInfoPanel->setParamValue(4, "0000");
	//! current postion
	mCharInfoPanel->setParamValue(5, "0000");
	mCharInfoPanel->setParamValue(6, "0000");
	mCharInfoPanel->setParamValue(7, "0000");
	//! destination
	mCharInfoPanel->setParamValue(8, "0000");
	mCharInfoPanel->setParamValue(9, "0000");
	mCharInfoPanel->setParamValue(10, "0000");
	//! distance
	mCharInfoPanel->setParamValue(11, "0000");


}

void SinbadApp::setupDebugInfoPanel()
{
	Ogre::StringVector items;
	items.push_back(">>");

	mDebugInfoPanel = mTrayMgr->createParamsPanel(TL_BOTTOM, "DebugInfoPanel", 400, items);
	mDebugInfoPanel->show();
}
