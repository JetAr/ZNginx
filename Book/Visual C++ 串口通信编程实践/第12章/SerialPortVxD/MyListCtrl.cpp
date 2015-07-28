// MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "serialportvxd.h"
#include "MyListCtrl.h"
#include "SerialPortVxDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

CMyListCtrl::CMyListCtrl()
{
}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
    //{{AFX_MSG_MAP(CMyListCtrl)
    ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
    ON_WM_MEASUREITEM_REFLECT()
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl message handlers

void CMyListCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    // TODO: Add your control notification handler code here
    delete (CListRecord *)pNMListView->lParam;
    *pResult = 0;
}

void CMyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    CFont *oldFont = dc.SelectObject(&((CSerialPortVxDDlg *)GetParent())->m_fontTrace);
    CSize te = dc.GetTextExtent( "0", 1 );
    lpMeasureItemStruct->itemHeight = ((CSerialPortVxDDlg *)GetParent())->m_nCharHeight = te.cy - 2;
    ((CSerialPortVxDDlg *)GetParent())->m_nCharWidth = te.cx;
    lpMeasureItemStruct->itemWidth = ((CSerialPortVxDDlg *)GetParent())->m_nCharWidth * 95;
}

void CMyListCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    int index = lpDrawItemStruct->itemID;
    TRACE("%d\n", index );
    if ( index < 0 )
        return;

    int y = lpDrawItemStruct->rcItem.top;

    HDC hDC = lpDrawItemStruct->hDC;
    HFONT oldFont = (HFONT)::SelectObject( hDC, (HFONT)((CSerialPortVxDDlg *)GetParent())->m_fontTrace );
    int oldBkMode = ::SetBkMode( hDC, OPAQUE );
    COLORREF oldTextColor = ::SetTextColor( hDC, RGB(0,0,0) );
    COLORREF oldBkColor = ::SetBkColor( hDC, RGB(255,255,255) );

    CListRecord *pRec = (CListRecord *)lpDrawItemStruct->itemData;
    CString str;
    for ( int i=0; i<(int)pRec->m_dwLength; i++ )
    {
        if ( pRec->m_pwData[i] & 0xFF00 )
        {
            ::SetTextColor( hDC, RGB(0,0,0) );
            ::SetBkColor( hDC, RGB(255,255,255) );
        }
        else
        {
            ::SetTextColor( hDC, RGB(255,255,255) );
            ::SetBkColor( hDC, RGB(0,0,0) );
        }
        str.Format( "%2.2X ", pRec->m_pwData[i] & 0x00FF );
        ::TextOut( hDC, ((CSerialPortVxDDlg *)GetParent())->m_nCharWidth*3*i, y, str, 3 );
    }
    ::SetTextColor( hDC, RGB(0,0,0) );
    ::SetBkColor( hDC, RGB(255,0,0) );
    for ( ; i<32; i++ )
        ::TextOut( hDC, ((CSerialPortVxDDlg *)GetParent())->m_nCharWidth*3*i, y, "   ", 3 );

    ::SetTextColor( hDC, oldTextColor );
    ::SetBkColor( hDC, oldBkColor );
    ::SetBkMode( hDC, oldBkMode );
    ::SelectObject( hDC, oldFont );
}


BOOL CMyListCtrl::OnEraseBkgnd(CDC* pDC)
{
    if ( ! GetItemCount() )
        return CListCtrl::OnEraseBkgnd( pDC );
    return TRUE;
}