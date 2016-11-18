// Transforms.cpp

// This example shows how to use a transform matrix when drawing text

#include <D3D11.h>
#include "FW1FontWrapper.h"
#include <xnamath.h>

#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "FW1FontWrapper.lib")


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Register windowclass
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.lpszClassName = TEXT("MyClass");
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wc);

    // Create window
    HWND hWnd = CreateWindowEx(
                    WS_EX_OVERLAPPEDWINDOW,
                    wc.lpszClassName,
                    TEXT("D3D11 Font Wrapper - Transforms"),
                    WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    NULL,
                    NULL,
                    hInstance,
                    NULL
                );

    // Create device and swapchain
    DXGI_SWAP_CHAIN_DESC scd = {0};
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 1;
    scd.OutputWindow = hWnd;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Windowed = TRUE;

    IDXGISwapChain *pSwapChain;
    ID3D11Device *pDevice;
    ID3D11DeviceContext *pImmediateContext;
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hResult = D3D11CreateDeviceAndSwapChain(
                          NULL,
                          D3D_DRIVER_TYPE_HARDWARE,
                          NULL,
                          0,
                          NULL,
                          0,
                          D3D11_SDK_VERSION,
                          &scd,
                          &pSwapChain,
                          &pDevice,
                          &featureLevel,
                          &pImmediateContext
                      );
    if(FAILED(hResult))
    {
        MessageBox(NULL, TEXT("D3D11CreateDeviceAndSwapChain"), TEXT("Error"), MB_OK);
        return 0;
    }

    // Render target
    ID3D11Texture2D *pBackBuffer;
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

    ID3D11RenderTargetView *pRTV;
    pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTV);

    D3D11_TEXTURE2D_DESC bd;
    pBackBuffer->GetDesc(&bd);

    pBackBuffer->Release();

    // Viewport
    D3D11_VIEWPORT vp = {0};
    vp.Width = static_cast<FLOAT>(bd.Width);
    vp.Height = static_cast<FLOAT>(bd.Height);
    vp.MaxDepth = 1.0f;
    pImmediateContext->RSSetViewports(1, &vp);

    // Create the font-wrapper factory
    IFW1Factory *pFW1Factory;
    hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
    if(FAILED(hResult))
    {
        MessageBox(NULL, TEXT("FW1CreateFactory"), TEXT("Error"), MB_OK);
        return 0;
    }

    // Set up the font-wrapper to use mip-mapping and anisotropic filtering for better visuals with transformed text
    FW1_FONTWRAPPERCREATEPARAMS createParams = {0};
    createParams.SheetMipLevels = 5;
    createParams.AnisotropicFiltering = TRUE;
    createParams.DefaultFontParams.pszFontFamily = L"Arial";
    createParams.DefaultFontParams.FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
    createParams.DefaultFontParams.FontStyle = DWRITE_FONT_STYLE_NORMAL;
    createParams.DefaultFontParams.FontStretch = DWRITE_FONT_STRETCH_NORMAL;

    IFW1FontWrapper *pFontWrapper;
    hResult = pFW1Factory->CreateFontWrapper(pDevice, NULL, &createParams, &pFontWrapper);
    if(FAILED(hResult))
    {
        MessageBox(NULL, TEXT("CreateFontWrapper"), TEXT("Error"), MB_OK);
        return 0;
    }

    pFW1Factory->Release();

    // Transform matrices
    FLOAT rotation = 0.0f;
    XMMATRIX matPerspectiveProjection = XMMatrixPerspectiveFovLH(XM_PI/4.0f, vp.Width/vp.Height, 1.0f, 2000.0f);
    XMMATRIX matOrthoProjection = XMMatrixOrthographicOffCenterLH(0.0f, vp.Width, vp.Height, 0.0f, 0.0f, 1.0f);
    XMMATRIX matTranslation = XMMatrixTranslation(0.0f, 0.0f, 700.0f);
    XMMATRIX matScale = XMMatrixScaling(1.0f, -1.0f, 1.0f);

    // Timer
    LARGE_INTEGER t0, tf;
    QueryPerformanceFrequency(&tf);
    QueryPerformanceCounter(&t0);
    UINT frameNum = 0;
    UINT lastFrame = 0;
    UINT fps = 0;
    FLOAT frameTime = 0.0f;

    // Main loop
    ShowWindow(hWnd, nCmdShow);

    bool loop = true;
    while(loop)
    {
        MSG msg;
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
        {
            if(msg.message == WM_QUIT)
                loop = false;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            pImmediateContext->OMSetRenderTargets(1, &pRTV, NULL);

            FLOAT backgroundColor[4] = {0.2f, 0.3f, 0.4f, 1.0f};
            pImmediateContext->ClearRenderTargetView(pRTV, backgroundColor);

            // Place strings at 0,0 and use the transform matrix to move them
            FW1_RECTF rect = {0.0f, 0.0f, 0.0f, 0.0f};

            // Cursor position
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hWnd, &pt);

            // Draw transformed string
            FLOAT rotx = (pt.y - vp.Height/2.0f) * -0.003f;
            FLOAT roty = (pt.x - vp.Width/2.0f) * -0.003f;
            XMMATRIX matTransform =
                matScale
                * XMMatrixRotationX(rotx)
                * XMMatrixRotationY(roty)
                * matTranslation
                * matPerspectiveProjection;

            WCHAR frameStr[255];
            wsprintfW(frameStr, L"Frame: %u\nFPS: %u", frameNum, fps);
            pFontWrapper->DrawString(
                pImmediateContext,
                frameStr,
                NULL,
                96.0f,
                &rect,
                0xff0000ff,
                NULL,
                reinterpret_cast<FLOAT*>(&matTransform),
                FW1_CENTER | FW1_VCENTER | FW1_NOWORDWRAP
            );

            // Draw rotating glyph at cursor position
            rotation += 1.0f * frameTime;
            matTransform =
                XMMatrixRotationZ(-rotation*2.0f)
                * XMMatrixTranslation(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f)
                * matOrthoProjection;

            pFontWrapper->DrawString(
                pImmediateContext,
                L"#",
                L"Webdings",
                128.0f,
                &rect,
                0xff7fccff,
                NULL,
                reinterpret_cast<FLOAT*>(&matTransform),
                FW1_CENTER | FW1_VCENTER | FW1_STATEPREPARED | FW1_BUFFERSPREPARED
            );

            // Timer
            ++frameNum;

            LARGE_INTEGER t1;
            QueryPerformanceCounter(&t1);
            if(t1.QuadPart - t0.QuadPart >= tf.QuadPart/16)
            {
                double frameCount = static_cast<double>(frameNum - lastFrame);
                double timePerFrame = (t1.QuadPart - t0.QuadPart) / frameCount;

                frameTime = static_cast<FLOAT>(timePerFrame / tf.QuadPart);
                fps = static_cast<UINT>((tf.QuadPart / timePerFrame) + 0.5);

                t0 = t1;
                lastFrame = frameNum;
            }

            // Present
            pSwapChain->Present(0, 0);
        }
    }

    // Release
    pSwapChain->SetFullscreenState(FALSE, NULL);

    pImmediateContext->ClearState();

    pFontWrapper->Release();

    pRTV->Release();

    pImmediateContext->Flush();
    pImmediateContext->Release();
    pDevice->Release();
    pSwapChain->Release();

    UnregisterClass(wc.lpszClassName, hInstance);

    return 0;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
