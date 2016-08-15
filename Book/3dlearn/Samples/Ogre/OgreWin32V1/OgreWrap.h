#pragma once

#include "Scene.h"
class OgreWrap
{
public:
	OgreWrap(void);
	virtual ~OgreWrap(void);

	void SetupOgre(HWND hWnd);
	void CreateScene(void);
	void CreateSceneManager(void);
	void CreateCamera(void);
	void CreateViewPort(RECT& rect);
	void loadResources(void);
private:

	Ogre::Root          *m_OgreRoot;
	Ogre::SceneManager  *m_SceneManager;
	Ogre::Camera        *m_Camera;

	Ogre::RenderWindow  *m_Window;

	CScene          m_scene;            // Out scene

public:
	void SetRobotState(int state);

	void OnSize();

};
