﻿#ifndef _MAIN_H
#define _MAIN_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++
// is determining if they are going to add them to the official language set.  Until then,
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#pragma warning(disable: 4786)							// This disables vector<> string warnings

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <crtdbg.h>
using namespace std;

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

#define MAX_TEXTURES 100								// The maximum amount of textures to load


// This is our 3D point class.  This will be used to store the vertices of our model.
class CVector3
{
public:
    float x, y, z;
};

// This is our 2D point class.  This will be used to store the UV coordinates.
class CVector2
{
public:
    float x, y;
};

// This is our face structure.  This is used for indexing into the vertex
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace
{
    int vertIndex[3];			// indices for the verts that make up this triangle
    int coordIndex[3];			// indices for the tex coords to texture this face
};

// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them.
struct tMaterialInfo
{
    char  strName[255];			// The texture name
    char  strFile[255];			// The texture file name (If this is set it's a texture map)
    BYTE  color[3];				// The color of the object (R, G, B)
    int   texureId;				// The texture ID
    float uTile;				// u tiling of texture
    float vTile;				// v tiling of texture
    float uOffset;			    // u offset of texture
    float vOffset;				// v offset of texture
} ;


// This holds all the information for our model/scene.
// You should eventually turn into a robust class that
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject
{
    int  numOfVerts;			// The number of verts in the model
    int  numOfFaces;			// The number of faces in the model
    int  numTexVertex;			// The number of texture coordinates
    int  materialID;			// The texture ID to use, which is the index into our texture array
    bool bHasTexture;			// This is TRUE if there is a texture map for this object
    char strName[255];			// The name of the object
    CVector3  *pVerts;			// The object's vertices
    CVector3  *pNormals;		// The object's normals
    CVector2  *pTexVerts;		// The texture's UV coordinates
    tFace *pFaces;				// The faces information of the object
};


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This holds our information for each animation of the Quake model.
// A STL vector list of this structure is created in our t3DModel structure below.
struct tAnimationInfo
{
    char strName[255];			// This stores the name of the animation (i.e. "TORSO_STAND")
    int startFrame;				// This stores the first frame number for this animation
    int endFrame;				// This stores the last frame number for this animation
    int loopingFrames;			// This stores the looping frames for this animation (not used)
    int framesPerSecond;		// This stores the frames per second that this animation runs
};

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// This our model structure
struct t3DModel
{
    int numOfObjects;					// The number of objects in the model
    int numOfMaterials;					// The number of materials for the model
    vector<tMaterialInfo> pMaterials;	// The list of material information (Textures and colors)
    vector<t3DObject> pObject;			// The object list for our model


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

    int numOfAnimations;				// The number of animations in this model
    int currentAnim;					// The current index into pAnimations list
    int currentFrame;					// The current frame of the current animation
    int nextFrame;						// The next frame of animation to interpolate too
    float t;							// The ratio of 0.0f to 1.0f between each key frame
    float lastTime;						// This stores the last time that was stored
    vector<tAnimationInfo> pAnimations; // The list of animations

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


    int numOfTags;						// This stores the number of tags in the model
    t3DModel			**pLinks;		// This stores a list of pointers that are linked to this model
    struct tMd3Tag		*pTags;			// This stores all the tags for the model animations
};


extern bool  g_bFullScreen;									// Set full screen as default
extern HWND  g_hWnd;										// This is the handle for the window
extern RECT  g_rRect;										// This holds the window dimensions
extern HDC   g_hDC;											// General HDC - (handle to device context)
extern HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;								// This holds our window hInstance

// This is our MAIN() for windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

// The window proc which handles all of window's messages.
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// This controls our main program loop
WPARAM MainLoop();

// This creates a texture and stores it in the texture array with it's ID.
bool CreateTexture(GLuint &textureID, LPTSTR szFileName);

// This changes the screen to full screen mode
void ChangeToFullScreen();

// This is our own function that makes creating a window modular and easy
HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);

// This allows us to configure our window for OpenGL and back buffered
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
// This file stores all the structures needed to store and animate a Quake3 character.
// We added the "tAnimationInfo" structure to our t3DModel class, along with a bunch
// of variables to control the current frame, the next frame, the current time and the
// last time.  This is so we can have multiple animations going on at the same time.
//
// The Quake3 .Md3 file format is owned by ID Software.  This tutorial is being used
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written consent
// from ID Software.
//
// Quake, Quake2 and Quake3 are trademarks of ID Software.
// All trademarks used are properties of their respective owners.
//
//
// © 2000-2005 GameTutorials
