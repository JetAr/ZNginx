
#include "MainFrame.h"

#include <wx/dockwindow.h>


#include <wx/layoutmanager.h>
#include <wx/dockhost.h>

#include "FairySceneCanvas.h"

// For compilers that support precompilation, includes "wx/wx.h".

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------


// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(NetTest_Quit,  MyFrame::OnQuit)
    EVT_MENU(NetTest_About, MyFrame::OnAbout)
    EVT_MENU(NetTest_HangUp, MyFrame::OnHangUp)
    EVT_MENU(NetTest_Dial, MyFrame::OnDial)
    EVT_MENU(NetTest_EnumISP, MyFrame::OnEnumISPs)
    EVT_MENU(NetTest_Check, MyFrame::OnCheck)

    EVT_UPDATE_UI(NetTest_Dial, MyFrame::OnUpdateUI)

    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------
WX_DELEGATE_TO_CONTROL_CONTAINER(MyFrame)

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{

	mCanvas = new FairySceneCanvas(this, wxID_ANY);
	m_container.SetDefaultItem(mCanvas);


	//! Tool bar
	m_pToolbar = CreateToolBar();
	//m_pToolbar->AddCheckTool(wxID_HIGHEST + 1, _("关联画刷大小"),	wxBITMAP(locker), wxNullBitmap, _("关联画刷大小"));



    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(NetTest_Dial, _T("&Dial\tCtrl-D"), _T("Dial default ISP"));
    menuFile->Append(NetTest_HangUp, _T("&HangUp\tCtrl-H"), _T("Hang up modem"));
    menuFile->AppendSeparator();
    menuFile->Append(NetTest_EnumISP, _T("&Enumerate ISPs...\tCtrl-E"));
    menuFile->Append(NetTest_Check, _T("&Check connection status...\tCtrl-C"));
    menuFile->AppendSeparator();
    menuFile->Append(NetTest_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(NetTest_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create status bar and fill the LAN field
    CreateStatusBar(3);
    static const int widths[3] = { -1, 100, 60 };
    SetStatusWidths(3, widths);


	//! win 1
	wxDockWindow *dock_window1 = new wxDockWindow(this, wxID_ANY,"Dock Window 1", wxPoint(64, 64),  wxSize(128, 128));  // Size when it is not docked.
	wxTextCtrl *dock_txt = new wxTextCtrl(this,                       // Parent (it will Reparent to the
		wxID_ANY,                   // Id
		"Text",                     // Initial text
		wxDefaultPosition,          // Position
		wxDefaultSize,              // Size
		wxTE_MULTILINE);            // Style

	dock_window1->SetClient(dock_txt); // Places the text control inside the wxDockWindow
	dock_window1->GetClient()->SetBackgroundColour(wxColour(255, 0, 0));

	//! win 2
	wxDockWindow *dock_window2 = new wxDockWindow( this, wxID_ANY, ("Dock Window #2"), wxPoint( 96, 96 ), wxSize( 128, 128 ), ("d1") );
	wxWindow *p = dock_window2->GetClient();
	p->SetBackgroundColour( *wxRED );
	dock_window2->Show(true);


	//! win 3
	wxDockWindow *dock_window3 = new wxDockWindow( this, wxID_ANY, ("Dock Window #3"), wxPoint( 128, 128 ), wxSize( 128, 128 ), ("d2") );
	dock_window3->GetClient()->SetBackgroundColour( *wxGREEN );
	dock_window3->Show();




	mlayout_manager = new wxLayoutManager( this ); // The parameter is the frame to
	// attach this wxLayoutManager to.
	mlayout_manager->AddDefaultHosts();           // Creates the Top, Right, Bottom, and Left dock hosts.


	wxHostInfo hostInfo = mlayout_manager->GetDockHost(wxDEFAULT_LEFT_HOST);
	mlayout_manager->DockWindow(dock_window1, hostInfo);
	hostInfo.GetHost()->SetAreaSize( 128 );

	hostInfo = mlayout_manager->GetDockHost(wxDEFAULT_RIGHT_HOST);
	//mlayout_manager->DockWindow(dock_window2, hostInfo);
	hostInfo.GetHost()->SetAreaSize( 64 );


	hostInfo = mlayout_manager->GetDockHost(wxDEFAULT_BOTTOM_HOST);
	//mlayout_manager->DockWindow(dock_window3, hostInfo);
	hostInfo.GetHost()->SetAreaSize( 128 );


	// You must call AddDockWindow for each wxDockWindow you want this wxLayoutManager to recognize and handle (this does not dock any of the windows though).

	mlayout_manager->AddDockWindow(dock_window1);
	mlayout_manager->AddDockWindow(dock_window2);
	mlayout_manager->AddDockWindow(dock_window3);


	mlayout_manager->SetLayout(wxDWF_LIVE_UPDATE | wxDWF_SPLITTER_BORDERS,mCanvas);



	


}


MyFrame::~MyFrame()
{
	delete mlayout_manager;
	int i = 0 ;

}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( wxT("This is the network functions test sample.\n")
                wxT("(c) 1999 Vadim Zeitlin") );

    wxMessageBox(msg, wxT("About NetTest"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnHangUp(wxCommandEvent& WXUNUSED(event))
{
        wxLogStatus(this, wxT("Connection was successfully terminated."));
        wxLogStatus(this, wxT("Failed to hang up."));
}

void MyFrame::OnDial(wxCommandEvent& WXUNUSED(event))
{
    wxLogStatus(this, wxT("Preparing to dial..."));
    wxYield();
    wxBeginBusyCursor();

    if ( 1 )
    {
        wxLogStatus(this, wxT("Dialing..."));
    }
    else
    {
        wxLogStatus(this, wxT("Dialing attempt failed."));
    }

    wxEndBusyCursor();
}

void MyFrame::OnCheck(wxCommandEvent& WXUNUSED(event))
{
   if(1 )
      wxLogMessage(wxT("Network is online."));
   else
      wxLogMessage(wxT("Network is offline."));
}

void MyFrame::OnEnumISPs(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString names;
    size_t nCount = 0;
    if ( nCount == 0 )
    {
        wxLogWarning(wxT("No ISPs found."));
    }
    else
    {
        wxString msg = _T("Known ISPs:\n");
        for ( size_t n = 0; n < nCount; n++ )
        {
            msg << names[n] << '\n';
        }

        wxLogMessage(msg);
    }
}

void MyFrame::OnUpdateUI(wxUpdateUIEvent& event)
{
    // disable this item while dialing
    event.Enable( true );
}

void MyFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    static int s_isOnline = -1; // not true nor false

    bool isOnline = false;
    if ( s_isOnline != (int)isOnline )
    {
        s_isOnline = isOnline;

        SetStatusText(isOnline ? _T("Online") : _T("Offline"), 1);
    }
}
