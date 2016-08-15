/****************************************
*   Nehe MFC by Yvo van Dillen based on *
*   Jeff Molofee's Basecode Example     *
*          nehe.gamedev.net             *
*             2001/2004                 *
*                                       *
*****************************************/

#include "stdafx.h"
#include "NeheMFC.h"
#include "DisplaySettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	SHOW_DISPLAY_SETTINGS	FALSE		//set to false if you don't want this

#define	CLASS_NAME		"OpenGL"
#define WINDOW_TITLE	"NeheMFC"
/////////////////////////////////////////////////////////////////////////////
// CNeheMFCApp

BEGIN_MESSAGE_MAP(CNeheMFCApp, CWinApp)
	//{{AFX_MSG_MAP(CNeheMFCApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNeheMFCApp construction

CNeheMFCApp::CNeheMFCApp()
{
	// set default variables
	m_Width			= 640;						// Width
	m_Height		= 480;						// Height
	m_bitsPerPixel  = 16;						// Bits Per Pixel
	m_isFullScreen  = FALSE;					// FullScreen?
	m_isVisible     = TRUE;

	m_pDC = NULL;								// pointer to our device context
	m_hRC = NULL;								

	m_lastTickCount = 0;						// reset tickcount
	
	memset( keyDown , 0 , 256 );				// reset all keys
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNeheMFCApp object

CNeheMFCApp theApp;	//global app-object (also defined in stdafx.h)

/////////////////////////////////////////////////////////////////////////////
// CNeheMFCApp initialization

BOOL CNeheMFCApp::InitInstance()
{		
	BOOL				isMessagePumpActive;							// Message Pump Active?
	MSG					msg;											// Window Message Structure
	DWORD				tickCount;

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif
	
	
		
	//  start the application's message pump.
	
	if( SHOW_DISPLAY_SETTINGS )
	{
		int nRet;
		CDisplaySettingsDlg	*dlg = new CDisplaySettingsDlg;
		nRet = dlg->DoModal();
		delete dlg;

		if( nRet != IDOK )
			return FALSE;
	}

	if( !RegisterWindowClass() )	//Register
		return FALSE;

	m_pMainWnd = &m_wndMain;		//Set as main window


	m_isProgramLooping = TRUE;											// Program Looping Is Set To TRUE
	m_createFullScreen = m_isFullScreen;						// g_createFullScreen Is Set To User Default

	while (m_isProgramLooping)											// Loop Until WM_QUIT Is Received
	{
		// Create A Window
		m_isFullScreen = m_createFullScreen;					// Set Init Param Of Window Creation To Fullscreen?
		if (CreateOpenGLWindow() == TRUE)							// Was Window Creation Successful?
		{
			if( !m_isFullScreen )
				m_wndMain.CenterWindow();
			else
				m_wndMain.MoveWindow(0,0,m_Width,m_Height); 

			m_wndMain.ShowWindow( SW_NORMAL );
			m_isVisible = TRUE;

			// At This Point We Should Have A Window That Is Setup To Render OpenGL
			if (m_render.Initialize() == FALSE)					// Call User Intialization
			{
				// Failure
				TerminateApplication ();							// Close Window, This Will Handle The Shutdown
			}
			else														// Otherwise (Start The Message Pump)
			{	// Initialize was a success
				isMessagePumpActive = TRUE;								// Set isMessagePumpActive To TRUE
				while (isMessagePumpActive == TRUE)						// While The Message Pump Is Active
				{
					// Success Creating Window.  Check For Window Messages
					if (PeekMessage (&msg, m_wndMain.m_hWnd, 0, 0, PM_REMOVE) != 0)
					{
						// Check For WM_QUIT Message
						if (msg.message != WM_QUIT)						// Is The Message A WM_QUIT Message?
						{
							DispatchMessage (&msg);						// If Not, Dispatch The Message
						}
						else											// Otherwise (If Message Is WM_QUIT)
						{
							isMessagePumpActive = FALSE;				// Terminate The Message Pump
						}
					}
					else												// If There Are No Messages
					{
						if (m_isVisible == FALSE)					// If Window Is Not Visible
						{
							WaitMessage ();								// Application Is Minimized Wait For A Message
						}
						else											// If Window Is Visible
						{
							// Process Application Loop
							tickCount = GetTickCount ();				// Get The Tick Count
							m_render.Update (tickCount - m_lastTickCount);	// Update The Counter
							m_lastTickCount = tickCount;			// Set Last Count To Current Count
							m_render.Draw ();									// Draw Our Scene

							SwapBuffers (m_pDC->m_hDC);					// Swap Buffers (Double Buffering)
						}
					}
				}														// Loop While isMessagePumpActive == TRUE
			}															// If (Initialize (...

			// Application Is Finished
			m_render.Deinitialize ();											// User Defined DeInitialization

			DestroyOpenGLWindow ();									// Destroy The Active Window
		}
		else															// If Window Creation Failed
		{
			// Error Creating Window
			MessageBox (HWND_DESKTOP, "Error Creating OpenGL Window", "Error", MB_OK | MB_ICONEXCLAMATION);
			m_isProgramLooping = FALSE;									// Terminate The Loop
		}
	}

	UnregisterClass (CLASS_NAME, m_hInstance);		// UnRegister Window Class


	return FALSE;
}

/***************************************************************************************/

BOOL CNeheMFCApp::CreateOpenGLWindow()
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW ;							// Define Our Window Style
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;
		

	PIXELFORMATDESCRIPTOR pfd =											// pfd Tells Windows How We Want Things To Be
	{
		sizeof (PIXELFORMATDESCRIPTOR),									// Size Of This Pixel Format Descriptor
		1,																// Version Number
		PFD_DRAW_TO_WINDOW |											// Format Must Support Window
		PFD_SUPPORT_OPENGL |											// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,												// Must Support Double Buffering
		PFD_TYPE_RGBA,													// Request An RGBA Format
		m_bitsPerPixel,													// Select Our Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		0,																// No Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		16,																// 16Bit Z-Buffer (Depth Buffer)  
		0,																// No Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

	RECT windowRect = {0, 0, m_Width, m_Height};	// Define Our Window Coordinates

	GLuint PixelFormat;													// Will Hold The Selected Pixel Format

	if (m_isFullScreen == TRUE)								// Fullscreen Requested, Try Changing Video Modes
	{
		if (ChangeScreenResolution (m_Width, m_Height, m_bitsPerPixel) == FALSE)
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox (HWND_DESKTOP, "Mode Switch Failed.\nRunning In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);
			m_isFullScreen = FALSE;							// Set isFullscreen To False (Windowed Mode)
		}
		else															// Otherwise (If Fullscreen Mode Was Successful)
		{
			ShowCursor (FALSE);											// Turn Off The Cursor
			windowStyle = WS_POPUP;										// Set The WindowStyle To WS_POPUP (Popup Window)
			windowExtendedStyle |= WS_EX_TOPMOST;						// Set The Extended Window Style To WS_EX_TOPMOST
		}																// (Top Window Covering Everything Else)
	}
	else																// If Fullscreen Was Not Selected
	{
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
	}
	
	if( !m_wndMain.CreateEx( windowExtendedStyle , 
				  			 CLASS_NAME , 
							 WINDOW_TITLE,
							 windowStyle,
							 0,
							 0,
							 m_Width,
							 m_Height,
							 NULL,
							 NULL,
							 NULL ) )
	{
		return FALSE;
	}

	m_wndMain.SetIcon( LoadIcon(IDR_MAINFRAME) , FALSE );				// Set Application Icon

	m_pDC = m_wndMain.GetDC ();								// Grab A Device Context For This Window
	if (m_pDC == NULL)										// Did We Get A Device Context?
	{
		// Failed
		m_wndMain.DestroyWindow();										// Destroy The Window
																		// Zero The Window Handle
		return FALSE;													// Return False
	}


	PixelFormat = ChoosePixelFormat (m_pDC->m_hDC, &pfd);				// Find A Compatible Pixel Format
	if (PixelFormat == 0)												// Did We Find A Compatible Format?
	{
		// Failed
		m_wndMain.ReleaseDC(m_pDC);							// Release Our Device Context
		m_pDC = NULL;											// Zero The Device Context
		m_wndMain.DestroyWindow();										// Destroy The Window		
		return FALSE;													// Return False
	}

	if (SetPixelFormat (m_pDC->m_hDC , PixelFormat, &pfd) == FALSE)		// Try To Set The Pixel Format
	{
		// Failed
		m_wndMain.ReleaseDC(m_pDC);							// Release Our Device Context
		m_pDC = NULL;											// Zero The Device Context
		m_wndMain.DestroyWindow();										// Destroy The Window		
		return FALSE;															// Return False
	}

	m_hRC = wglCreateContext (m_pDC->m_hDC);						// Try To Get A Rendering Context
	if (m_hRC == NULL)												// Did We Get A Rendering Context?
	{
		// Failed
		m_wndMain.ReleaseDC(m_pDC);							// Release Our Device Context
		m_pDC = NULL;											// Zero The Device Context
		m_wndMain.DestroyWindow();										// Destroy The Window		
		return FALSE;													// Return False
	}

	// Make The Rendering Context Our Current Rendering Context
	if (wglMakeCurrent (m_pDC->m_hDC, m_hRC) == FALSE)
	{
		// Failed
		wglDeleteContext (m_hRC);								// Delete The Rendering Context
		m_hRC = NULL;											// Zero The Rendering Context
		m_wndMain.ReleaseDC(m_pDC);							// Release Our Device Context
		m_pDC = NULL;											// Zero The Device Context
		m_wndMain.DestroyWindow();										// Destroy The Window		
		return FALSE;													// Return False
	}
	
	ReshapeGL (m_Width, m_Height);					// Reshape Our GL Window

	ZeroMemory ( keyDown, 256);							// Clear All Keys

	m_lastTickCount = GetTickCount ();							// Get Tick Count

	return TRUE;
}

void CNeheMFCApp::DestroyOpenGLWindow()								// Destroy The OpenGL Window & Release Resources
{
	
	if (m_wndMain.m_hWnd  != 0)												// Does The Window Have A Handle?
	{	
		if (m_pDC != NULL)											// Does The Window Have A Device Context?
		{
			wglMakeCurrent (m_pDC->m_hDC, 0);							// Set The Current Active Rendering Context To Zero
			if (m_hRC != NULL)										// Does The Window Have A Rendering Context?
			{
				wglDeleteContext (m_hRC);							// Release The Rendering Context
				m_hRC = NULL;										// Zero The Rendering Context

			}
			m_wndMain.ReleaseDC (m_pDC);						// Release The Device Context
			m_pDC = NULL;										// Zero The Device Context
		}
		m_wndMain.DestroyWindow ();									// Destroy The Window		
	}

	if (m_isFullScreen)										// Is Window In Fullscreen Mode
	{
		ChangeDisplaySettings (NULL,0);									// Switch Back To Desktop Resolution
		ShowCursor (TRUE);												// Show The Cursor
	}		
}

BOOL CNeheMFCApp::RegisterWindowClass()
{
	// Register A Window Class
	WNDCLASSEX windowClass;												// Window Class
	ZeroMemory (&windowClass, sizeof (WNDCLASSEX));						// Make Sure Memory Is Cleared
	windowClass.cbSize			= sizeof (WNDCLASSEX);					// Size Of The windowClass Structure
	windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraws The Window For Any Movement / Resizing
	windowClass.lpfnWndProc		= DefWindowProc;
	windowClass.hInstance		= m_hInstance;				// Set The Instance
	windowClass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);			// Class Background Brush Color
	windowClass.hCursor			= LoadCursor(IDC_ARROW);			// Load The Arrow Pointer
	windowClass.lpszClassName	= CLASS_NAME;				// Sets The Applications Classname

	if (RegisterClassEx (&windowClass) == 0)							// Did Registering The Class Fail?
	{
		// NOTE: Failure, Should Never Happen
		MessageBox (HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;													// Return False (Failure)
	}
	return TRUE;														// Return True (Success)
}




BOOL CNeheMFCApp::ChangeScreenResolution (int width, int height, int bitsPerPixel)	// Change The Screen Resolution
{
	DEVMODE dmScreenSettings;											// Device Mode
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));					// Make Sure Memory Is Cleared
	dmScreenSettings.dmSize				= sizeof (DEVMODE);				// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth		= width;						// Select Screen Width
	dmScreenSettings.dmPelsHeight		= height;						// Select Screen Height
	dmScreenSettings.dmBitsPerPel		= bitsPerPixel;					// Select Bits Per Pixel
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;													// Display Change Failed, Return False
	}
	return TRUE;														// Display Change Was Successful, Return True
}

void CNeheMFCApp::ReshapeGL(int width, int height)						// Reshape The Window When It's Moved Or Resized
{
	glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				// Reset The Current Viewport
	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();													// Reset The Projection Matrix
	gluPerspective (45.0f, (GLfloat)(width)/(GLfloat)(height),			// Calculate The Aspect Ratio Of The Window
					1.0f, 100.0f);		
	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();													// Reset The Modelview Matrix
}


void CNeheMFCApp::TerminateApplication()
{
	PostMessage (m_wndMain.m_hWnd, WM_QUIT, 0, 0);							// Send A WM_QUIT Message
	m_isProgramLooping = FALSE;											// Stop Looping Of The Program
}

void CNeheMFCApp::ToggleFullScreen()
{	
	m_createFullScreen = !m_createFullScreen;							// toggle the boolean
	m_wndMain.PostMessage( WM_QUIT );									// send quit message
}
