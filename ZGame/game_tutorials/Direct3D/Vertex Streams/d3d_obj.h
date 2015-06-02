﻿#ifndef D3D_OBJ_H
#define D3D_OBJ_H

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include <assert.h>

// A position in 3D space
class CPos
{
public:

    CPos()
    {
        /* do nothing */
    }
    CPos(float X, float Y, float Z) : x(X), y(Y), z(Z)
    {
        /* do nothing */
    }

    // Public data
    float x, y, z;
};

// A 2D texture coordinate
class CTexCoord
{
public:

    CTexCoord()
    {
        /* do nothing */
    }
    CTexCoord(float U, float V) : u(U), v(V)
    {
        /* do nothing */
    }

    // Public data
    float u, v;
};

// Our 3D Object that allows for rendering and setting
// projection, view, and world matrices
class CD3DObj
{
public:

    CD3DObj()
    {
        /* do nothing */
    } // Constructor

    static bool init(HWND hwnd); // Initializes our 3D Object

    static void begin(); // Begins a scene
    static void end(); // Ends a scene

    // Sets all rendering states such as z-buffering, culling, etc
    // to a default state
    static void setDefaultRenderState();

    // Goes between windowed and full screen
    static bool toggleFullScreen();

    // Functions to set either the projection, view, or world matrix
    static void setProjMatrix(float fov, float aspectRatio, float nearClip, float farClip);
    static void setViewMatrix(const CPos &eye, const CPos &target);
    static void setWorldMatrix(const D3DXMATRIX *matrix);

    // Clears the viewport color and z-buffer
    static bool clear(int color = D3DCOLOR_XRGB(0,0,0), float zDepth = 1.0f);

    static void deinit(); // Free up our 3D Object

    ~CD3DObj()
    {
        /* do nothing */
    }; // Deconstuctor

    // This is our D3D interface used to create other D3D objects/interfaces.
    static IDirect3D9 *mInterface;

    // This is our D3D device which allows "direct" access to the graphics card
    // It is our main device for primitive rendering functionality
    static IDirect3DDevice9 *mDevice;

private:

    static HRESULT mResult; // Used for error checking
    static bool mFullScreen; // True if we're full screen, false otherwise
    static HWND mHWND; // Copy of the window's hwnd

    // Perspective matrix parameters
    static float mFOV; // Field of view
    static float mNearClip; // Near clip plane
    static float mFarClip; // Far clip plane
    static float mAspectRatio; // Width / Height of viewable screen

    static bool createDevice(); // Creates the IDirect3DDevice9
    static void setPresentationParams(D3DPRESENT_PARAMETERS &params); // Fills "params" depending on
    // full screen mode

    // We make the copy constructor and assignment operator private because
    // we do NOT want anyone accidentally making a copy of this class
    CD3DObj(const CD3DObj &obj) {}
    CD3DObj& operator =(CD3DObj &obj)
    {
        return *this;
    }
};

// Here we define our global 3D object
extern CD3DObj *g3D;

#endif
