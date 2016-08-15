#pragma once


/////////////////////////////////////////////////////////////////////////////
// Name:        net.cpp
// Purpose:     wxWidgets sample demonstrating network-related functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.07.99
// RCS-ID:      $Id: nettest.cpp,v 1.15 2005/05/31 09:19:17 JS Exp $
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_DIALUP_MANAGER
#error You must set wxUSE_DIALUP_MANAGER to 1 in setup.h!
#endif

//#include "wx/dialup.h"



// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class FairySceneCanvas;


class wxLayoutManager;


// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
	WX_DECLARE_CONTROL_CONTAINER();


public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnHangUp(wxCommandEvent& event);
    void OnDial(wxCommandEvent& event);
    void OnEnumISPs(wxCommandEvent& event);
    void OnCheck(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);

    void OnIdle(wxIdleEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()


private:
	FairySceneCanvas* mCanvas;

	// ¹¤¾ßÀ¸
	wxToolBar*		m_pToolbar;



	wxLayoutManager* mlayout_manager;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    NetTest_Quit = 1,
    NetTest_About,
    NetTest_HangUp,
    NetTest_Dial,
    NetTest_EnumISP,
    NetTest_Check,
    NetTest_Max
};

