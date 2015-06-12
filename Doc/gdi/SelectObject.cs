//z 2015-06-12 17:03:34 L.202'24986 T662091025 .K
SelectObject function
The SelectObject function selects an object into the specified device context (DC). The new object replaces the previous object of the same type.
Syntax

C++

HGDIOBJ SelectObject(
  _In_  HDC     hdc,
  _In_  HGDIOBJ hgdiobj
);

Parameters

hdc [in]
A handle to the DC.
hgdiobj [in]
A handle to the object to be selected. The specified object must have been created by using one of the following functions.

Return value

If the selected object is not a region and the function succeeds, the return value is a handle to the object being replaced. If the selected object is a region and the function succeeds, the return value is one of the following values.

Value	Meaning
SIMPLEREGION	Region consists of a single rectangle.
COMPLEXREGION	Region consists of more than one rectangle.
NULLREGION	Region is empty.
 
If an error occurs and the selected object is not a region, the return value is NULL. Otherwise, it is HGDI_ERROR.

Remarks
2015-06-12 17:05 zitem 在使用新object绘制完毕后，应该总是恢复旧的默认object。
This function returns the previously selected object of the specified type. An application should always replace a new object with the original, default object after it has finished drawing with the new object.
一个bitmap不能被选入到多于一个DC中去。（同样的handle？）
An application cannot select a single bitmap into more than one DC at a time.
ICM: If the object being selected is a brush or a pen, color management is performed.

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:

    {
        hdc = BeginPaint(hWnd, &ps);
        //    Initializing original object
        HGDIOBJ original = NULL;

        //    Saving the original object
        //z 选入 dc pen ，保存原来的 pen 
        original = SelectObject(hdc,GetStockObject(DC_PEN));

        //    Rectangle function is defined as...
        //    BOOL Rectangle(hdc, xLeft, yTop, yRight, yBottom);

        //    Drawing a rectangle with just a black pen
        //    The black pen object is selected and sent to the current device context
        //  The default brush is WHITE_BRUSH
        //z 选入 black pen 
        SelectObject(hdc, GetStockObject(BLACK_PEN));
        Rectangle(hdc,0,0,200,200);

        //    Select DC_PEN so you can change the color of the pen with
        //    COLORREF SetDCPenColor(HDC hdc, COLORREF color)
        SelectObject(hdc, GetStockObject(DC_PEN));

        //    Select DC_BRUSH so you can change the brush color from the
        //    default WHITE_BRUSH to any other color
        SelectObject(hdc, GetStockObject(DC_BRUSH));

        //    Set the DC Brush to Red
        //    The RGB macro is declared in "Windowsx.h"
        SetDCBrushColor(hdc, RGB(255,0,0));

        //    Set the Pen to Blue
        SetDCPenColor(hdc, RGB(0,0,255));

        //    Drawing a rectangle with the current Device Context
        Rectangle(hdc,100,300,200,400);

        //    Changing the color of the brush to Green
        SetDCBrushColor(hdc, RGB(0,255,0));
        Rectangle(hdc,300,150,500,300);

        //    Restoring the original object
        //z 对选入的 pen 以及 brush 都只用选入这个即可？
        SelectObject(hdc,original);

        // It is not necessary to call DeleteObject to delete stock objects.
    }

    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
