#include "Framework.h"

namespace XRENREN
{
	Framework::Framework()
	{
		// so we don't have to worry about checking if these keys exist later
		mInfo["Title"] = "Untitled";
		mInfo["Description"] = "";
		mInfo["Category"] = "Unsorted";
		mInfo["Thumbnail"] = "";
		mInfo["Help"] = "";

		mTrayMgr = 0;
		mCameraMan = 0;
		mCamera = 0;
		mViewport = 0;
		mDetailsPanel = 0;
		mCursorWasVisible = false;
		mDragLook = false;
	}

	Framework::~Framework() {}

	/*-----------------------------------------------------------------------------
	| Manually update the cursor position after being unpaused.
	-----------------------------------------------------------------------------*/
	void Framework::unpaused()
	{
		mTrayMgr->refreshCursor();
	}

	/*-----------------------------------------------------------------------------
	| Automatically saves position and orientation for free-look cameras.
	-----------------------------------------------------------------------------*/
	void Framework::saveState(Ogre::NameValuePairList& state)
	{
		if (mCameraMan->getStyle() == CS_FREELOOK)
		{
			state["CameraPosition"] = Ogre::StringConverter::toString(mCamera->getPosition());
			state["CameraOrientation"] = Ogre::StringConverter::toString(mCamera->getOrientation());
		}
	}

	/*-----------------------------------------------------------------------------
	| Automatically restores position and orientation for free-look cameras.
	-----------------------------------------------------------------------------*/
	void Framework::restoreState(Ogre::NameValuePairList& state)
	{
		if (state.find("CameraPosition") != state.end() && state.find("CameraOrientation") != state.end())
		{
			mCameraMan->setStyle(CS_FREELOOK);
			mCamera->setPosition(Ogre::StringConverter::parseVector3(state["CameraPosition"]));
			mCamera->setOrientation(Ogre::StringConverter::parseQuaternion(state["CameraOrientation"]));
		}
	}

	bool Framework::frameRenderingQueued(const Ogre::FrameEvent& evt)
	{
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

#ifdef USE_RTSHADER_SYSTEM
				mDetailsPanel->setParamValue(14, Ogre::StringConverter::toString(mShaderGenerator->getVertexShaderCount()));
				mDetailsPanel->setParamValue(15, Ogre::StringConverter::toString(mShaderGenerator->getFragmentShaderCount()));		
#endif

				Ogre::Quaternion q = mCamera->getDerivedOrientation();
				mDetailsPanel->setParamValue(16,  Ogre::StringConverter::toString(q.xAxis() ) );
				mDetailsPanel->setParamValue(17,  Ogre::StringConverter::toString(q.yAxis() ));
				mDetailsPanel->setParamValue(18,  Ogre::StringConverter::toString(q.zAxis() ));															

			}	

		}

		return true;
	}

	void Framework::windowResized(Ogre::RenderWindow* rw)
	{
		mCamera->setAspectRatio((Ogre::Real)mViewport->getActualWidth() / (Ogre::Real)mViewport->getActualHeight());
	}

	bool Framework::keyPressed(const OIS::KeyEvent& evt)
	{
		if (evt.key == OIS::KC_H || evt.key == OIS::KC_F1)   // toggle visibility of help dialog
		{
			if (!mTrayMgr->isDialogVisible() && mInfo["Help"] != "") 
				mTrayMgr->showOkDialog("Help", mInfo["Help"]);
			else mTrayMgr->closeDialog();
		}

		if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

		if (evt.key == OIS::KC_F)   // toggle visibility of advanced frame stats
		{
			mTrayMgr->toggleAdvancedFrameStats();
		}
		else if (evt.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
		{
			if (mDetailsPanel->getTrayLocation() == TL_NONE)
			{
				mTrayMgr->moveWidgetToTray(mDetailsPanel, TL_TOPRIGHT, 0);
				mDetailsPanel->show();
			}
			else
			{
				mTrayMgr->removeWidgetFromTray(mDetailsPanel);
				mDetailsPanel->hide();
			}
		}
		else if (evt.key == OIS::KC_T)   // cycle polygon rendering mode
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
		else if (evt.key == OIS::KC_R)   // cycle polygon rendering mode
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
			mDetailsPanel->setParamValue(10, newVal);
		}
		else if(evt.key == OIS::KC_F5)   // refresh all textures
		{
			Ogre::TextureManager::getSingleton().reloadAll();
		}
		else if (evt.key == OIS::KC_SYSRQ)   // take a screenshot
		{
			mWindow->writeContentsToTimestampedFile("screenshot", ".png");
		}

#ifdef USE_RTSHADER_SYSTEM		
		// Toggle schemes.			
		else if (evt.key == OIS::KC_F2)
		{	
			Ogre::Viewport* mainVP = mCamera->getViewport();
			const Ogre::String& curMaterialScheme = mainVP->getMaterialScheme();

			if (curMaterialScheme == Ogre::MaterialManager::DEFAULT_SCHEME_NAME)
			{							
				mainVP->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
				mDetailsPanel->setParamValue(11, "On");
			}
			else if (curMaterialScheme == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
			{
				mainVP->setMaterialScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
				mDetailsPanel->setParamValue(11, "Off");
			}														
		}			

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS

		// Toggles per pixel per light model.
		else if (evt.key == OIS::KC_F3)
		{
			static bool usePerPixelLighting = true;					

			// Grab the scheme render state.												
			Ogre::RTShader::RenderState* schemRenderState = mShaderGenerator->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);


			// Add per pixel lighting sub render state to the global scheme render state.
			// It will override the default FFP lighting sub render state.
			if (usePerPixelLighting)
			{
				Ogre::RTShader::SubRenderState* perPixelLightModel = mShaderGenerator->createSubRenderState(Ogre::RTShader::PerPixelLighting::Type);

				schemRenderState->addTemplateSubRenderState(perPixelLightModel);					
			}

			// Search the per pixel sub render state and remove it.
			else
			{
				const Ogre::RTShader::SubRenderStateList& subRenderStateList = schemRenderState->getTemplateSubRenderStateList();
				Ogre::RTShader::SubRenderStateListConstIterator it = subRenderStateList.begin();
				Ogre::RTShader::SubRenderStateListConstIterator itEnd = subRenderStateList.end();

				for (; it != itEnd; ++it)
				{
					Ogre::RTShader::SubRenderState* curSubRenderState = *it;

					// This is the per pixel sub render state -> remove it.
					if (curSubRenderState->getType() == Ogre::RTShader::PerPixelLighting::Type)
					{
						schemRenderState->removeTemplateSubRenderState(*it);
						break;
					}
				}
			}

			// Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
			mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);


			// Update UI.
			if (usePerPixelLighting)
				mDetailsPanel->setParamValue(12, "Pixel");
			else
				mDetailsPanel->setParamValue(12, "Vertex");
			usePerPixelLighting = !usePerPixelLighting;				
		}	
#endif

		// Switch vertex shader outputs compaction policy.
		else if (evt.key == OIS::KC_F4)   
		{
			switch (mShaderGenerator->getVertexShaderOutputsCompactPolicy())
			{
			case Ogre::RTShader::VSOCP_LOW:
				mShaderGenerator->setVertexShaderOutputsCompactPolicy(Ogre::RTShader::VSOCP_MEDIUM);
				mDetailsPanel->setParamValue(13, "Medium");
				break;

			case Ogre::RTShader::VSOCP_MEDIUM:
				mShaderGenerator->setVertexShaderOutputsCompactPolicy(Ogre::RTShader::VSOCP_HIGH);
				mDetailsPanel->setParamValue(13, "High");
				break;

			case Ogre::RTShader::VSOCP_HIGH:
				mShaderGenerator->setVertexShaderOutputsCompactPolicy(Ogre::RTShader::VSOCP_LOW);
				mDetailsPanel->setParamValue(13, "Low");
				break;
			}

			// Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
			mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
		}	
#endif

		mCameraMan->injectKeyDown(evt);
		return true;
	}

	bool Framework::keyReleased(const OIS::KeyEvent& evt)
	{
		mCameraMan->injectKeyUp(evt);

		return true;
	}

	/* IMPORTANT: When overriding these following handlers, remember to allow the tray manager
	to filter out any interface-related mouse events before processing them in your scene.
	If the tray manager handler returns true, the event was meant for the trays, not you. */

	bool Framework::mouseMoved(const OIS::MouseEvent& evt)
	{
		if (mTrayMgr->injectMouseMove(evt)) return true;

		//mCameraMan->injectMouseMove(evt);

		return true;
	}

	bool Framework::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		if (mTrayMgr->injectMouseDown(evt, id)) return true;

		if (mDragLook && id == OIS::MB_Left)
		{
			mCameraMan->setStyle(CS_FREELOOK);
			mTrayMgr->hideCursor();
		}

		mCameraMan->injectMouseDown(evt, id);

		return true;
	}


	bool Framework::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		if (mTrayMgr->injectMouseUp(evt, id)) return true;

		if (mDragLook && id == OIS::MB_Left)
		{
			mCameraMan->setStyle(CS_MANUAL);
			mTrayMgr->showCursor();
		}

		mCameraMan->injectMouseUp(evt, id);

		return true;
	}

	/*-----------------------------------------------------------------------------
	| Extended to setup a default tray interface and camera controller.
	-----------------------------------------------------------------------------*/
	void Framework::_setup(Ogre::RenderWindow* window, OIS::Keyboard* keyboard, OIS::Mouse* mouse, XRENREN::FileSystemLayer* fsLayer)
	{
		// assign mRoot here in case Root was initialised after the Sample's constructor ran.
		mRoot = Ogre::Root::getSingletonPtr();
		mWindow = window;
		mMouse = mouse;
		mFSLayer = fsLayer;

		locateResources();
		createSceneManager();
		setupView();

		mTrayMgr = new SdkTrayManager("SampleControls", window, mouse, this);  // create a tray interface

#ifdef USE_RTSHADER_SYSTEM
		// Initialize shader generator.
		// Must be before resource loading in order to allow parsing extended material attributes.
		bool success = initializeRTShaderSystem(mSceneMgr);
		if (!success) 
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND, 
				"Shader Generator Initialization failed - Core shader libs path not found", 
				"SdkSample::_setup");
		}														
#endif

		loadResources();
		mResourcesLoaded = true;

		// show stats and logo and hide the cursor
		mTrayMgr->showFrameStats(TL_BOTTOMLEFT);
		mTrayMgr->hideLogo();
		mTrayMgr->showCursor();

		setupDetailsPanel();


		setupContent();
		mContentSetup = true;

		mDone = false;
	}

	void Framework::_shutdown()
	{
		Sample::_shutdown();

		if (mTrayMgr) delete mTrayMgr;
		if (mCameraMan) delete mCameraMan;

		// restore settings we may have changed, so as not to affect other samples
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(1);
	}

	void Framework::setupView()
	{
		// setup default viewport layout and camera
		mCamera = mSceneMgr->createCamera("MainCamera");
		mViewport = mWindow->addViewport(mCamera);
		mCamera->setAspectRatio((Ogre::Real)mViewport->getActualWidth() / (Ogre::Real)mViewport->getActualHeight());
		mCamera->setNearClipDistance(5);

		mCameraMan = new SdkCameraMan(mCamera);   // create a default camera controller
	}

	void Framework::setDragLook(bool enabled)
	{
		if (enabled)
		{
			mCameraMan->setStyle(CS_MANUAL);
			mTrayMgr->showCursor();
			mDragLook = true;
		}
		else
		{
			mCameraMan->setStyle(CS_FREELOOK);
			mTrayMgr->hideCursor();
			mDragLook = false;
		}
	}



	void Framework::setupDetailsPanel()
	{
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

#ifdef USE_RTSHADER_SYSTEM
		items.push_back("RT Shaders");
		items.push_back("LightingModel");
		items.push_back("CompactPolicy");
		items.push_back("Generated VS");
		items.push_back("Generated FS");														
#endif

		//! 16\17\18
		items.push_back("cam.xaxis");
		items.push_back("cam.yaxis");
		items.push_back("cam.zaxis");														

		mDetailsPanel = mTrayMgr->createParamsPanel(TL_NONE, "DetailsPanel", 150*2, items);
		mDetailsPanel->hide();

		mDetailsPanel->setParamValue(9, "Bilinear");
		mDetailsPanel->setParamValue(10, "Solid");

#ifdef USE_RTSHADER_SYSTEM
		mDetailsPanel->setParamValue(11, "Off");
		mDetailsPanel->setParamValue(12, "Vertex");
		mDetailsPanel->setParamValue(13, "Low");
		mDetailsPanel->setParamValue(14, "0");
		mDetailsPanel->setParamValue(15, "0");															
#endif
		mDetailsPanel->setParamValue(16, "0");
		mDetailsPanel->setParamValue(17, "0");
		mDetailsPanel->setParamValue(18, "0");															

	}


}
