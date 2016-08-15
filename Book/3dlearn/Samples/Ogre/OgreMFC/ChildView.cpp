// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include <mmsystem.h>
#include <OgreFrameListener.h>

#include "OgreMFC.h"
#include "ChildView.h"

using namespace Ogre;


// CChildView

CChildView::CChildView() : m_firstDraw(true)
{
	m_OgreRoot = 0;         // Until we know otherwise
	m_SceneManager = 0;
	m_Camera = 0;
	m_Window = 0;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_ROBOT_WALK, OnRobotWalk)
	ON_UPDATE_COMMAND_UI(ID_ROBOT_WALK, OnUpdateRobotWalk)
	ON_COMMAND(ID_ROBOT_SHOOT, OnRobotShoot)
	ON_UPDATE_COMMAND_UI(ID_ROBOT_SHOOT, OnUpdateRobotShoot)
	ON_COMMAND(ID_ROBOT_IDLE, OnRobotIdle)
	ON_UPDATE_COMMAND_UI(ID_ROBOT_IDLE, OnUpdateRobotIdle)
	ON_COMMAND(ID_ROBOT_SLUMP, OnRobotSlump)
	ON_UPDATE_COMMAND_UI(ID_ROBOT_SLUMP, OnUpdateRobotSlump)
	ON_COMMAND(ID_ROBOT_DIE, OnRobotDie)
	ON_UPDATE_COMMAND_UI(ID_ROBOT_DIE, OnUpdateRobotDie)
END_MESSAGE_MAP()



// =============================================================================
// CChildView::PreCreateWindow(CREATESTRUCT& cs) 
// -----------------------------------------------------------------------------
///CChildView message handlers
// =============================================================================
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}
// =============================================================================
// CChildView::OnPaint()
// -----------------------------------------------------------------------------
///Handle drawing into the child window.
// =============================================================================
void CChildView::OnPaint() 
{
	// Do the initial update things, since no OnInitialUpdate for raw CWnd objects.
	if(m_firstDraw)
	{
		m_firstDraw = false;
		SetupOgre();
		SetTimer(1, 30, NULL);
	}

	m_OgreRoot->renderOneFrame();

	ValidateRect(NULL);
}
// =============================================================================
// CChildView::SetupOgre(void)
// -----------------------------------------------------------------------------
///Setup the OGRE system.
// =============================================================================
void CChildView::SetupOgre(void)
{
	// Obtain pointer to the Ogre root object
	m_OgreRoot = ((COgreMFCApp *)AfxGetApp())->GetOgreRoot();

	//
	// Create a render window
	// This window should be the current ChildView window using the externalWindowHandle
	// value pair option.
	//

	NameValuePairList parms;
	parms["externalWindowHandle"] = StringConverter::toString((long)m_hWnd);

	CRect   rect;
	GetClientRect(&rect);

	m_Window = m_OgreRoot->createRenderWindow("MFC Window", rect.Width(), rect.Height(), false, &parms);

	//
	// Choose a scene manager
	//
	CreateSceneManager();

	// Create the camera
	CreateCamera();

	//
	// Create a viewport
	//
	CreateViewPort(rect);

	//
	// Load resources
	//
	loadResources();

	//
	// Create the scene
	//

	CreateScene();

	// Make window active and post an update
	m_Window->setActive(true);
	m_Window->update();
}
// =============================================================================
// CChildView::CreateSceneManager(void)
// -----------------------------------------------------------------------------
///
// =============================================================================
void CChildView::CreateSceneManager(void)
{
	m_SceneManager = m_OgreRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");
}
// =============================================================================
// CChildView::CreateCamera(void)
// -----------------------------------------------------------------------------
///
// =============================================================================
void CChildView::CreateCamera(void)
{
	///´´½¨ÉãÏñ»ú
	m_Camera = m_SceneManager->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	m_Camera->setPosition(Vector3(0,0,500));
	// Look back along -Z
	m_Camera->lookAt(Vector3(0,0,-300));
	m_Camera->setNearClipDistance(5);
}
// =============================================================================
// CChildView::CreateViewPort(void)
// -----------------------------------------------------------------------------
///
// =============================================================================
void CChildView::CreateViewPort(CRect& rect)
{
	// Create one viewport, entire window
	Ogre::Viewport* vp = m_Window->addViewport(m_Camera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	m_Camera->setAspectRatio(Ogre::Real(rect.Width()) / Ogre::Real(rect.Height()));

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
}
// =============================================================================
// CChildView::loadResources(void)
// -----------------------------------------------------------------------------
///
// =============================================================================
void CChildView::loadResources()
{
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
// =============================================================================
// CChildView::CreateScene(void)
// -----------------------------------------------------------------------------
///Create a scene in our Ogre system.
// =============================================================================
void CChildView::CreateScene(void)
{
	// Setup animation default
	Animation::setDefaultInterpolationMode(Animation::IM_LINEAR);
	Animation::setDefaultRotationInterpolationMode(Animation::RIM_LINEAR);

	// Set ambient light
	m_SceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

	// Create the robot scene
	m_scene.CreateScene(m_SceneManager);

	// Give it a little ambience with lights
	Light* l;
	l = m_SceneManager->createLight("BlueLight");
	l->setPosition(-200,-80,-100);
	l->setDiffuseColour(0.5, 0.5, 1.0);

	l = m_SceneManager->createLight("GreenLight");
	l->setPosition(0,0,-100);
	l->setDiffuseColour(0.5, 1.0, 0.5);

	// Position the camera
	m_Camera->setPosition(0, 50, 200);
	m_Camera->lookAt(0, 50, 0);

	// Add the frame listenter
	m_OgreRoot->addFrameListener(&m_scene);
}
// =============================================================================
// CChildView::OnEraseBkgnd(CDC* pDC)
// -----------------------------------------------------------------------------
///This simply overrides the default background erase so as to prevent flicker.
// =============================================================================
BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
// =============================================================================
// CChildView::OnTimer(UINT nIDEvent)
// -----------------------------------------------------------------------------
///Handle timer events.  I simply invalidate here.
// =============================================================================
void CChildView::OnTimer(UINT nIDEvent)
{
	Invalidate();

	CWnd::OnTimer(nIDEvent);
}
// =============================================================================
// CChildView::OnSize(UINT nType, int cx, int cy)
// -----------------------------------------------------------------------------
///Handle changes in window size.
// =============================================================================
void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_Window) 
	{ 
		CRect rect; 
		GetClientRect(&rect); 

		// notify "render window" instance 
		m_Window->windowMovedOrResized(); 

		// Adjust camera's aspect ratio, too 
		if (rect.Height() != 0 && m_Camera != 0) 
			m_Camera->setAspectRatio((Ogre::Real)m_Window->getWidth() / (Ogre::Real)m_Window->getHeight()); 

		m_Camera->yaw(Radian(0));
	} 
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_mouseLast = point;

	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(nFlags & MK_LBUTTON)
	{
		CPoint mouseDiff = point - m_mouseLast;
		m_mouseLast = point;

		m_Camera->yaw(Degree(mouseDiff.x) * 0.2);
		m_Camera->pitch(Degree(mouseDiff.y) * 0.2);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CChildView::OnRobotWalk()
{
	m_scene.SetRobotState(ID_ROBOT_WALK);
}

void CChildView::OnUpdateRobotWalk(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_scene.GetRobotState() == ID_ROBOT_WALK);
}

void CChildView::OnRobotShoot()
{
	m_scene.SetRobotState(ID_ROBOT_SHOOT);
}

void CChildView::OnUpdateRobotShoot(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_scene.GetRobotState() == ID_ROBOT_SHOOT);
}

void CChildView::OnRobotIdle()
{
	m_scene.SetRobotState(ID_ROBOT_IDLE);
}

void CChildView::OnUpdateRobotIdle(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_scene.GetRobotState() == ID_ROBOT_IDLE);
}

void CChildView::OnRobotSlump()
{
	m_scene.SetRobotState(ID_ROBOT_SLUMP);
}

void CChildView::OnUpdateRobotSlump(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_scene.GetRobotState() == ID_ROBOT_SLUMP);
}

void CChildView::OnRobotDie()
{
	m_scene.SetRobotState(ID_ROBOT_DIE);
}

void CChildView::OnUpdateRobotDie(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_scene.GetRobotState() == ID_ROBOT_DIE);
}
