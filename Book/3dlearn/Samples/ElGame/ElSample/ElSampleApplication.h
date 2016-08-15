#ifndef __ElSampleApplication_H__
#define __ElSampleApplication_H__

#include "ElApplicationHeaders.h"
#include "ElFps.h"
#include "ElBrush.h"
#include "ElAvatar.h"
#include "ElNameBoard.h"
#include "ELGUI.h"

class ElSampleApplication : public ElApplication
{
public:
	ElSampleApplication();

	virtual void terminate();
	virtual void update();
	virtual void renderScene();
	virtual void endRender();

	void pickTerrain(float screenx, float screeny);
	void pickEntities(float screenx, float screeny);
	void toggleFillMode();
	void toggleCamera();
	void saveScreen();

	bool OnGUIEventSheetMouseMove(const ELGUI::EventArgs& args);
	bool OnGUIEventSheetLButtonDown(const ELGUI::EventArgs& args);

	bool OnGUIEventToggleFpsWnd(const ELGUI::EventArgs& args);
	bool OnGUIEventToggleSatyrPortrait(const ELGUI::EventArgs& args);
	bool OnGUIEventToggleOrcWnd(const ELGUI::EventArgs& args);
	
	bool OnGUIEventPause(const ELGUI::EventArgs& args);
	bool OnGUIEventResume(const ELGUI::EventArgs& args);

	bool OnGUIEventMouseEnter(const ELGUI::EventArgs& args);
	bool OnGUIEventMouseLeave(const ELGUI::EventArgs& args);

protected:
	virtual bool createScene();

	bool _setupContent();
	void _cleanupContent();
	bool _setupEntities();
	bool _setupRPGCamera();
	void _updateRPGCamera();
	bool _setupGUI();
	bool _setupGUIWidgets();
	bool _setupGUICursor();
	void _cleanupGUI();

	virtual void _renderScreenElements();

	void _saveScreenImp(LPCSTR directory);

protected:
	D3DXVECTOR3			mTerrainIntersectedPoint;
	ElMovableObject*	mSceneIntersectedObject;

	bool				mSaveScreenAfterRendered;

	ElBrush				mBrush;

	ElNameboard			mSatyrNameboard;
	ElNameboard			mTigerNameboard;

	ElCamera*			mRPGCamera;
	ElSceneNode*		mCameraPivot;
	ElSceneNode*		mCameraGoal;
	ElSceneNode*		mCameraNode;
	ElSceneNode*		mFirstPersonNode;

	ELGUI::Renderer*	mGUIRenderer;
	ELGUI::System*		mGUISystem;

	ElAvatar			mAvatar;
	ELGUI::StaticImage*	mAvatarWindow;
	ElFps				mFps;
	ELGUI::StaticText*	mFpsWindow;
};

#endif //__ElSampleApplication_H__