// MultiContext.cpp

// This example uses a deferred context to draw two strings in parallel
// This is an extreme case of bad performance, as two 25,000 character text layouts are reconstructed every frame

#include <D3D11.h>
#include "FW1FontWrapper.h"

#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "FW1FontWrapper.lib")


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadProc(LPVOID lpParam);
void drawRandomString(IFW1FontWrapper *pFontWrapper, ID3D11DeviceContext *pContext, FLOAT x, FLOAT y, FLOAT lineWidth);

struct ThreadData
{
    IFW1FontWrapper *pFontWrapper;
    ID3D11DeviceContext *pContext;
    FLOAT x;
    FLOAT y;
    FLOAT lineWidth;

    volatile bool loop;
    HANDLE hStartDraw;
    HANDLE hDrawComplete;
};

volatile bool g_multiThread = false;


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
                    TEXT("D3D11 Font Wrapper"),
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

    // Create the font-wrapper
    IFW1Factory *pFW1Factory;
    hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
    if(FAILED(hResult))
    {
        MessageBox(NULL, TEXT("FW1CreateFactory"), TEXT("Error"), MB_OK);
        return 0;
    }

    IFW1FontWrapper *pFontWrapper;
    hResult = pFW1Factory->CreateFontWrapper(pDevice, L"Comic Sans MS", &pFontWrapper);
    if(FAILED(hResult))
    {
        MessageBox(NULL, TEXT("CreateFontWrapper"), TEXT("Error"), MB_OK);
        return 0;
    }

    pFW1Factory->Release();

    // Deferred context
    ID3D11DeviceContext *pDeferredContext;
    pDevice->CreateDeferredContext(0, &pDeferredContext);

    // Thread
    ThreadData threadData;
    threadData.pFontWrapper = pFontWrapper;
    threadData.pContext = pDeferredContext;
    threadData.x = vp.Width/2.0f + 40.0f;
    threadData.y = 10.0f;
    threadData.lineWidth = vp.Width/2.0f - 50.0f;
    threadData.loop = true;
    threadData.hStartDraw = CreateEvent(NULL, FALSE, FALSE, NULL);
    threadData.hDrawComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
    HANDLE hThread = CreateThread(NULL, 0, ThreadProc, &threadData, 0, NULL);

    // Main loop
    ShowWindow(hWnd, nCmdShow);

    LARGE_INTEGER t0, tf;
    QueryPerformanceFrequency(&tf);
    QueryPerformanceCounter(&t0);
    UINT frameNum = 0;
    UINT lastFrame = 0;
    UINT fps = 0;
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

            // Draw two random strings, one on a deferred context
            pDeferredContext->RSSetViewports(1, &vp);
            pDeferredContext->OMSetRenderTargets(1, &pRTV, NULL);

            if(g_multiThread)
                SetEvent(threadData.hStartDraw);
            else
                drawRandomString(threadData.pFontWrapper, threadData.pContext, threadData.x, threadData.y, threadData.lineWidth);

            drawRandomString(pFontWrapper, pImmediateContext, 10.0f, 150.0f, vp.Width / 2.0f - 50.0f);

            if(g_multiThread)
                WaitForSingleObject(threadData.hDrawComplete, INFINITE);

            ID3D11CommandList *pCommandList;
            pDeferredContext->FinishCommandList(TRUE, &pCommandList);

            pFontWrapper->Flush(pImmediateContext);
            pImmediateContext->ExecuteCommandList(pCommandList, TRUE);
            pCommandList->Release();

            // Info output
            WCHAR frameStr[255];
            wsprintfW(frameStr, L"FPS: %u", fps);
            pFontWrapper->DrawString(pImmediateContext, frameStr, 64.0f, 10.0f, 10.0f, 0xff0000ff, 0);

            WCHAR str[255];
            wsprintfW(str, L"[M]ulti-thread: %s", g_multiThread ? L"Enabled" : L"Disabled");
            pFontWrapper->DrawString(pImmediateContext, str, 32.0f, 10.0f, 90.0f, 0xff0000ff, FW1_IMMEDIATECALL);

            // Timer
            ++frameNum;

            LARGE_INTEGER t1;
            QueryPerformanceCounter(&t1);
            if(t1.QuadPart - t0.QuadPart >= tf.QuadPart/2)
            {
                double frameCount = static_cast<double>(frameNum - lastFrame);
                double timePerFrame = static_cast<double>(t1.QuadPart - t0.QuadPart) / frameCount;
                fps = static_cast<UINT>((static_cast<double>(tf.QuadPart) / timePerFrame) + 0.5);

                t0 = t1;
                lastFrame = frameNum;
            }

            // Present
            pSwapChain->Present(0, 0);
        }
    }

    // Release thread
    threadData.loop = false;
    SetEvent(threadData.hStartDraw);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(threadData.hStartDraw);
    CloseHandle(threadData.hDrawComplete);

    // Release
    pSwapChain->SetFullscreenState(FALSE, NULL);

    pImmediateContext->ClearState();

    pDeferredContext->Release();

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

    case WM_KEYDOWN:
        if(wParam == 'M')
            g_multiThread = !g_multiThread;
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Thread
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
    ThreadData *threadData = (ThreadData*)lpParam;

    while(threadData->loop)
    {
        WaitForSingleObject(threadData->hStartDraw, INFINITE);
        if(threadData->loop)
        {
            drawRandomString(threadData->pFontWrapper, threadData->pContext, threadData->x, threadData->y, threadData->lineWidth);
            SetEvent(threadData->hDrawComplete);
        }
    }

    return 0;
}

// Draw random string
void drawRandomString(IFW1FontWrapper *pFontWrapper, ID3D11DeviceContext *pContext, FLOAT x, FLOAT y, FLOAT lineWidth)
{
    const UINT len = 25000;
    WCHAR str[len+1];

    for(UINT i=0; i < len/2; ++i)
    {
        str[i*2+0] = static_cast<WCHAR>(33 + (rand()%94));
        str[i*2+1] = L' ';
    }
    str[len] = 0;

    FW1_RECTF rect = {x, y, x+lineWidth, y};
    pFontWrapper->DrawString(pContext, str, NULL, 9.0f, &rect, 0xffffffff, NULL, NULL, FW1_NOFLUSH | FW1_ALIASED);
}
