/*
Module : PUSHPIN.H
Purpose: Interface of a push pin button 
         (as seen on X-Windows & property dialogs in VC 4)
Created: PJN / DATE/1 / 04-04-1996
History: None

Copyright (c) 1997 by PJ Naughter.  
All rights reserved.

*/

#ifndef __PUSHPIN_H__
#define __PUSHPIN_H__


////////////////////////////////// Includes ///////////////////////////////////


class CPushPinButton : public CButton
{
public:
	void ProcessClick();
//standard constructor
  CPushPinButton();

//accessors & mutators for the pinned state
  void SetPinned(BOOL bPinned);
  BOOL IsPinned() { return m_bPinned; };

//should be called in response to system color changes
  void ReloadBitmaps(); 

//Use an edged version of the bitmap
  void SetUseEdgeBitmap(BOOL bUseEdge);
  BOOL GetUseEdgeBitmap() const { return m_bUseEdge; };


protected:
  //{{AFX_MSG(CPushPinButton)
  //}}AFX_MSG

  //{{AFX_VIRTUAL(CPushPinButton)
	public:
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

  void SizeToContent();
  void LoadBitmaps();

  DECLARE_MESSAGE_MAP()
  BOOL    m_bPinned;
  BOOL    m_bUseEdge;
  CBitmap m_PinnedBitmap;
  CBitmap m_UnPinnedBitmap;
  CRect   m_MaxRect;
};

#endif //__PUSHPIN_H__