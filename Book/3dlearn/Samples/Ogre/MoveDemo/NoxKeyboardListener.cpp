#include "stdafx.h"
#include "NoxKeyboardListener.h"

NoxKeyboardListener::NoxKeyboardListener(Ogre::RenderWindow* window , Ogre::Camera* camera
, OgreBites::SdkCameraMan* cameraMan , 	OgreBites::SdkTrayManager* trayMgr , OgreBites::ParamsPanel* detailsPanel)
 : mWindow(window)
 , mCamera(camera)
 , mCameraMan(cameraMan)
 , mTrayMgr(trayMgr)
 , mDetailsPanel(detailsPanel)
{


}

NoxKeyboardListener::~NoxKeyboardListener(void)
{
}



//-------------------------------------------------------------------------------------
bool NoxKeyboardListener::keyPressed( const OIS::KeyEvent &arg )
{
	if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

	if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
	{
		mTrayMgr->toggleAdvancedFrameStats();
	}
	else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
	{
		//if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
		//{
		//	//mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_NONE, 0);
		//	mDetailsPanel->show();
		//}
		//else
		//{
		//	mTrayMgr->removeWidgetFromTray(mDetailsPanel);
		//	mDetailsPanel->hide();
		//}
	}
	else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
	{
		Ogre::String newVal;
		Ogre::TextureFilterOptions tfo;
		unsigned int aniso;

		switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
		{
		case 'B':
			newVal = "Trilinear";
			tfo = Ogre::TFO_TRILINEAR;
			aniso = 1;
			break;
		case 'T':
			newVal = "Anisotropic";
			tfo = Ogre::TFO_ANISOTROPIC;
			aniso = 8;
			break;
		case 'A':
			newVal = "None";
			tfo = Ogre::TFO_NONE;
			aniso = 1;
			break;
		default:
			newVal = "Bilinear";
			tfo = Ogre::TFO_BILINEAR;
			aniso = 1;
		}

		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
		mDetailsPanel->setParamValue(9, newVal);
	}
	else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
	{
		Ogre::String newVal;
		Ogre::PolygonMode pm;

		switch (mCamera->getPolygonMode())
		{
		case Ogre::PM_SOLID:
			newVal = "Wireframe";
			pm = Ogre::PM_WIREFRAME;
			break;
		case Ogre::PM_WIREFRAME:
			newVal = "Points";
			pm = Ogre::PM_POINTS;
			break;
		default:
			newVal = "Solid";
			pm = Ogre::PM_SOLID;
		}

		mCamera->setPolygonMode(pm);
		//mDetailsPanel->setParamValue(10, newVal);
	}
	else if(arg.key == OIS::KC_F5)   // refresh all textures
	{
		Ogre::TextureManager::getSingleton().reloadAll();
	}
	else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
	{
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
	}
	else if (arg.key == OIS::KC_ESCAPE)
	{
		//mShutDown = true;
		//mWindow->destroy();



	}

	mCameraMan->injectKeyDown(arg);
	return true;
}

bool NoxKeyboardListener::keyReleased( const OIS::KeyEvent &arg )
{
	mCameraMan->injectKeyUp(arg);
	return true;
}