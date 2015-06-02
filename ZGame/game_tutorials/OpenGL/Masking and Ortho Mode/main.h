﻿#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// Since we are using a full screen picture for the scope, we should bring down our
// resolution so we don't have a much larger file in texture memory.
#define SCREEN_WIDTH 640								// We want our screen width 640 pixels
#define SCREEN_HEIGHT 480								// We want our screen height 480 pixels

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#define SCREEN_DEPTH 16									// We want 16 bits per pixel


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// We increase our texture array to 4 textures (Wall, floor, CrossHair and CrossHairMask)
#define MAX_TEXTURES 4									// This says how many texture we will be using

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


extern bool  g_bFullScreen;								// Set full screen as default
extern HWND  g_hWnd;									// This is the handle for the window
extern RECT  g_rRect;									// This holds the window dimensions
extern HDC   g_hDC;										// General HDC - (handle to device context)
extern HGLRC g_hRC;										// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;							// This holds our window hInstance


extern UINT g_Texture[MAX_TEXTURES];					// This is our texture data array

// This is our MAIN() for windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

// The window proc which handles all of window's messages.
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// This controls our main program loop
WPARAM MainLoop();

// This loads a texture into openGL from a file (IE, "bitmap.bmp")
bool CreateTexture(GLuint &textureID, LPTSTR szFileName);

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
// Nothing new was added to this file, except we increase the MAX_TEXTURES to 4,
// and changed our resolution from 800 by 600 to 640 by 480.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
