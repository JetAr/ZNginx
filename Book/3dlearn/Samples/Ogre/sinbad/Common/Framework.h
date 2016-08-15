#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#include "./XSample.h"
#include "./xSdkTrays.h"
#include "./XSdkCameraMan.h"

#ifdef USE_RTSHADER_SYSTEM
#include "OgreRTShaderSystem.h"
#endif

using namespace XRENREN;

namespace XRENREN
{
	/*=============================================================================
	// Base SDK sample class. Includes default player camera and SDK trays.
	=============================================================================*/
	class Framework : public XRENREN::Sample, public XRENREN::SdkTrayListener
	{
	public:

		Framework();

		virtual ~Framework();

		/*-----------------------------------------------------------------------------
		| Manually update the cursor position after being unpaused.
		-----------------------------------------------------------------------------*/
		virtual void unpaused();

		/*-----------------------------------------------------------------------------
		| Automatically saves position and orientation for free-look cameras.
		-----------------------------------------------------------------------------*/
		virtual void saveState(Ogre::NameValuePairList& state);

		/*-----------------------------------------------------------------------------
		| Automatically restores position and orientation for free-look cameras.
		-----------------------------------------------------------------------------*/
		virtual void restoreState(Ogre::NameValuePairList& state);

		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

		virtual void windowResized(Ogre::RenderWindow* rw);

		virtual bool keyPressed(const OIS::KeyEvent& evt);


		virtual bool keyReleased(const OIS::KeyEvent& evt);

		/* IMPORTANT: When overriding these following handlers, remember to allow the tray manager
		to filter out any interface-related mouse events before processing them in your scene.
		If the tray manager handler returns true, the event was meant for the trays, not you. */
		virtual bool mouseMoved(const OIS::MouseEvent& evt);

		virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);


		virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

		/*-----------------------------------------------------------------------------
		| Extended to setup a default tray interface and camera controller.
		-----------------------------------------------------------------------------*/
		virtual void _setup(Ogre::RenderWindow* window, OIS::Keyboard* keyboard, OIS::Mouse* mouse, XRENREN::FileSystemLayer* fsLayer);

		virtual void _shutdown();

	protected:

		virtual void setupView();

		virtual void setDragLook(bool enabled);

		Ogre::Viewport* mViewport;    		// main viewport
		Ogre::Camera* mCamera;        		// main camera
		SdkTrayManager* mTrayMgr;     		// tray interface manager
		SdkCameraMan* mCameraMan;     		// basic camera controller
		ParamsPanel* mDetailsPanel;   		// sample details panel
		bool mCursorWasVisible;				// was cursor visible before dialog appeared
		bool mDragLook;                     // click and drag to free-look

		void setupDetailsPanel();


	};
}

#endif
