// MFC_appView.cpp : implementation of the CMFC_appView class
//

#include "stdafx.h"
#include "MFC_app.h"

#include "MFC_appDoc.h"
#include "MFC_appView.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutOpenGL.h"
#include "GutDX9.h"

#include "render_opengl.h"
#include "render_dx9.h"

#ifdef _ENABLE_DX10_
#include "render_dx10.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFC_appView
IMPLEMENT_DYNCREATE(CMFC_appView, CView)

BEGIN_MESSAGE_MAP(CMFC_appView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

bool (*g_init_resource)(void) = InitResourceDX9;
bool (*g_release_resource)(void) = ReleaseResourceDX9;
void (*g_render)(void) = RenderFrameDX9;
void (*g_onsize)(int w, int h) = ResizeWindowDX9;

// CMFC_appView construction/destruction

CMFC_appView::CMFC_appView()
{
	// TODO: add construction code here

}

CMFC_appView::~CMFC_appView()
{
}

BOOL CMFC_appView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	cs.style |= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	return CView::PreCreateWindow(cs);
}

// CMFC_appView drawing

void CMFC_appView::OnDraw(CDC* /*pDC*/)
{
	CMFC_appDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	g_render();
}


// CMFC_appView printing

BOOL CMFC_appView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFC_appView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFC_appView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CMFC_appView diagnostics

#ifdef _DEBUG
void CMFC_appView::AssertValid() const
{
	CView::AssertValid();
}

void CMFC_appView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFC_appDoc* CMFC_appView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFC_appDoc)));
	return (CMFC_appDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFC_appView message handlers

char *g_device = NULL;	
bool g_bDeviceReady = false;

int CMFC_appView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	GutSetWindowHandleWin32(m_hWnd);
	int device = 1;

	switch(device)
	{
	default:
	case 1:
		g_device = "dx9";
		g_render = RenderFrameDX9;
		g_onsize = ResizeWindowDX9;
		g_init_resource = InitResourceDX9;
		g_release_resource = ReleaseResourceDX9;
		break;
	case 2:
		g_device = "opengl";
		g_init_resource = InitResourceOpenGL;
		g_release_resource = ReleaseResourceOpenGL;
		g_render = RenderFrameOpenGL;
		g_onsize = ResizeWindowOpenGL;
		break;
	case 3:
#ifdef _ENABLE_DX10_
		g_device = "dx10";
		g_init_resource = InitResourceDX10;
		g_release_resource = ReleaseResourceDX10;
		g_render = RenderFrameDX10;
		g_onsize = ResizeWindowDX10;
#endif
		break;
	}

	g_bDeviceReady = GutInitGraphicsDevice(g_device);
	if ( g_bDeviceReady )
	{
		g_init_resource();
		SetTimer(1, 15, NULL);
	}

	return 0;
}

void CMFC_appView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if ( !g_bDeviceReady )
	{
		g_bDeviceReady = GutInitGraphicsDevice(g_device);
		if ( g_bDeviceReady )
		{
			g_init_resource();
			SetTimer(1, 16, NULL);
		}
	}


	if ( cx && cy )
	{
		g_onsize(cx, cy);
	}
}

void CMFC_appView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnTimer(nIDEvent);

	OnDraw(NULL);
}

void CMFC_appView::OnDestroy()
{
	CView::OnDestroy();

	KillTimer(1);
	g_release_resource();
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
