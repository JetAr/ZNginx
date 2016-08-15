/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __SampleContext_H__
#define __SampleContext_H__

#include "Ogre.h"
#include "OgrePlugin.h"
#include "XSample.h"
#include "FileSystemLayerImpl.h"

#include "OIS.h"

namespace XRENREN
{
	/*=============================================================================
	| Base class responsible for setting up a common context for samples.
	| May be subclassed for specific sample types (not specific samples).
	| Allows one sample to run at a time, while maintaining a sample queue.
	=============================================================================*/
	class SampleContext :
		public Ogre::FrameListener,
		public Ogre::WindowEventListener,
		public OIS::KeyListener,
		public OIS::MouseListener
	{
	public:

		SampleContext()
		{
			mFSLayer = OGRE_NEW_T(FileSystemLayerImpl, Ogre::MEMCATEGORY_GENERAL)(OGRE_VERSION_NAME);
			mRoot = 0;
			mWindow = 0;
			mCurrentSample = 0;
			mSamplePaused = false;
			mFirstRun = true;
			mLastRun = false;
			mLastSample = 0;
			mInputMgr = 0;
			mKeyboard = 0;
			mMouse = 0;
		}

		virtual ~SampleContext() 
		{
			OGRE_DELETE_T(mFSLayer, FileSystemLayer, Ogre::MEMCATEGORY_GENERAL);
		}

		virtual Ogre::RenderWindow* getRenderWindow()
		{
			return mWindow;
		}

		virtual Sample* getCurrentSample()
		{
			return mCurrentSample;
		}

		/*-----------------------------------------------------------------------------
		| Quits the current sample and starts a new one.
		-----------------------------------------------------------------------------*/
		virtual void runSample(Sample* s)
		{
			if (mCurrentSample)
			{
				mCurrentSample->_shutdown();    // quit current sample
				mSamplePaused = false;          // don't pause the next sample
			}

			mWindow->removeAllViewports();                  // wipe viewports

			if (s)
			{
				// retrieve sample's required plugins and currently installed plugins
				Ogre::Root::PluginInstanceList ip = mRoot->getInstalledPlugins();
				Ogre::StringVector rp = s->getRequiredPlugins();

				for (Ogre::StringVector::iterator j = rp.begin(); j != rp.end(); j++)
				{
					bool found = false;
					// try to find the required plugin in the current installed plugins
					for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); k++)
					{
						if ((*k)->getName() == *j)
						{
							found = true;
							break;
						}
					}
					if (!found)  // throw an exception if a plugin is not found
					{
						Ogre::String desc = "Sample requires plugin: " + *j;
						Ogre::String src = "SampleContext::runSample";
						OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, desc, src);
					}
				}

				// throw an exception if samples requires the use of another renderer
				Ogre::String rrs = s->getRequiredRenderSystem();
				if (!rrs.empty() && rrs != mRoot->getRenderSystem()->getName())
				{
					Ogre::String desc = "Sample only runs with renderer: " + rrs;
					Ogre::String src = "SampleContext::runSample";
					OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, desc, src);
				}

				// test system capabilities against sample requirements
				s->testCapabilities(mRoot->getRenderSystem()->getCapabilities());

				s->_setup(mWindow, mKeyboard, mMouse, mFSLayer);   // start new sample
			}

			mCurrentSample = s;
		}

		/*-----------------------------------------------------------------------------
		| This function encapsulates the entire lifetime of the context.
		-----------------------------------------------------------------------------*/
		virtual void go(Sample* initialSample = 0)
		{
			while (!mLastRun)
			{
				mLastRun = true;  // assume this is our last run

				createRoot();
				if (!oneTimeConfig()) return;

				// if the context was reconfigured, set requested renderer
				if (!mFirstRun) mRoot->setRenderSystem(mRoot->getRenderSystemByName(mNextRenderer));

				setup();

				// restore the last sample if there was one or, if not, start initial sample
				if (!mFirstRun)
					recoverLastSample();
				else if (initialSample)
					runSample(initialSample);

				mRoot->startRendering();    // start the render loop

				mRoot->saveConfig();
				shutdown();
				if (mRoot) OGRE_DELETE mRoot;
				mFirstRun = false;
			}
		}

		virtual bool isCurrentSamplePaused()
		{
			if (mCurrentSample) return mSamplePaused;
			return false;
		}

		virtual void pauseCurrentSample()
		{
			if (mCurrentSample && !mSamplePaused)
			{
				mSamplePaused = true;
				mCurrentSample->paused();
			}
		}

		virtual void unpauseCurrentSample()
		{
			if (mCurrentSample && mSamplePaused)
			{
				mSamplePaused = false;
				mCurrentSample->unpaused();
			}
		}

		/*-----------------------------------------------------------------------------
		| Processes frame started events.
		-----------------------------------------------------------------------------*/
		virtual bool frameStarted(const Ogre::FrameEvent& evt)
		{
			captureInputDevices();      // capture input


			// relay input events to character controller
			//if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);

			// manually call sample callback to ensure correct order
			return (mCurrentSample && !mSamplePaused) ? mCurrentSample->frameStarted(evt) : true;
		}

		/*-----------------------------------------------------------------------------
		| Processes rendering queued events.
		-----------------------------------------------------------------------------*/
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt)
		{
			// manually call sample callback to ensure correct order
			return (mCurrentSample && !mSamplePaused) ? mCurrentSample->frameRenderingQueued(evt) : true;
		}

		/*-----------------------------------------------------------------------------
		| Processes frame ended events.
		-----------------------------------------------------------------------------*/
		virtual bool frameEnded(const Ogre::FrameEvent& evt)
		{
			// manually call sample callback to ensure correct order
			if (mCurrentSample && !mSamplePaused && !mCurrentSample->frameEnded(evt)) return false;
			// quit if window was closed
			if (mWindow->isClosed()) return false;
			// go into idle mode if current sample has ended
			if (mCurrentSample && mCurrentSample->isDone()) runSample(0);

			return true;
		}

		/*-----------------------------------------------------------------------------
		| Processes window size change event. Adjusts mouse's region to match that
		| of the window. You could also override this method to prevent resizing.
		-----------------------------------------------------------------------------*/
		virtual void windowResized(Ogre::RenderWindow* rw)
		{
			// manually call sample callback to ensure correct order
			if (mCurrentSample && !mSamplePaused) mCurrentSample->windowResized(rw);

			const OIS::MouseState& ms = mMouse->getMouseState();
			ms.width = rw->getWidth();
			ms.height = rw->getHeight();
		}

		// window event callbacks which manually call their respective sample callbacks to ensure correct order

		virtual void windowMoved(Ogre::RenderWindow* rw)
		{
			if (mCurrentSample && !mSamplePaused) mCurrentSample->windowMoved(rw);
		}

		virtual bool windowClosing(Ogre::RenderWindow* rw)
		{
			if (mCurrentSample && !mSamplePaused) return mCurrentSample->windowClosing(rw);
			return true;
		}

		virtual void windowClosed(Ogre::RenderWindow* rw)
		{
			if (mCurrentSample && !mSamplePaused) mCurrentSample->windowClosed(rw);
		}

		virtual void windowFocusChange(Ogre::RenderWindow* rw)
		{
			if (mCurrentSample && !mSamplePaused) mCurrentSample->windowFocusChange(rw);
		}

		// keyboard and mouse callbacks which manually call their respective sample callbacks to ensure correct order

		virtual bool keyPressed(const OIS::KeyEvent& evt)
		{
			if (mCurrentSample && !mSamplePaused) return mCurrentSample->keyPressed(evt);
			return true;
		}

		virtual bool keyReleased(const OIS::KeyEvent& evt)
		{
			if (mCurrentSample && !mSamplePaused) return mCurrentSample->keyReleased(evt);
			return true;
		}


		virtual bool mouseMoved(const OIS::MouseEvent& evt)
		{
			if (mCurrentSample && !mSamplePaused) return mCurrentSample->mouseMoved(evt);
			return true;
		}

		virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			if (mCurrentSample && !mSamplePaused) return mCurrentSample->mousePressed(evt, id);
			return true;
		}


		virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			if (mCurrentSample && !mSamplePaused) return mCurrentSample->mouseReleased(evt, id);
			return true;
		}

	protected:

		/*-----------------------------------------------------------------------------
		| Sets up the context after configuration.
		-----------------------------------------------------------------------------*/
		virtual void setup()
		{
			createWindow();
			setupInput();
			locateResources();
			loadResources();

			Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

			// adds context as listener to process context-level (above the sample level) events
			mRoot->addFrameListener(this);
			Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
		}

		/*-----------------------------------------------------------------------------
		| Creates the OGRE root.
		-----------------------------------------------------------------------------*/
		virtual void createRoot()
		{
			Ogre::String pluginsPath = Ogre::StringUtil::BLANK;
			pluginsPath = mFSLayer->getConfigFilePath("plugins.cfg");
			mRoot = OGRE_NEW Ogre::Root(pluginsPath, mFSLayer->getWritablePath("ogre.cfg"), 
				mFSLayer->getWritablePath("ogre.log"));

		}

		/*-----------------------------------------------------------------------------
		| Configures the startup settings for OGRE. I use the config dialog here,
		| but you can also restore from a config file. Note that this only happens
		| when you start the context, and not when you reset it.
		-----------------------------------------------------------------------------*/
		virtual bool oneTimeConfig()
		{
			return mRoot->showConfigDialog();
			// return mRoot->restoreConfig();
		}

		/*-----------------------------------------------------------------------------
		| Creates the render window to be used for this context. I use an auto-created
		| window here, but you can also create an external window if you wish.
		| Just don't forget to initialise the root.
		-----------------------------------------------------------------------------*/
		virtual void createWindow()
		{
			mWindow = mRoot->initialise(true);
		}


		/*-----------------------------------------------------------------------------
		| Sets up OIS input.
		-----------------------------------------------------------------------------*/
		virtual void setupInput()
		{
			OIS::ParamList pl;
			size_t winHandle = 0;
			std::ostringstream winHandleStr;

			mWindow->getCustomAttribute("WINDOW", &winHandle);
			winHandleStr << winHandle;

			pl.insert(std::make_pair("WINDOW", winHandleStr.str()));
			// 设置鼠标显示和非游戏独占,这样鼠标可以显示在屏幕上并可以移动到窗口外
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
			pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));

			mInputMgr = OIS::InputManager::createInputSystem(pl);

			createInputDevices();      // create the specific input devices

			windowResized(mWindow);    // do an initial adjustment of mouse area
		}

		/*-----------------------------------------------------------------------------
		| Creates the individual input devices. I only create a keyboard and mouse
		| here because they are the most common, but you can override this method
		| for other modes and devices.
		-----------------------------------------------------------------------------*/
		virtual void createInputDevices()
		{
			mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard, true));
			mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));

			mKeyboard->setEventCallback(this);
			mMouse->setEventCallback(this);
		}

		/*-----------------------------------------------------------------------------
		| Finds context-wide resource groups. I load paths from a config file here,
		| but you can choose your resource locations however you want.
		-----------------------------------------------------------------------------*/
		virtual void locateResources()
		{
			// load resource paths from config file
			Ogre::ConfigFile cf;
			cf.load(mFSLayer->getConfigFilePath("resources.cfg"));

			Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
			Ogre::String sec, type, arch;

			// go through all specified resource groups
			while (seci.hasMoreElements())
			{
				sec = seci.peekNextKey();
				Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
				Ogre::ConfigFile::SettingsMultiMap::iterator i;

				// go through all resource paths
				for (i = settings->begin(); i != settings->end(); i++)
				{
					type = i->first;
					arch = i->second;

					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch, type, sec);
				}
			}
		}

		/*-----------------------------------------------------------------------------
		| Loads context-wide resource groups. I chose here to simply initialise all
		| groups, but you can fully load specific ones if you wish.
		-----------------------------------------------------------------------------*/
		virtual void loadResources()
		{
			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		}

		/*-----------------------------------------------------------------------------
		| Reconfigures the context. Attempts to preserve the current sample state.
		-----------------------------------------------------------------------------*/
		virtual void reconfigure(const Ogre::String& renderer, Ogre::NameValuePairList& options)
		{
			// save current sample state
			mLastSample = mCurrentSample;
			if (mCurrentSample) mCurrentSample->saveState(mLastSampleState);

			mNextRenderer = renderer;
			Ogre::RenderSystem* rs = mRoot->getRenderSystemByName(renderer);

			// set all given render system options
			for (Ogre::NameValuePairList::iterator it = options.begin(); it != options.end(); it++)
			{
				rs->setConfigOption(it->first, it->second);

			}

			mLastRun = false;             // we want to go again with the new settings
			mRoot->queueEndRendering();   // break from render loop
		}

		/*-----------------------------------------------------------------------------
		| Recovers the last sample after a reset. You can override in the case that
		| the last sample is destroyed in the process of resetting, and you have to
		| recover it through another means.
		-----------------------------------------------------------------------------*/
		virtual void recoverLastSample()
		{
			runSample(mLastSample);
			mLastSample->restoreState(mLastSampleState);
			mLastSample = 0;
			mLastSampleState.clear();
		}

		/*-----------------------------------------------------------------------------
		| Cleans up and shuts down the context.
		-----------------------------------------------------------------------------*/
		virtual void shutdown()
		{
			if (mCurrentSample)
			{
				mCurrentSample->_shutdown();
				mCurrentSample = 0;
			}

			// remove window event listener before shutting down OIS
			Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);

			shutdownInput();
		}

		/*-----------------------------------------------------------------------------
		| Destroys OIS input devices and the input manager.
		-----------------------------------------------------------------------------*/
		virtual void shutdownInput()
		{
			if (mInputMgr)
			{
				mInputMgr->destroyInputObject(mKeyboard);
				mInputMgr->destroyInputObject(mMouse);

				OIS::InputManager::destroyInputSystem(mInputMgr);
				mInputMgr = 0;
			}
		}

		/*-----------------------------------------------------------------------------
		| Captures input device states.
		-----------------------------------------------------------------------------*/
		virtual void captureInputDevices()
		{
			mKeyboard->capture();
			mMouse->capture();
		}

		FileSystemLayer* mFSLayer; 		// File system abstraction layer
		Ogre::Root* mRoot;              // OGRE root
		Ogre::RenderWindow* mWindow;    // render window
		OIS::InputManager* mInputMgr;   // OIS input manager
		OIS::Keyboard* mKeyboard;       // keyboard device
		OIS::Mouse* mMouse;             // mouse device
		Sample* mCurrentSample;         // currently running sample
		bool mSamplePaused;             // whether current sample is paused
		bool mFirstRun;                 // whether or not this is the first run
		bool mLastRun;                  // whether or not this is the final run
		Ogre::String mNextRenderer;     // name of renderer used for next run
		Sample* mLastSample;            // last sample run before reconfiguration
		Ogre::NameValuePairList mLastSampleState;     // state of last sample
	};
}

#endif
