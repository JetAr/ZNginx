/****************************************
*   Nehe MFC by Yvo van Dillen based on *
*   Jeff Molofee's Basecode Example     *
*          nehe.gamedev.net             *
*             2001/2004                 *
*                                       *
*****************************************/

#include "stdafx.h"
#include "NeheMFC.h"
#include "NeheWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CNeheWindow

CNeheWindow::CNeheWindow()
{	
}

CNeheWindow::~CNeheWindow()
{
}


BEGIN_MESSAGE_MAP(CNeheWindow, CWnd)
	//{{AFX_MSG_MAP(CNeheWindow)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_TOGGLEFULLSCREEN , OnToggleFullScreen )
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNeheWindow message handlers

void CNeheWindow::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
	switch (lParam)													// Check System Calls
	{
			case SC_SCREENSAVE:										// Screensaver Trying To Start?
			case SC_MONITORPOWER:									// Monitor Trying To Enter Powersave?
			return;													// Prevent From Happening
	}

	CWnd::OnSysCommand(nID, lParam);
}

void CNeheWindow::OnClose() 
{		
	CWnd::CloseWindow();
	theApp.TerminateApplication();
}

void CNeheWindow::OnSize(UINT nType, int cx, int cy) 
{	
	switch (nType)											// Evaluate Size Action
	{
		case SIZE_MINIMIZED:								// Was Window Minimized?
			theApp.m_isVisible = FALSE;						// Set isVisible To False
		return;												// Return

		case SIZE_MAXIMIZED:								// Was Window Maximized?
			theApp.m_isVisible = TRUE;						// Set isVisible To True
			theApp.ReshapeGL (cx,cy);						// Reshape Window - LoWord=Width, HiWord=Height
		return;												// Return

		case SIZE_RESTORED:									// Was Window Restored?
			theApp.m_isVisible = TRUE;					    // Set isVisible To True
			theApp.ReshapeGL (cx,cy);						// Reshape Window - LoWord=Width, HiWord=Height
		return;												// Return
	}
}

void CNeheWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	if ((nChar >= 0) && (nChar <= 255))						// Is Key (nChar) In A Valid Range?
	{
		theApp.keyDown [nChar] = TRUE;						// Set The Selected Key (wParam) To True
		return;												// Return
	}	
}

void CNeheWindow::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar >= 0) && (nChar <= 255))						// Is Key (nChar) In A Valid Range?
	{
		theApp.keyDown [nChar] = FALSE;						// Set The Selected Key (wParam) To True
		return;												// Return
	}	
}

LRESULT CNeheWindow::OnToggleFullScreen(WPARAM w,LPARAM l)
{
	theApp.ToggleFullScreen();								// somebody (app/you) want to toggle fullscreen
	return 1;
}


BOOL CNeheWindow::OnEraseBkgnd(CDC* pDC) 
{
	//Disable that irritating flicker thingy
	return FALSE;	
}
