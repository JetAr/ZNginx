﻿#ifndef _MAIN_H
#define _MAIN_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++
// is determining if they are going to add them to the official language set.  Until then,
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>					// Include the much need windows.h
#include <gl\gl.h>						// Header File For The OpenGL32 Library
#include <gl\glu.h>						// Header File For The GLu32 Library
#include <iostream>						// Include our c++ standard header
#include <fstream>						// Include this to use ifstream
#include <string>						// Include this to use the string class
#include <vector>						// Include this to use STL vectors
#include <mmsystem.h>					// Include this to use timeGetTime()
#include "image.h"
using namespace std;					// Start off with the standard namespace

#define SCREEN_WIDTH  640				// We want our screen width 640 pixels
#define SCREEN_HEIGHT 480				// We want our screen height 480 pixels
#define SCREEN_DEPTH 16					// We want 16 bits per pixel

#define MAX_TEXTURES 1000				// The maximum amount of textures to load

extern UINT g_Texture[MAX_TEXTURES];	// Our global texture array

typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);

extern PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC  glClientActiveTextureARB;

#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1


// This is our basic 3D point/vector class
struct CVector3
{
public:

    // A default constructor
    CVector3() {}

    // This is our constructor that allows us to initialize our data upon creating an instance
    CVector3(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    // Here we overload the + operator so we can add vectors together
    CVector3 operator+(CVector3 vVector)
    {
        // Return the added vectors result.
        return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
    }

    // Here we overload the - operator so we can subtract vectors
    CVector3 operator-(CVector3 vVector)
    {
        // Return the subtracted vectors result
        return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
    }

    // Here we overload the * operator so we can multiply by scalars
    CVector3 operator*(float num)
    {
        // Return the scaled vector
        return CVector3(x * num, y * num, z * num);
    }

    // Here we overload the / operator so we can divide by a scalar
    CVector3 operator/(float num)
    {
        // Return the scale vector
        return CVector3(x / num, y / num, z / num);
    }

    float x, y, z;
};


// This is our basic 2D point class.  This will be used to store the UV coordinates.
class CVector2
{
public:

    // A default constructor
    CVector2() {}

    // This is our constructor that allows us to initialize our data upon creating an instance
    CVector2(float X, float Y)
    {
        x = X;
        y = Y;
    }

    // Here we overload the + operator so we can add vectors together
    CVector2 operator+(CVector2 vVector)
    {
        // Return the added vectors result.
        return CVector2(vVector.x + x, vVector.y + y);
    }

    // Here we overload the - operator so we can subtract vectors
    CVector2 operator-(CVector2 vVector)
    {
        // Return the subtracted vectors result
        return CVector2(x - vVector.x, y - vVector.y);
    }

    // Here we overload the * operator so we can multiply by scalars
    CVector2 operator*(float num)
    {
        // Return the scaled vector
        return CVector2(x * num, y * num);
    }

    // Here we overload the / operator so we can divide by a scalar
    CVector2 operator/(float num)
    {
        // Return the scale vector
        return CVector2(x / num, y / num);
    }

    float x, y;
};


// Extern our global variables so other source files can use them
extern bool  g_bFullScreen;									// Set full screen as default
extern HWND  g_hWnd;										// This is the handle for the window
extern RECT  g_rRect;										// This holds the window dimensions
extern HDC   g_hDC;											// General HDC - (handle to device context)
extern HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;								// This holds our window hInstance

extern double g_FrameInterval;

// This is our MAIN() for windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

// The window proc which handles all of window's messages.
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// This controls our main program loop
WPARAM MainLoop();

// This creates a texture and stores it in the texture array with it's ID.
bool CreateTexture(UINT &texture, LPSTR strFileName);

// This changes the screen to full screen mode
void ChangeToFullScreen();

// This is our own function that makes creating a window modular and easy
HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);

// This allows us to configure our window for OpenGL and backbuffering
bool bSetupPixelFormat(HDC hdc);

// This inits our screen translations and projections
void SizeOpenGLScreen(int width, int height);

// This sets up OpenGL
void InitializeOpenGL(int width, int height);

// This initializes the whole program
void Init(HWND hWnd);

// This draws everything to the screen
void RenderScene();

// This frees all our memory in our program
void DeInit();
#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// There was nothing added to this file in this version.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
