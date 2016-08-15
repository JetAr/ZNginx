#include "XrApp.h"

#include "MainFrame.h"

BEGIN_EVENT_TABLE(XrApp, wxApp)

EVT_IDLE(XrApp::OnIdle)

//EVT_DIALUP_CONNECTED(MyApp::OnConnected)
//EVT_DIALUP_DISCONNECTED(MyApp::OnConnected)
END_EVENT_TABLE()

IMPLEMENT_APP(XrApp)

// `Main program' equivalent: the program execution "starts" here
bool XrApp::OnInit()
{
	// Create the main application window


	m_frame = new MyFrame(_T("OGRE±à¼­Æ÷"),wxPoint(50, 50), wxSize(450, 340));
	m_frame->SetBackgroundColour(*wxBLUE);

		
	// Show it and tell the application that it's our main window
	m_frame->Show(true);
	SetTopWindow(m_frame);

	// Init dial up manager


	m_frame->SetStatusText(_T("No LAN"), 2);

	return true;
}

int XrApp::OnExit()
{
	//delete m_frame;
	// exit code is 0, everything is ok
	return 0;
}

void XrApp::OnIdle(wxIdleEvent &e)
{
	wxApp::OnIdle(e);	

	//! renderWindow->isActive() && 
	//if ( mSceneManipulator && mSceneManipulator->getRenderWindow() && mSceneManipulator->getRenderWindow()->isActive()
	//	&& mIsRenderEnable && mMainFrame->GetCanvas()->doUpdate(true) )
	//{
	//	e.RequestMore(IsActive());
	//}//
}


