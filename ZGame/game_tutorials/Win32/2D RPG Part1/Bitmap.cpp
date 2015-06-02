﻿//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 1)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //
//																		 //
//***********************************************************************//

#include "main.h"

// Include the Win32 library for the GDI function TransparentBlt()
#pragma comment(lib, "msimg32.lib")


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    CBuffer()
///// DESCRIPTION: The constructor for the CBuffer class
///// INPUT:       nothing
///// OUTPUT:      All the member variables are freed
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

CBuffer::CBuffer():hWnd(0),hCompBitmap(0),hOldCompBitmap(0),hOldDispBitmap(0),hdcFront(0),hdcBack(0),oldPen(0),oldBrush(0)
{
    memset(&scrnRect, 0, sizeof(RECT));
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    ~CBuffer()
///// DESCRIPTION: The deconstructor for the CBuffer class
///// INPUT:       nothing
///// OUTPUT:      oldPen, oldBrush - these are restored
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

CBuffer::~CBuffer()
{
    // We need to make sure if we used a pen or brush the original
    // pen and brushes are restored so there isn't a memory leak.
    if(oldPen)
    {
        HPEN setPen = (HPEN)SelectObject(hdcBack, oldPen);
        DeleteObject(setPen);
    }

    if(oldBrush)
    {
        HBRUSH setBrush = (HBRUSH)SelectObject(hdcBack, oldBrush);
        DeleteObject(setBrush);
    }

    // Free our back buffer information
    FreeBuffers();
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    CreateDoubleBuffering()
///// DESCRIPTION: This sets up the double buffering by filling in the
/////			   buffer structure that is passed in.
///// INPUT:       hWnd    - The window handle
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::CreateDoubleBuffering(HWND hWndNew)
{
    // Assign the window handle to our structure
    hWnd = hWndNew;

    // Store the client rectangle in our structure
    GetClientRect(hWnd, &(scrnRect));

    // Get a handle to the device context and store it as the front buffer
    hdcFront = GetDC(hWnd);

    // Create a compatible device context for the backbuffer
    hdcBack = CreateCompatibleDC(hdcFront);

    // Create another hdc to store the bitmap in before the backbuffer
    hdcBitmap = CreateCompatibleDC(hdcFront);

    // Create a dummy bitmap handle to store in the front buffer
    hCompBitmap = CreateCompatibleBitmap(hdcFront, scrnRect.right, scrnRect.bottom);

    // Select the bitmap handle into the back buffer to make it the same size as the front hdc
    hOldCompBitmap = (HBITMAP)SelectObject(hdcBack, hCompBitmap);

    // Also select it int the bitmap hdc so we can store its original handle to put back later
    hOldDispBitmap = (HBITMAP)SelectObject(hdcBack, hCompBitmap);
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    LoadABitmap()
///// DESCRIPTION: This loads a bitmap and returns a handle to that bitmap.
///// INPUT:       szFileName - The name of the file to load (Ie. "Bitmap.bmp")
///// OUTPUT:      nothing
///// RETURN:      HBITMAP - The handle that references the bitmap loaded
/////
//////////////////////////////////////////////////////////////////////////

HBITMAP CBuffer::LoadABitmap(LPSTR szFileName)
{
    // Load the bitmap and return the handle to the bitmap we just loaded
    return (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DisplayBitmap()
///// DESCRIPTION: This puts the bitmap in the backbuffer
///// INPUT:       hBitmap - The handle to the bitmap to display
/////			   x, y    - The X and Y coordinates to display the bitmap
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::DisplayBitmap(HBITMAP hBitmap, int x, int y)
{
    // Used to hold information about the bitmap
    BITMAP info = {0};

    // Get the bitmap's information
    GetObject(hBitmap, sizeof(BITMAP), &info);

    // Select the bitmap handle into the extra hdc that holds the bitmap
    SelectObject(hdcBitmap, hBitmap);

    // Blit the bitmap hdc into the backbuffer
    BitBlt(hdcBack, x, y, info.bmWidth, info.bmHeight, hdcBitmap, 0, 0, SRCCOPY);
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DisplayBitmap()
///// DESCRIPTION: This takes a portion of a bitmap and puts it in the backbuffer
///// INPUT:       hBitmap - The handle to the bitmap to display
/////			   x, y    - The X and Y coordinates to display the bitmap
/////			   rPortion - The portion of the bitmap to be displayed
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::DisplayBitmap(HBITMAP hBitmap, int x, int y, RECT rPortion)
{
    // Assign the width and height to shorten variables.
    int width = rPortion.right - rPortion.left, height = rPortion.bottom - rPortion.top;

    // Select the bitmap handle into the extra hdc that holds the bitmap
    SelectObject(hdcBitmap, hBitmap);

    // Blit the bitmap hdc into the backbuffer using the transparent color 0 (black)
    BOOL result = BitBlt(hdcBack, x, y, width, height, hdcBitmap, rPortion.left, rPortion.top, SRCCOPY);
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DisplayTransparentBitmap()
///// DESCRIPTION: This takes a bitmap and puts it in the backbuffer
/////			   BUT, not displaying any color with rgb values of (0, 0, 0).
/////			   The problem with this function is that is leaks memory in Win98
///// INPUT:       hBitmap - The handle to the bitmap to display
/////			   x, y    - The X and Y coordinates to display the bitmap
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::DisplayTransparentBitmap(HBITMAP hBitmap, int x, int y)
{
    // Used to hold information about the bitmap
    BITMAP info = {0};

    // Get the bitmap's information
    GetObject(hBitmap, sizeof(BITMAP), &info);

    // Select the bitmap handle into the extra hdc that holds the bitmap
    SelectObject(hdcBitmap, hBitmap);

    TransparentBlt(hdcBack, x, y, info.bmWidth, info.bmHeight,
                   hdcBitmap, 0, 0, info.bmWidth, info.bmHeight, TRANSPARENT_COLOR);
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DisplayTransparentBitmap()
///// DESCRIPTION: This takes a portion of a bitmap and puts it in the backbuffer
/////			   BUT, not displaying any color with rgb values of (0, 0, 0).
/////			   The problem with this function is that is leaks memory in Win98
///// INPUT:       hBitmap - The handle to the bitmap to display
/////			   x, y    - The X and Y coordinates to display the bitmap
/////			   rPortion - The portion of the bitmap to be displayed
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::DisplayTransparentBitmap(HBITMAP hBitmap, int x, int y, RECT rPortion)
{
    // Assign the width and height to shorten variables.
    int width = rPortion.right - rPortion.left, height = rPortion.bottom - rPortion.top;

    // Select the bitmap handle into the extra hdc that holds the bitmap
    SelectObject(hdcBitmap, hBitmap);

    TransparentBlt(hdcBack, x, y, width, height, hdcBitmap,
                   rPortion.left, rPortion.top, width, height, TRANSPARENT_COLOR);
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DrawBackgroundImage()
///// DESCRIPTION: This draws and if desired, stretches an image to the backbuffer
///// INPUT:       hBitmap - The handle to the bitmap to display
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::DrawBackgroundImage(HBITMAP hBitmap)
{
    // This will hold our information on the bitmap
    BITMAP bitmapInfo;

    // Select the bitmap handle into the extra hdc that holds the bitmap
    HBITMAP hOldBitmap = (HBITMAP) SelectObject(hdcBitmap, hBitmap);

    // Get the width and height of the background bitmap
    GetObject(hBitmap, sizeof(BITMAP), &bitmapInfo);

    // Blit the bitmap hdc into the backbuffer using the transparent color 0 (black)
    BOOL result = StretchBlt(hdcBack, 0, 0, scrnRect.right, scrnRect.bottom,
                             hdcBitmap, 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, SRCCOPY);

    // Select the old handle to the bitmap back where it was to clean up
    SelectObject(hdcBitmap, hOldBitmap);
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    ClearScreen()
///// DESCRIPTION: This fills the rectangle passed in with a specified color
///// INPUT:       color    - The color to fill the rectangle in (i.e. WHITE_BRUSH)
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::ClearScreen(int color)
{
    // Fill the backbuffer with white to erase the previous position of the bitmap
    FillRect(hdcBack, &scrnRect, (HBRUSH)GetStockObject(color));
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    SetPen()
///// DESCRIPTION: This sets the current pen type, thickness and color
///// INPUT:       type - the type of pen
/////			   thickness - the thickness in pixels for the pen
/////			   color - the color of the pen
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::SetPen(int type, int thickness, COLORREF color)
{
    // First create a brush that is hollow for shapes
    LOGBRUSH logBrush = {BS_HOLLOW, DIB_RGB_COLORS, NULL};

    // Set our new brush and our desired pen type
    HBRUSH hBrush = CreateBrushIndirect(&logBrush);
    HPEN hPen = CreatePen(type, thickness, color);

    // Make sure we only store the old original pen if we haven't yet
    if(!oldPen)
    {
        // Select both into our HDC
        oldPen = (HPEN)SelectObject(hdcBack,hPen);
        oldBrush = (HBRUSH)SelectObject(hdcBack, hBrush);
    }
    else
    {
        HPEN lastPen = (HPEN)SelectObject(hdcBack,hPen);
        DeleteObject(lastPen);
    }
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    SwapBackBuffer()
///// DESCRIPTION: This flips the backbuffer to the front of the screen,
/////			   which displays the bitmaps to the screen.
///// INPUT:       bClearBackBuffer - A flag to clear the backbuffer or not
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::SwapBackBuffer(BOOL bClearBackBuffer)
{
    // Blit the whole backbuffer to the screen (The front buffer)
    BitBlt(hdcFront, scrnRect.left, scrnRect.top,
           scrnRect.right, scrnRect.bottom, hdcBack, 0, 0, SRCCOPY);

    // If we want to clear the screen afterwards, set the background to white
    if(bClearBackBuffer)
        ClearScreen(BLACK_BRUSH);
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    ResizeBuffers()
///// DESCRIPTION: Changes the size of the front and back buffers according
/////			   to the new client size of the window.
///// INPUT:       nothing
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::ResizeBuffers()
{
    // Get the new client rectangle of the window
    GetClientRect(hWnd, &(scrnRect));

    // Select the old bitmap back into the back buffer
    SelectObject(hdcBack, hOldCompBitmap);

    // Delete the old compatible bitmap
    DeleteObject(hCompBitmap);

    // Create a new compatible bitmap with the new size of the window
    hCompBitmap = CreateCompatibleBitmap(hdcFront, scrnRect.right, scrnRect.bottom);

    // Select the new compatible bitmap into the backbuffer and store the old one again
    hOldCompBitmap = (HBITMAP) SelectObject(hdcBack, hCompBitmap);
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    FreeBuffers()
///// DESCRIPTION: Frees the buffer for the window
///// INPUT:       nothing
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CBuffer::FreeBuffers()
{
    // Set the back buffer to it's original bitmap (This is necessary for clean-up)
    SelectObject(hdcBack, hOldCompBitmap);

    // Set the bitmap hdc back to it's original bitmap handle (This is necessary for clean-up)
    SelectObject(hdcBitmap, hOldDispBitmap);

    // Free the hbitmap
    if(hCompBitmap)	DeleteObject(hCompBitmap);

    // Free all the HDC's
    if(hdcBack)		DeleteDC(hdcBack);
    if(hdcBitmap)	DeleteDC(hdcBitmap);
    if(hdcFront)	ReleaseDC(hWnd, hdcFront);
}


////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// In this file we handle all of the back buffer and bitmap
// routines.  We created a class to encapsulate all these
// important routines into one place.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials
