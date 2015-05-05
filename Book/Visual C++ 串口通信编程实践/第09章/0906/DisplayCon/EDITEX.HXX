// EditEx.hxx : header file
//
// Copyright (C) 1996 Bill Berry All rights reserved.
//
// This class is freely distributable as long as the copyright accompanies 
// the header and implementaion sources.
//
// Also, please send me any improvements or bug fixes to bberry@javanet.com
//
// Modification History:
//
// Bill Berry			November 1996			Created
//                      March    1998           Update
//
// Description:
//
//  Extended CEdit class. Allows easy customization of the following:
//
//      1. COLORREF bkColor( COLORREF crColor )
//         - Sets back ground color of the control
//
//      2. COLORREF bkColor()
//         - Returns back ground color of control
//
//      3. COLORREF textColor( COLORREF crColor )
//         - Sets text or foreground color of the control
//
//      4. COLORREF textColor() const
//         - Returns text (or foreground) color of control
//
//      5. void setCustomMask( CEditMask* editMask /* NULL means default state */ )
//
//      6. void definePopupMenu( UINT uResourceID = 0 /* 0 uses default */ )
//         - Overide default context menu with new menu resource.
//
//      *** Set new font for this control ***
//
//      7. void setFont( const LOGFONT* lpLogFont );
//  
//      8. void setFont( LONG fontHeight      = -8, 
//                       LONG fontWeight      = FW_NORMAL,
//                       UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
//                       LPCSTR faceName      = _T("MS Sans Serif"          
//                     );
//
// Remarks:
//
//      When using the CEditEx control do not free the CEditMask pointer
//      assigned by the setCustomMask call. This class will free it for 
//      you once it leaves scope.
//
// Declares CLogFont - implementation file is not defined for this class.
//
// Discussion:
//
//  This is a subclass of the LOGFONT structure and defines 
//  the attributes of a font. See the Microsoft documentation 
//  for more information.
//
// Modification History:
//
//  Bill Berry      March 1996      Creation
//
//  
// Copyright (C) Bill Berry 1996 
//
// This class may be used freely. Please distribute with its
// copyright notice in place.
//
#ifndef LOGFONT_H_E7F7D1A0_AE36_11d1_9257_006097CAB461
#define LOGFONT_H_E7F7D1A0_AE36_11d1_9257_006097CAB461

#ifndef __cplusplus
#   error C++ compilation is required.
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

struct CLogFont : public LOGFONT
{
    CLogFont( LONG fontHeight      = -8, 
              LONG fontWeight      = FW_NORMAL,
              UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
              LPCSTR faceName      = _T("MS Sans Serif" )
            )
    {
        const int size = sizeof(*this);

        memset( this, 0, size );

        lfHeight         = fontHeight;
        lfWeight         = fontWeight;
        lfPitchAndFamily = pitchAndFamily;

        _ASSERT( LF_FACESIZE > lstrlen( faceName ) );

        lstrcpy( lfFaceName, faceName );
    }

    // Note: No need for CLogFont& operator =(const CLogFont& lf) {...}
    //       We let the compiler handle it...

    void lfFaceNameSet( LPCSTR faceName )
    {
        _ASSERT( faceName );

        if ( !faceName ) return;

        _ASSERT( LF_FACESIZE > lstrlen( faceName ) );

        lstrcpy( lfFaceName, faceName );        
    }

};

#endif // LOGFONT_H_E7F7D1A0_AE36_11d1_9257_006097CAB461

#if !defined(AFX_EDITEX_H__B3788DB1_4FA1_11D1_9FB8_006097B531B3__INCLUDED_)
#define AFX_EDITEX_H__B3788DB1_4FA1_11D1_9FB8_006097B531B3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CEditEx window

class CEditMask
{
public:
    virtual BOOL AddChar( UINT nChar ) = 0;
};


class CEditEx : public CEdit
{
// Construction
public:
	CEditEx();

    DECLARE_DYNCREATE( CEditEx )

// Copy Semantics

   // Block copy construction...
   //
   // No m_hWnd defined for object. 
   //
private:
   CEditEx( const CEditEx& ); 

public:
   // Allow basics to be copied...
   CEditEx& operator = ( const CEditEx& );

// Attributes
public:

    void setCustomMask( CEditMask* editMask )
    {
        if ( m_pEditMask ) delete m_pEditMask;
        m_pEditMask = editMask;
    } 

    COLORREF bkColor  ( COLORREF );    
    COLORREF textColor( COLORREF );

    COLORREF bkColor()   const { return m_crBkColor;   }   
    COLORREF textColor() const { return m_crTextColor; }

    void setFont( const LOGFONT* lpLogFont );

    void setFont( LONG fontHeight      = -8, 
                  LONG fontWeight      = FW_NORMAL,
                  UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
                  LPCSTR faceName      = _T("MS Sans Serif" ) );
    
    void definePopupMenu( UINT uResourceID = 0 /* 0 uses default */ )
    {
        m_MenuResourceID = uResourceID;
    }

// Operations:
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditEx)
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

    // Customize your brush
    //
    virtual BOOL CreateBrushType();

private:
    CBrush   m_brBkGround;
    COLORREF m_crBkColor;
    COLORREF m_crTextColor;

    CEditMask* m_pEditMask;

    CFont* m_pCFont;

    UINT m_MenuResourceID;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITEX_H__B3788DB1_4FA1_11D1_9FB8_006097B531B3__INCLUDED_)
