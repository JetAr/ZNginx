/****************************************
*   Nehe MFC Display Settings			*
*      by Yvo van Dillen				*
*             2001/2004                 *
*                                       *
*****************************************/

#include "stdafx.h"
#include "nehemfc.h"
#include "DisplaySettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


typedef	struct					//our struct for the available modes
{								//width/height/bits per pixel
	UINT dwWidth,
		 dwHeight,
		 dwBpp;
}TDisplayMode;


/////////////////////////////////////////////////////////////////////////////
// CDisplaySettingsDlg dialog


CDisplaySettingsDlg::CDisplaySettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplaySettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisplaySettingsDlg)
	m_bDontAskAgain = FALSE;
	m_bFullscreen = FALSE;
	//}}AFX_DATA_INIT
}
CDisplaySettingsDlg::~CDisplaySettingsDlg()
{
	TDisplayMode *mode;
	for(int i=0;i<m_DisplayModes.GetSize();i++ )	//destroy our array of modes
	{
		mode = (TDisplayMode*)m_DisplayModes[i];
		delete mode;
	}
	m_DisplayModes.RemoveAll();
}

void CDisplaySettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplaySettingsDlg)
	DDX_Control(pDX, IDC_RESOLUTIONS, m_cboResolutions);
	DDX_Check(pDX, IDC_DONTASKAGAIN, m_bDontAskAgain);
	DDX_Check(pDX, IDC_FULLSCREEN, m_bFullscreen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDisplaySettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CDisplaySettingsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplaySettingsDlg message handlers


// Function name	: CDisplaySettingsDlg::OnOK
// Description	    : the OK button function 
// Return type		: void 

void CDisplaySettingsDlg::OnOK() 
{	
	TDisplayMode *mode = NULL;									
	
	UpdateData();												// retreive vars
	
	SaveSettings();												// save vars (ok is pressed)

	int sel = m_cboResolutions.GetCurSel();						// get the current selection from the combobox
	if( sel >= 0 )
	{
		mode = (TDisplayMode *)m_cboResolutions.GetItemData(sel);	// get the mode stored in the itemdata (cast)	
	}

	if( mode != NULL )											// not null ? (correct selection)	
	{
		theApp.m_Width			= mode->dwWidth;				//pass vars to main app
		theApp.m_Height			= mode->dwHeight;
		theApp.m_bitsPerPixel	= mode->dwBpp ;
		theApp.m_isFullScreen   = m_bFullscreen; 

		CDialog::OnOK();
	}
	
}


// Function name	: CDisplaySettingsDlg::OnInitDialog
// Description	    : initialization of the dialog
// Return type		: BOOL 
BOOL CDisplaySettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_strIniFile = __argv[0];								// get application name
	if( m_strIniFile.ReverseFind('.') >= 0 )			    // strip the extenstion
	{
		m_strIniFile = m_strIniFile.Left( m_strIniFile.ReverseFind('.') );
	}
	m_strIniFile += ".ini";									// and paste '.ini'


	EnumDisplays();											// enumerate the display modes
	FillCombo();											// fill the combo with the fetched modes

	LoadSettings();											// load the settings
	UpdateData(FALSE);										// update vars with the loaded settings

	if( m_bDontAskAgain )									// did the user pressed 'don't ask again' the last time ?
	{
		OnOK();												// yup.. bail
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// Function name	: CDisplaySettingsDlg::ModeExists
// Description	    : checks if the mode already exists in our array
//
//					  this is function is added because some nVidia-cards (GF2)
//					  give back 8 of the same resolutions (yup... it's strange)
//
// Return type		: BOOL 
// Argument         : DEVMODE *dm
BOOL CDisplaySettingsDlg::ModeExists(DEVMODE *dm)
{
	TDisplayMode *mode;
	for(int i=0;i<m_DisplayModes.GetSize();i++ )
	{
		mode = (TDisplayMode*)m_DisplayModes[i];
		
		if( mode->dwBpp == dm->dmBitsPerPel &&
			mode->dwWidth == dm->dmPelsWidth && 
			mode->dwHeight == dm->dmPelsHeight )
			return TRUE;
	}
	return FALSE;
}


// Function name	: CDisplaySettingsDlg::EnumDisplays
// Description	    : loop all displaymodes
// Return type		: void 
void CDisplaySettingsDlg::EnumDisplays()
{
	DWORD dwMode=0;
	DEVMODE	dm;
	TDisplayMode *mode;

	while( ::EnumDisplaySettings( NULL , dwMode , &dm ) )		//loop modes
	{
		if( !ModeExists( &dm ) )									// are we missing this mode ?
		{
			if( dm.dmBitsPerPel == 16 ||							// 16 or 32 bits ?
				dm.dmBitsPerPel == 32 )
			{
				if( dm.dmPelsWidth >= 640 )							// resolution >= 640 x 480 ?
				{
					mode = new TDisplayMode;						// add it...
					if( mode )
					{
						mode->dwWidth  = dm.dmPelsWidth;
						mode->dwHeight = dm.dmPelsHeight;
						mode->dwBpp    = dm.dmBitsPerPel;

						m_DisplayModes.Add( mode );
					}
				}
			}
		}
		dwMode++;
	}	
}


// Function name	: CDisplaySettingsDlg::FillCombo
// Description	    : Fills the combobox with the previously fetched displaymodes
// Return type		: void 
void CDisplaySettingsDlg::FillCombo()
{
	int item;
	CString str;
	TDisplayMode *mode;

	for(int i=0;i<m_DisplayModes.GetSize();i++ )			// loop modes
	{
		mode = (TDisplayMode*)m_DisplayModes[i];
		str.Format("%d x %d x %d",mode->dwWidth,mode->dwHeight,mode->dwBpp);	//format modes
		item = m_cboResolutions.AddString( str );								//add it to the combo
		m_cboResolutions.SetItemData( item , (DWORD)mode );						//cast it to the itemdata
	}

	if( m_cboResolutions.GetCount() <= 0 )										//do we have <=0 entries ?
	{
		item = m_cboResolutions.AddString("Not supported.");					//your videocard can't do anything ?!
		m_cboResolutions.EnableWindow( FALSE );									
	}

	m_cboResolutions.SetCurSel( 0 );											

}


// Function name	: CDisplaySettingsDlg::LoadSettings
// Description	    : Loads the settings from the current inifile
// Return type		: void 
void CDisplaySettingsDlg::LoadSettings()
{
	UINT width,height,bpp;

	width  = GetPrivateProfileInt("Display Settings" , "Width"  , 0 , m_strIniFile );	//load the settings from the inifile
	height = GetPrivateProfileInt("Display Settings" , "Height" , 0 , m_strIniFile );
	bpp    = GetPrivateProfileInt("Display Settings" , "BPP"    , 0 , m_strIniFile );

	m_bFullscreen   = GetPrivateProfileInt("Display Settings" , "Fullscreen"      , 1 , m_strIniFile );
	m_bDontAskAgain = GetPrivateProfileInt("Display Settings" , "DontAskAgain"    , 0 , m_strIniFile );

	int i;
	BOOL bFound = FALSE;

	if( m_cboResolutions.GetCount() > 0 )			//now set the current selection matching to the loaded settings
	{
		m_cboResolutions.SetCurSel( 0 );
		for(i=0;i<m_cboResolutions.GetCount();i++)
		{
			TDisplayMode *mode = (TDisplayMode *)m_cboResolutions.GetItemData(i);
			if( width  == mode->dwWidth &&
				height == mode->dwHeight &&
				bpp    == mode->dwBpp )
			{
				m_cboResolutions.SetCurSel( i );
				bFound = TRUE;
				break;
			}
		}
	}	

	if( !bFound )
		m_bDontAskAgain = 0;		//make sure the 'don't ask again'-check is flagged out (if not found)
}



// Function name	: CDisplaySettingsDlg::SaveSettings
// Description	    : Saves the current settings to the ini files
// Return type		: void 
void CDisplaySettingsDlg::SaveSettings( )
{
	TDisplayMode *mode = NULL;
	int sel = m_cboResolutions.GetCurSel();
	if( sel >= 0 )
	{
		mode = (TDisplayMode *)m_cboResolutions.GetItemData(sel);		
	}

	if( mode == NULL )return;

	WritePrivateProfileString("Display Settings" , "Width"  , WordToString( mode->dwWidth  ) , m_strIniFile );
	WritePrivateProfileString("Display Settings" , "Height" , WordToString( mode->dwHeight ) , m_strIniFile );
	WritePrivateProfileString("Display Settings" , "BPP"    , WordToString( mode->dwBpp    ) , m_strIniFile );

	WritePrivateProfileString("Display Settings" , "Fullscreen"    , WordToString( m_bFullscreen ) , m_strIniFile );
	WritePrivateProfileString("Display Settings" , "DontAskAgain"  , WordToString( m_bDontAskAgain ) , m_strIniFile );
}


// Function name	: CDisplaySettingsDlg::WordToString
// Description	    : Quick conversion from a unsigned integer to a string
// Return type		: CString 
// Argument         : UINT uValue
CString CDisplaySettingsDlg::WordToString(UINT uValue)
{
	CString str;
	str.Format("%u",uValue );
	return str;
}


