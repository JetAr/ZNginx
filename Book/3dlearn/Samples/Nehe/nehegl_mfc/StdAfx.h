// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9DA03A1D_27CB_4CA0_B0C4_6D840FA63F1B__INCLUDED_)
#define AFX_STDAFX_H__9DA03A1D_27CB_4CA0_B0C4_6D840FA63F1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <gl/gl.h>														// Header File For The OpenGL32 Library
#include <gl/glu.h>														// Header File For The GLu32 Library

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking
#pragma comment( lib, "glaux.lib" )								// Search For GLaux.lib While Linking

#define WM_TOGGLEFULLSCREEN (WM_USER+1)									// Application Define Message For Toggling

class  CNeheMFCApp;												// declare for global usage
extern CNeheMFCApp	theApp;										// declare for global usage

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9DA03A1D_27CB_4CA0_B0C4_6D840FA63F1B__INCLUDED_)
