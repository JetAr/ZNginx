// OgreMFC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OgreMFC.h"
#include "MainFrm.h"

#include "OgreConfigFile.h"



// COgreMFCApp

BEGIN_MESSAGE_MAP(COgreMFCApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()


// COgreMFCApp construction

COgreMFCApp::COgreMFCApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	mRoot = 0;
}

COgreMFCApp::~COgreMFCApp()
{
	delete mRoot;
}



// The one and only COgreMFCApp object

COgreMFCApp theApp;

// COgreMFCApp initialization

BOOL COgreMFCApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();
	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();


	///////////////////////////////////////////////////////////////////////////
	//
	// OGRE specific implementations
	//
	///////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
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
		AfxMessageBox("Unable to locate OpenGL rendering system.  Application is terminating");
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

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	return TRUE;
}



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void COgreMFCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// COgreMFCApp message handlers

