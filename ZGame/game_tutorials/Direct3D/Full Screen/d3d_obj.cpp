﻿#include "d3d_obj.h"

// Declare our static variables
IDirect3D9* CD3DObj::mInterface = NULL;
IDirect3DDevice9* CD3DObj::mDevice = NULL;
HRESULT CD3DObj::mResult = 0;
bool CD3DObj::mFullScreen = false; // Start out windowed
HWND CD3DObj::mHWND = NULL;
float CD3DObj::mFOV = 0.0f; // Field of view
float CD3DObj::mNearClip = 0.0f; // Near clip plane
float CD3DObj::mFarClip = 0.0f; // Far clip plane
float CD3DObj::mAspectRatio = 0.0f; // Width / Height of display screen

// Initializes our D3D object -- Returns true on success, false otherwise
bool CD3DObj::init(HWND hwnd)
{
    if(!hwnd)
        return false;

    mHWND = hwnd; // Store window handle

    // Create the D3D object, which is needed to create the D3DDevice.
    mInterface = Direct3DCreate9(D3D_SDK_VERSION);

    // Error Check
    if(mInterface == NULL)
        return false;

    // Create the D3D device
    if(!createDevice())
        return false;

    // Setup default render states
    setDefaultRenderState();
    return true; // We got loaded!
}

// Begins the scene
void CD3DObj::begin()
{
    // This begins our scene.
    mResult = mDevice->BeginScene();
    assert(mResult == D3D_OK);
}

// End the scene and draw it
void CD3DObj::end()
{
    // This ends the scene
    mResult = mDevice->EndScene();
    assert(mResult == D3D_OK);

    // Present the current buffer (we have two in our case) to the screen
    mResult = mDevice->Present(NULL, NULL, NULL, NULL);
    assert(mResult == D3D_OK);
}

// Set all the render states to a default value
void CD3DObj::setDefaultRenderState()
{
    assert(mDevice != NULL);

    // Turn off D3D lighting
    mResult = mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    assert(mResult == D3D_OK);

    // Turn on the z-buffer
    mResult = mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    assert(mResult == D3D_OK);
}

bool CD3DObj::toggleFullScreen()
{
    assert(mDevice != NULL);

    static RECT winRect = {0};
    D3DPRESENT_PARAMETERS params = {0}; // Presentation parameters to be filled

    // Toggle full screen flag
    mFullScreen = !mFullScreen;

    // Get presentation parameters for the new current full screen mode
    setPresentationParams(params);

    // If we're changing to full screen mode, save off the window's RECT so
    // we can put the window back in position when switching back to windowed mode
    if(mFullScreen == true)
        GetWindowRect(mHWND, &winRect);

    // Reset the device to the new parameters
    // This is the function that does the magic from windowed to full screen
    // and vice-versa
    /*
    	**NOTE** It is important to note that Reset() resets all of your
    			 render states.  So you have to set them back once you have
    			 changed from screen modes.
    */
    if(mDevice->Reset(&params) != D3D_OK)
    {
        // Put flag back
        mFullScreen = !mFullScreen;
        return false;
    }

    // If we've changed to windowed mode
    if(mFullScreen == false)
    {
        // Move and resize window
        SetWindowPos(mHWND, HWND_NOTOPMOST, winRect.left, winRect.top, winRect.right - winRect.left,
                     winRect.bottom - winRect.top, SWP_SHOWWINDOW);
    }

    // Calculate the new aspect ratio
    float aspectRatio = (float)params.BackBufferWidth / (float)params.BackBufferHeight;

    // We must reset the render states
    setDefaultRenderState();
    setProjMatrix(mFOV, aspectRatio, mNearClip, mFarClip);
    return true;
}

// This renders a list of SVertex type vertices that is "numVerts" long
bool CD3DObj::render(SVertex *vertList, int numVerts)
{
    // We're rendering triangles so the number of verts should evenly be divisible by 3
    assert(numVerts % 3 == 0);

    // Set the flexible vertex format
    mDevice->SetFVF(SVertexType);

    // Draw the vertex list
    mResult = mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, numVerts / 3, vertList, sizeof(SVertex));
    return (mResult == D3D_OK);
}

// Renders a indexed list of SVertex as triangles
bool CD3DObj::render(SVertex *vertList, int numVerts, WORD *indexList, int numIndices)
{
    // We're rendering triangles so the number of indices should evenly be divisible by 3
    assert(numIndices % 3 == 0);

    // Set the flexible vertex format
    mDevice->SetFVF(SVertexType);

    // Draw the indexed vertex list
    mResult = mDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, numVerts, numIndices / 3,
              indexList, D3DFMT_INDEX16, vertList, sizeof(SVertex));
    return (mResult == D3D_OK);
}

bool CD3DObj::renderLine(SVertex *vertList, int numVerts)
{
    // We're rendering line segments so there should  be an even number of vertices
    assert(numVerts % 2 == 0);

    // Set the FVF
    mDevice->SetFVF(SVertexType);

    // Render the line segments
    mResult = mDevice->DrawPrimitiveUP(D3DPT_LINELIST, numVerts / 2, vertList, sizeof(SVertex));
    return (mResult == D3D_OK); // Returns status of DrawPrimitiveUP()
}

// Sets up the view of the scene based on passed in eye and target
void CD3DObj::setViewMatrix(const CPos &eye, const CPos &lookAt)
{
    D3DXMATRIXA16 matrix;
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // World's up vector

    // Create and set the view matrix
    D3DXMatrixLookAtLH(&matrix, (D3DXVECTOR3*)(&eye), (D3DXVECTOR3*)(&lookAt), &up);
    mDevice->SetTransform(D3DTS_VIEW, &matrix); // Set our view of the world
}

// Sets the projection matrix, which dictates how our 3D scene is projected onto our 2D monitor
// using the passed in field of view, aspect ratio, near clip plane, and far clip plane
void CD3DObj::setProjMatrix(float fov, float aspectRatio, float nearClip, float farClip)
{
    D3DXMATRIXA16 matrix;

    mFOV = fov;
    mAspectRatio = aspectRatio;
    mNearClip = nearClip;
    mFarClip = farClip;

    // Create and set projection matrix
    D3DXMatrixPerspectiveFovLH(&matrix, fov, aspectRatio, nearClip, farClip);
    mDevice->SetTransform(D3DTS_PROJECTION, &matrix);
}

// Sets the world matrix to the matrix passed in
void CD3DObj::setWorldMatrix(const D3DXMATRIX *matrix)
{
    mDevice->SetTransform(D3DTS_WORLD, matrix);
}

// Clears the viewport to a specified ARGB color
bool CD3DObj::clear(int color, float zDepth)
{
    // This clears our viewport to the color and zDepth passed in
    mResult = mDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, zDepth, 0);
    return (mResult == D3D_OK);
}

// Free up all the memory
void CD3DObj::deinit()
{
    if(mDevice != NULL)
        mDevice->Release();

    if(mInterface != NULL)
        mInterface->Release();

    // Zero out our D3D interface and device
    mDevice = NULL;
    mInterface = NULL;

    // Zero out window handle
    mHWND = NULL;
}

////////////////////////////////
// *** Private Functions *** //
//////////////////////////////

bool CD3DObj::createDevice()
{
    assert(mInterface != NULL);
    assert(mDevice == NULL); // Should be destroyed before reallocated

    D3DPRESENT_PARAMETERS params = {0}; // Presentation parameters to be filled

    // Fill the presentation parameters
    setPresentationParams(params);

    // Set the Aspect Ratio
    mAspectRatio = (float)params.BackBufferWidth / (float)params.BackBufferHeight;

    mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT,
                                       D3DDEVTYPE_HAL,
                                       mHWND,
                                       D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                       &params,
                                       &mDevice);

    // It's possible we'll get an error because not all vid-cards can handle vertex processing
    // So in the event we do get an error we'll try to make the device again.  We will
    // only change D3DCREATE_HARDWARE_VERTEXPROCESSING to D3DCREATE_SOFTWARE_VERTEXPROCESSING
    // which says, "Do the vertex processing in software"
    if(mResult != D3D_OK)
    {
        mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT,
                                           D3DDEVTYPE_HAL,
                                           mHWND,
                                           D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                           &params,
                                           &mDevice);

        if(mResult != D3D_OK)
            return false; // Couldn't create a D3D 9.0 device
    }

    return true; // Device got loaded
}

// Fill the D3DPRESENT_PARAMETERS with the correct parameters based
// on windowed or full screen mode
void CD3DObj::setPresentationParams(D3DPRESENT_PARAMETERS &params)
{
    // Zero out the params struct
    ZeroMemory(&params, sizeof(D3DPRESENT_PARAMETERS));

    /*
    	By default D3D only has one buffer, the frame buffer, which it draws to.  Just like
    	in Win32 apps, if you always draw directly to the screen, there's a good chance
    	you get visual artifacts which look bad.  To get around this, a back buffer, is used.
    	Using the back buffer you can do all of your drawing to it, then once finished,
    	draw the whole thing to the screen.

    	Also by default, D3D locks to V-sync.  What does that mean?  Well your monitor
    	refreshes at a set Hertz.  Lets say it's 60 Hertz.  If you're locked to V-Sync that
    	means you can never acheive more than 60 frames per-second, because the screen
    	only refreshes 60 times a second.  If you want an accurate gage of how many frames
    	per second your rendering, you need to not doing V-syncing and draw as fast as
    	possible.

    	Below we set the D3DPRESENT_PARAMETERS so that it has a double buffer and
    	draws as fast as possible (ie no V-sync).  The double buffer acts like this:
    	Everything gets drawn to the back buffer, which is then flipped to the front, the
    	previous front buffer becomes the back buffer, and the next rendering scene gets
    	drawn there.  This process loops indefinitely.

    	You should notice a smother application, if you drag the window and when
    	you toggle between windowed mode and full screen.
    */

    // Set all parameters that can be shared between windowed and full screen mode
    params.hDeviceWindow = mHWND; // Handle to window
    params.BackBufferCount = 1; // Number of render surfaces
    params.SwapEffect = D3DSWAPEFFECT_FLIP; // We want to treat the back buffers as a circular
    // queue and rotate through them
    params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // Draw everything immediately, don't
    // wait for V-sync
    params.EnableAutoDepthStencil = TRUE; // We want a Z-buffer
    params.AutoDepthStencilFormat = D3DFMT_D16; // 16-bit Z-buffer (should be safe on most cards...)

    if(mFullScreen)
    {
        D3DDISPLAYMODE dispMode = {0};

        // Get the current configuration of the primary monitor
        mResult = mInterface->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);
        assert(mResult == D3D_OK);

        // Fill in the rest of the parameters with the primary display device's parameters
        params.Windowed = FALSE;
        params.BackBufferWidth = dispMode.Width;
        params.BackBufferHeight = dispMode.Height;
        params.FullScreen_RefreshRateInHz = dispMode.RefreshRate;
        params.BackBufferFormat = dispMode.Format; // Use the current color depth of the monitor
    }
    else
    {
        // Get client rect
        RECT rect = {0};
        GetClientRect(mHWND, &rect);

        params.Windowed = true; // We want a window
        params.BackBufferWidth = rect.right; // Backbuffer's width equals client rect's width
        params.BackBufferHeight = rect.bottom; // Backbuffer's height equals client rect's height
        params.BackBufferFormat = D3DFMT_UNKNOWN; // Use whatever the current color depth of the
        // monitor is for the back buffer
    }
}

CD3DObj theD3DObj; // Create our 3D Object
CD3DObj *g3D = &theD3DObj; // Set the global pointer to our 3D Object
