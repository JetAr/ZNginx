// ChildView.h : interface of the CChildView class
//


#pragma once

#include "Scene.h"

// CChildView window

class CChildView : public CWnd
{
	// Construction
public:
	CChildView();

	// Attributes
public:

	// Operations
public:

	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
	void SetupOgre(void);
	void CreateScene(void);
	void CreateSceneManager(void);
	void CreateCamera(void);
	void CreateViewPort(CRect& rect);
	void loadResources(void);
private:
	CScene          m_scene;            // Out scene

	bool            m_firstDraw;

	Ogre::Root          *m_OgreRoot;
	Ogre::SceneManager  *m_SceneManager;
	Ogre::Camera        *m_Camera;
	Ogre::RenderWindow  *m_Window;

	CPoint              m_mouseLast;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRobotWalk();
	afx_msg void OnUpdateRobotWalk(CCmdUI *pCmdUI);
	afx_msg void OnRobotShoot();
	afx_msg void OnUpdateRobotShoot(CCmdUI *pCmdUI);
	afx_msg void OnRobotIdle();
	afx_msg void OnUpdateRobotIdle(CCmdUI *pCmdUI);
	afx_msg void OnRobotSlump();
	afx_msg void OnUpdateRobotSlump(CCmdUI *pCmdUI);
	afx_msg void OnRobotDie();
	afx_msg void OnUpdateRobotDie(CCmdUI *pCmdUI);
};

