#ifndef __ElApplication_H__
#define __ElApplication_H__

#include "ElCamera.h"
#include "ElSceneManager.h"
#include "ElScreenElement.h"

class ElApplication
{
public:
	ElApplication();
	virtual ~ElApplication();

	virtual bool initialize(HWND wnd);
	virtual void terminate();

	virtual void process();
	virtual void beginFrame();
	
	virtual void beginUpdate();
	virtual void update();
	virtual void endUpdate();

	virtual void beginRender();
	virtual void render();
	virtual void endRender();

	virtual void renderScene();
	virtual void renderScreenItems();

	virtual void displayFrame();
	virtual void endFrame();

	virtual void setPause(bool pause);
	virtual bool getPause() const;
	virtual TimeValue getCurrentTime(bool ignorePaused = false) const;

	virtual ElCamera* getCamera();
	virtual ElSceneManager* getScene();
	
	virtual DWORD getFillMode() const;
	virtual void setFillMode(DWORD fm);

	virtual void addScreenElement(ElScreenElement* element);

	virtual HWND getReferenceWindow() const;

protected:
	virtual bool createDevice();
	virtual bool createCamera();
	virtual bool createScene();

	virtual bool _measureTime();

	virtual bool _setScreenSpaceCamera();
	virtual void _useScreenSpaceCamera();
	virtual void _renderScreenElements();

protected:
	ElCamera*			mCamera;
	ElSceneManager*		mScene;

	bool				mZDefaultEnable;
	DWORD				mFillMode;

	TimeValue			mLastTime;
	TimeValue			mCurrentTime;
	TimeValue			mFrameTime;
	TimeValue			mAccumTime;
	
	bool				mPause;
	TimeValue			mAccumTimeWithPause;

	// Frame rate limitation
	TimeValue			mMinFramePeriod;
	TimeValue			mLastFrame;

	ElCamera*			mOrthoCamera;
	ElScreenElementList	mScreenElements;

	HWND				mReferenceWindow;
};

#endif //__ElApplication_H__