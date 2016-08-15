#pragma once
#include <wx\window.h>
#include <wx\timer.h>

#include <Ogre.h>
using namespace Ogre;

const int FIX_FRAME_TIMTER = 10100;

class OgreWrap;

class FairySceneCanvas : public wxWindow
{
public:
	FairySceneCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxFULL_REPAINT_ON_RESIZE | wxWANTS_CHARS);
	virtual ~FairySceneCanvas(void);

	unsigned long mLastTime;
	unsigned long mTargetFPS;
	wxTimer   mTimer;


	int mDragButton;


	DECLARE_EVENT_TABLE()

	void OnPaint(wxPaintEvent &e);
	void OnEraseBackground(wxEraseEvent &e);
	void OnSize(wxSizeEvent &e);

	void OnMouseCaptureChanged(wxMouseCaptureChangedEvent& e);
	void OnMouseEnter(wxMouseEvent& e);
	void OnMouseLeave(wxMouseEvent& e);
	void OnMouseWheel(wxMouseEvent& e);
	void OnMouseMove(wxMouseEvent& e);
	void OnMiddleDown(wxMouseEvent& e);
	void OnMiddleUp(wxMouseEvent& e);
	void OnLeftDown(wxMouseEvent& e);
	void OnLeftUp(wxMouseEvent& e);

	void OnKeyDown(wxKeyEvent& e);
	void OnKeyUp(wxKeyEvent& e);
	void OnChar( wxKeyEvent &event );

	void OnContextMenu(wxContextMenuEvent& e);
	void OnDestroy(wxWindowDestroyEvent& e);
	bool IsKeyDown(int key)
	{
		return ( ::GetAsyncKeyState( key ) & (1<<15) ) != 0;
	}
	void OnTimer(wxTimerEvent& event);

	void setTargetFPS(unsigned long fps);


	OgreWrap * mogreHandle;

	BOOL InitOgre();
	Ogre::Root         *mRoot;

	inline Ogre::Root *GetOgreRoot() {return mRoot;}


};
