#include <wx/wxprec.h>
#include <wx/dataobj.h>
#include <wx/clipbrd.h>
#include <wx\dcclient.h>

#include "FairySceneCanvas.h"


#include "ogrewrap.h"

BEGIN_EVENT_TABLE(FairySceneCanvas, wxWindow)
EVT_PAINT               (FairySceneCanvas::OnPaint)
EVT_ERASE_BACKGROUND    (FairySceneCanvas::OnEraseBackground)
EVT_SIZE                (FairySceneCanvas::OnSize)

EVT_MOTION              (FairySceneCanvas::OnMouseMove)
EVT_ENTER_WINDOW        (FairySceneCanvas::OnMouseEnter)
EVT_LEAVE_WINDOW        (FairySceneCanvas::OnMouseLeave)
EVT_MIDDLE_DOWN         (FairySceneCanvas::OnMiddleDown)
EVT_MIDDLE_UP           (FairySceneCanvas::OnMiddleUp)
EVT_LEFT_DOWN           (FairySceneCanvas::OnLeftDown)
EVT_LEFT_UP             (FairySceneCanvas::OnLeftUp)
EVT_MOUSEWHEEL          (FairySceneCanvas::OnMouseWheel)

EVT_KEY_DOWN            (FairySceneCanvas::OnKeyDown)
EVT_KEY_UP              (FairySceneCanvas::OnKeyUp)
EVT_CHAR                (FairySceneCanvas::OnChar)

EVT_MOUSE_CAPTURE_CHANGED (FairySceneCanvas::OnMouseCaptureChanged)
EVT_CONTEXT_MENU        (FairySceneCanvas::OnContextMenu)
EVT_WINDOW_DESTROY      (FairySceneCanvas::OnDestroy)
EVT_TIMER(FIX_FRAME_TIMTER, FairySceneCanvas::OnTimer)

END_EVENT_TABLE()

FairySceneCanvas::FairySceneCanvas(wxWindow *parent, wxWindowID id,
								   const wxPoint& pos, const wxSize& size, long style)
								   : wxWindow(parent, id, pos, size, style)
								   //, mSceneManipulator(NULL)
								   //, mUpdateListener(NULL)
								   //, mCameraManip(NULL)
								   , mDragButton(wxMOUSE_BTN_NONE)
								   //, mTerrGrid(NULL)
								   //, m_pParentFrame(NULL)
								   , mLastTime(0)
								   , mTargetFPS(0)
{
	mogreHandle = 0;

	mTargetFPS = 30;

	mTimer.SetOwner(this, FIX_FRAME_TIMTER);


}

FairySceneCanvas::~FairySceneCanvas(void)
{
	if(mogreHandle)
	{
		delete mogreHandle;
		mogreHandle = 0;

	}

	if(mRoot)
	{
		delete mRoot;
		mRoot = 0;

	}


}


void
FairySceneCanvas::OnPaint(wxPaintEvent &e)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

	static bool firstrun = true;

	

	if (firstrun)
	{
		firstrun = FALSE;
		InitOgre();
		mogreHandle = new OgreWrap;
		mogreHandle->SetupOgre((HWND)GetHandle() , mRoot);
		//SetTimer(1 , 30, NULL);
		mTimer.Stop();
		mTimer.Start(1000 / mTargetFPS);

	}
	else
	{
		GetOgreRoot()->renderOneFrame();
	}
	//ValidateRect(hWnd , NULL);


	//dc.DrawText("FairySceneCanvas" , wxPoint(100 , 100) );
	//if (GetSceneManipulator())
	//{
	//	//GetSceneManipulator()->updateWindow();
	//	doUpdate();

	//}
}

void
FairySceneCanvas::OnEraseBackground(wxEraseEvent &e)
{
	//if (!GetSceneManipulator())
	//{
		//wxWindow::OnEraseBackground(e);
	//}
}

void
FairySceneCanvas::OnSize(wxSizeEvent &e)
{
	//if (GetSceneManipulator())
	//{
	//	wxSize size = e.GetSize();
	//	GetSceneManipulator()->resizeWindow(size.x, size.y);
	//}
}

void
FairySceneCanvas::OnMouseCaptureChanged(wxMouseCaptureChangedEvent& e)
{
	if (e.GetEventObject() == this && e.GetCapturedWindow() != this)
	{
		switch (mDragButton)
		{
		case wxMOUSE_BTN_MIDDLE:
			{
				//wxASSERT(mCameraManip);
				//wxPoint pt = wxGetMousePosition();
				//pt = ScreenToClient(pt);
				//mCameraManip->onEnd(pt.x, pt.y, true);
				//mCameraManip = NULL;
				break;
			}

		case wxMOUSE_BTN_LEFT:
			{
				//wxASSERT(GetActiveAction());
				//wxPoint pt = wxGetMousePosition();
				//pt = ScreenToClient(pt);
				//GetActiveAction()->onEnd(pt.x, pt.y, true);
				//mUpdateListener->disable();
			}
			break;
		}

		mDragButton = wxMOUSE_BTN_NONE;
	}
}

void
FairySceneCanvas::OnMouseEnter(wxMouseEvent& e)
{
	//if (!GetSceneManipulator())
	//	return;

	//GetSceneManipulator()->showHitPoint(true);
}

void
FairySceneCanvas::OnMouseLeave(wxMouseEvent& e)
{
	//if (!GetSceneManipulator())
	//	return;

	//GetSceneManipulator()->showHitPoint(false);
}

void
FairySceneCanvas::OnMouseWheel(wxMouseEvent& e)
{
	//if (!GetSceneManipulator())
	//	return;

	//if (HasCapture())
	//	return;

	//Ogre::Real distance = - GetSceneManipulator()->_getWheelSpeed() * e.GetWheelRotation() / e.GetWheelDelta();
	//GetSceneManipulator()->getCamera()->moveRelative(Ogre::Vector3(0, 0, distance));

	//GetSceneManipulator()->_fireCameraPosChanged();

	//wxPoint pt = ScreenToClient(wxPoint(e.GetX(), e.GetY()));
	//if (GetActiveAction())
	//{
	//	GetActiveAction()->onMotion(pt.x, pt.y);
	//}
	//else
	//{
	//	GetSceneManipulator()->getHitIndicator("IntersectPoint")->setHitPoint(pt.x, pt.y);
	//}
}


void
FairySceneCanvas::OnKeyDown(wxKeyEvent& e)
{

}

void 
FairySceneCanvas::OnChar( wxKeyEvent &event )
{
	switch (event.m_keyCode)
	{
	case WXK_PAGEUP:
		//ProcessArrowKey(2);
		break;
	case WXK_PAGEDOWN:
		//ProcessArrowKey(-2);
		break;
	}

}

void FairySceneCanvas::OnKeyUp(wxKeyEvent& e)
{
    e.Skip(true);  // default to continue key process

    //typedef std::set<WX::ObjectPtr> ObjectSet;
    //const ObjectSet& objSet = GetSceneManipulator()->getSelectedObjects();
    //WX::ModifyObjectPropertyOperator* op = NULL;

    switch (e.GetKeyCode())
    {
    case WXK_UP:  
    case WXK_DOWN:
    case WXK_LEFT:
    case WXK_RIGHT:
    case WXK_HOME:
    case WXK_END:
        break;
          
    }
}

void
FairySceneCanvas::OnMouseMove(wxMouseEvent& e)
{

}
void
FairySceneCanvas::OnMiddleDown(wxMouseEvent& e)
{

}

void
FairySceneCanvas::OnMiddleUp(wxMouseEvent& e)
{

}

void
FairySceneCanvas::OnLeftDown(wxMouseEvent& e)
{

}

void
FairySceneCanvas::OnLeftUp(wxMouseEvent& e)
{

}

void
FairySceneCanvas::OnContextMenu(wxContextMenuEvent& e)
{

}

void
FairySceneCanvas::OnDestroy(wxWindowDestroyEvent& e)
{
	//if (GetSceneManipulator())
	//{
	//	GetSceneManipulator()->getRenderWindow()->setActive(false);
	//}
}


void FairySceneCanvas::OnTimer(wxTimerEvent& event)
{
	//if (GetSceneManipulator())
	//{
	//	GetSceneManipulator()->renderOneFrame();
	//}
	//InvalidateBestSize();
	InvalidateRect((HWND)GetHandle() , NULL , FALSE);

}

void FairySceneCanvas::setTargetFPS(unsigned long fps)
{
	if (mTargetFPS != fps)
	{
		mTargetFPS = fps;
		if (mTargetFPS)
		{
			mTimer.Stop();
			mTimer.Start(1000 / mTargetFPS);
		}
	}
}


BOOL FairySceneCanvas::InitOgre()
{
#ifdef _DEBUG
	mRoot = new Ogre::Root("plugins_d.cfg", "OgreMFC.cfg", "OgreMFC.log"); 
#else
	mRoot = new Ogre::Root("plugins.cfg", "OgreMFC.cfg", "OgreMFC.log"); 
#endif

	//
	// Setup paths to all resources
	//

	Ogre::ConfigFile cf;
	cf.load("resources_d.cfg");

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation( archName, typeName, secName);
		}
	}

	const RenderSystemList& render =  mRoot->getAvailableRenderers();

	RenderSystemList::const_iterator pRend = render.begin();

	while (pRend != render.end())
	{
		Ogre::String rName = (*pRend)->getName();
		//if (rName == "OpenGL Rendering Subsystem") //
		if (rName == "Direct3D9 Rendering Subsystem") //Direct3D9 Rendering Subsystem
			break;
		pRend++;
	}

	if (pRend == render.end())
	{
		// Unrecognised render system
		//MessageBox("Unable to locate OpenGL rendering system.  Application is terminating");
		return FALSE;
	}

	Ogre::RenderSystem *rsys = *pRend;
	rsys->setConfigOption("Full Screen", "No");
	rsys->setConfigOption("VSync", "Yes");

	// Set the rendering system.
	mRoot->setRenderSystem(rsys);

	//
	// Initialize the system, but don't create a render window.
	//
	mRoot->initialise(false);

	return TRUE;
}