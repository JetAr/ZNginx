/*
Module : PUSHPIN.H
Purpose: Implementation of a push pin button
         (as seen on X-Windows & property dialogs in VC 4)
Created: PJN / 04-04-1996
History: PJN / 08-06-1996 / Removed win32sup.h include
         PJN / 20-11-1997 / Major Update to code, changes include
                            1. Support non default color schemes
                            2. Supports dynamic changes to color schemes
                            3. Complete restructuring of code
                            4. Can now use an edged bitmap if you want to
                            5. Dropped support for Win16
                            6. Redid the example program
                            7. Simplified external usage of class
         PJN / 24-11-1997   1. Minor changes to support CPushPinFrame class
         PJN / 07-12-1997   Minor changes to fix a small redraw bug



Copyright (c) 1997 by PJ Naughter.
All rights reserved.
*/


/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "PushPin.h"




//////////////////////////////////  Macros  ///////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



////////////////////////////////// Implementation /////////////////////////////
BEGIN_MESSAGE_MAP(CPushPinButton, CButton)
    //{{AFX_MSG_MAP(CPushPinButton)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CPushPinButton::CPushPinButton()
{
    m_bPinned = FALSE;
    m_bUseEdge = TRUE;
    m_MaxRect = CRect(0, 0, 0, 0);

    LoadBitmaps();
}


void CPushPinButton::ReloadBitmaps()
{
    //free the bitmap resources
    m_PinnedBitmap.DeleteObject();
    m_UnPinnedBitmap.DeleteObject();

    //Reload the bitmaps
    LoadBitmaps();

    //size to content
    SizeToContent();

    //Invalidate the maximum rect of the pushpin on the parent window
    GetParent()->InvalidateRect(m_MaxRect);

    //Force this button to redraw aswell
    Invalidate();
}

void CPushPinButton::LoadBitmaps()
{


    BOOL bLoad = m_PinnedBitmap.Attach((HBITMAP) LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PINNED_BITMAP),
                                       IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
    ASSERT(bLoad);

    bLoad = m_UnPinnedBitmap.Attach((HBITMAP) LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_UNPINNED_BITMAP),
                                    IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
    ASSERT(bLoad);
}

/*
void CPushPinButton::LoadBitmaps()
{
  if (m_bUseEdge)
  {
    BOOL bLoad = m_PinnedBitmap.Attach((HBITMAP) LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PINNEDEDGE_BITMAP),
                                                           IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
    ASSERT(bLoad);

    bLoad = m_UnPinnedBitmap.Attach((HBITMAP) LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_UNPINNEDEDGE_BITMAP),
                                                        IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
    ASSERT(bLoad);
  }
  else
  {
    BOOL bLoad = m_PinnedBitmap.Attach((HBITMAP) LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PINNED_BITMAP),
                                                           IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
    ASSERT(bLoad);

    bLoad = m_UnPinnedBitmap.Attach((HBITMAP) LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_UNPINNED_BITMAP),
                                                        IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
    ASSERT(bLoad);
  }
}

*/


void CPushPinButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT(lpDIS != NULL);

    //select the bitmap
    CBitmap* pBitmap;
    if (m_bPinned)
        pBitmap = &m_PinnedBitmap;
    else
        pBitmap = &m_UnPinnedBitmap;

    // draw the whole button
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    CBitmap* pOld = memDC.SelectObject(pBitmap);
    if (pOld == NULL)
        return;     // destructors will clean up

    CRect rect;
    rect.CopyRect(&lpDIS->rcItem);
    pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
                &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOld);
}


void CPushPinButton::SetPinned(BOOL bPinned)
{
    m_bPinned = bPinned;
    Invalidate();
}


void CPushPinButton::SetUseEdgeBitmap(BOOL bUseEdge)
{
    if (bUseEdge == m_bUseEdge)  //quick return
        return;

    m_bUseEdge = bUseEdge;   //toggle the option, reload and
    ReloadBitmaps();         //and force a redraw
}

void CPushPinButton::PreSubclassWindow()
{
    CButton::PreSubclassWindow();

    //button must be owner draw
    ASSERT(GetWindowLong(m_hWnd, GWL_STYLE) & BS_OWNERDRAW);

    //size to content
    SizeToContent();
}


void CPushPinButton::SizeToContent()
{
    ASSERT(m_PinnedBitmap.m_hObject != NULL);
    CSize bitmapSize;
    BITMAP bmInfo;
    VERIFY(m_PinnedBitmap.GetObject(sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));

    //Calculate the client rect in parent coordinates of the maximum size of the pushpin button
    m_MaxRect = CRect(0, 0, max(bmInfo.bmWidth, m_MaxRect.Width()), max(bmInfo.bmHeight, m_MaxRect.Height()));
    ClientToScreen(&m_MaxRect);

    CPoint p1(m_MaxRect.left, m_MaxRect.top);
    CPoint p2(m_MaxRect.right, m_MaxRect.bottom);
    HWND hParent = ::GetParent(m_hWnd);
    ::ScreenToClient(hParent, &p1);
    ::ScreenToClient(hParent, &p2);
    m_MaxRect = CRect(p1, p2);

    //resize the button to match the size of the bitmap
    VERIFY(SetWindowPos(NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight,
                        SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}

void CPushPinButton::ProcessClick()
{
    m_bPinned = !m_bPinned;  //toggle the pinned option
    Invalidate();            //and force a redraw
}
