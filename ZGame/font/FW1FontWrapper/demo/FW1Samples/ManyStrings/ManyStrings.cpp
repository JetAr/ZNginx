// ManyStrings.cpp

// This example shows how to reduce buffer updates and draw calls when drawing many small strings
// It also shows the performance difference between pre-constructing text layouts and re-constructing them
// every frame, as well as keeping the actual geometry around between frames for static strings

#include <D3D11.h>
#include "FW1FontWrapper.h"
#include <string>
#include <vector>

#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "FW1FontWrapper.lib")


struct StringData
{
    std::wstring str;
    float fontSize;
    const wchar_t *fontName;
    IDWriteTextLayout *pTextLayout;
    UINT32 color;
    float x, y;
    float vx, vy;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Draw settings
bool g_static = false;
bool g_staticComplete = false;
bool g_batchStrings = false;
bool g_usePreconstructed = false;

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
                    TEXT("D3D11 Font Wrapper - Many Strings"),
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
    hResult = pFW1Factory->CreateFontWrapper(pDevice, L"Arial", &pFontWrapper);
    if(FAILED(hResult))
    {
        MessageBox(NULL, TEXT("CreateFontWrapper"), TEXT("Error"), MB_OK);
        return 0;
    }

    // Create a text-geometry to store string vertices in
    IFW1TextGeometry *pTextGeometry;
    pFW1Factory->CreateTextGeometry(&pTextGeometry);

    pFW1Factory->Release();

    // Get DirectWrite factory to create text layouts with
    IDWriteFactory *pDWriteFactory;
    pFontWrapper->GetDWriteFactory(&pDWriteFactory);

    // Create the default DirectWrite text format to base layouts on
    IDWriteTextFormat *pTextFormat;
    hResult = pDWriteFactory->CreateTextFormat(
                  L"Arial",
                  NULL,
                  DWRITE_FONT_WEIGHT_NORMAL,
                  DWRITE_FONT_STYLE_NORMAL,
                  DWRITE_FONT_STRETCH_NORMAL,
                  16.0f,
                  L"",
                  &pTextFormat
              );
    if(FAILED(hResult))
    {
        MessageBox(NULL, TEXT("CreateTextFormat"), TEXT("Error"), MB_OK);
        return 0;
    }

    // Init strings
    const size_t stringCount = 1000;
    const int minLength = 2;
    const int maxLength = 7;
    const float maxVel = 100.0f;
    const int minSize = 9;
    const int maxSize = 20;
    std::vector<StringData> strings(stringCount);
    const wchar_t* fonts[] = {L"Arial", L"Courier New", L"MS Sans Serif", L"Times New Roman"};
    const int fontCount = sizeof(fonts) / sizeof(fonts[0]);

    for(size_t i=0; i < strings.size(); ++i)
    {
        StringData &stringData = strings[i];

        size_t length = static_cast<size_t>((rand() % (maxLength - minLength + 1)) + minLength);

        stringData.str.resize(length);
        for(size_t i=0; i < length; ++i)
            stringData.str[i] = static_cast<wchar_t>(33 + (rand()%94));

        stringData.fontSize = static_cast<float>((rand() % (maxSize - minSize + 1)) + minSize);
        stringData.fontName = fonts[rand() % fontCount];

        UINT red = (rand() % 256U) << 16;
        UINT green = (rand() % 256U) << 8;
        UINT blue = (rand() % 256U);
        stringData.color = 0xff000000 | red | green | blue;

        stringData.x = static_cast<float>((rand() % bd.Width));
        stringData.y = static_cast<float>((rand() % bd.Height));
        stringData.vx = (((rand() % 2001) - 1000) / 1000.0f) * maxVel;
        stringData.vy = (((rand() % 2001) - 1000) / 1000.0f) * maxVel;

        pDWriteFactory->CreateTextLayout(
            stringData.str.c_str(),
            static_cast<UINT32>(stringData.str.size()),
            pTextFormat,
            0.0f,
            0.0f,
            &stringData.pTextLayout
        );

        stringData.pTextLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        stringData.pTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        stringData.pTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        DWRITE_TEXT_RANGE allText = {0, static_cast<UINT32>(stringData.str.size())};
        stringData.pTextLayout->SetFontSize(stringData.fontSize, allText);
        stringData.pTextLayout->SetFontFamilyName(stringData.fontName, allText);
    }

    pTextFormat->Release();
    pDWriteFactory->Release();

    // Timer
    LARGE_INTEGER t0, tf;
    QueryPerformanceFrequency(&tf);
    QueryPerformanceCounter(&t0);
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

            // Draw empty string to prepare states
            pFontWrapper->DrawString(pImmediateContext, L"", 10.0f, 0.0f, 0.0f, 0xff000000, 0);

            if(!g_static || !g_staticComplete)
            {
                g_staticComplete = true;

                // Move strings
                for(size_t i=0; i < strings.size(); ++i)
                {
                    StringData &stringData = strings[i];

                    stringData.x += stringData.vx * frameTime;
                    stringData.y += stringData.vy * frameTime;

                    if((stringData.x < 0.0f && stringData.vx < 0.0f) || (stringData.x > vp.Width && stringData.vx > 0.0f))
                        stringData.vx = -stringData.vx;
                    if((stringData.y < 0.0f && stringData.vy < 0.0f) || (stringData.y > vp.Height && stringData.vy > 0.0f))
                        stringData.vy = -stringData.vy;
                }

                // Clear geometry
                pTextGeometry->Clear();

                // Draw strings using the specified method
                for(size_t i=0; i < strings.size(); ++i)
                {
                    StringData &stringData = strings[i];

                    if(g_batchStrings || g_static)
                    {
                        if(g_usePreconstructed)
                            pFontWrapper->AnalyzeTextLayout(NULL, stringData.pTextLayout, stringData.x, stringData.y, stringData.color, FW1_NOFLUSH, pTextGeometry);
                        else
                        {
                            FW1_RECTF rect = {stringData.x, stringData.y, stringData.x, stringData.y};
                            pFontWrapper->AnalyzeString(NULL, stringData.str.c_str(), stringData.fontName, stringData.fontSize, &rect, stringData.color, FW1_CENTER | FW1_VCENTER | FW1_NOWORDWRAP | FW1_NOFLUSH, pTextGeometry);
                        }
                    }
                    else
                    {
                        if(g_usePreconstructed)
                            pFontWrapper->DrawTextLayout(pImmediateContext, stringData.pTextLayout, stringData.x, stringData.y, stringData.color, FW1_IMMEDIATECALL);
                        else
                            pFontWrapper->DrawString(pImmediateContext, stringData.str.c_str(), stringData.fontName, stringData.fontSize, stringData.x, stringData.y, stringData.color, FW1_CENTER | FW1_VCENTER | FW1_IMMEDIATECALL);
                    }
                }
            }

            // Draw the geometry
            pFontWrapper->Flush(pImmediateContext);
            pFontWrapper->DrawGeometry(pImmediateContext, pTextGeometry, NULL, NULL, FW1_IMMEDIATECALL);

            // Draw framerate
            WCHAR frameStr[255];
            wsprintfW(frameStr, L"FPS: %u", fps);
            pFontWrapper->DrawString(pImmediateContext, frameStr, 64.0f, 10.0f, 10.0f, 0xff0000ff, FW1_IMMEDIATECALL);

            WCHAR settingsStr[255];
            wsprintfW(settingsStr, L"%u Strings\n[S]tatic Cached: %s\n[T]extLayouts: %s\n[B]atch Draws: %s", stringCount, g_static ? L"Yes" : L"No", g_usePreconstructed ? L"Yes" : L"No", g_batchStrings ? L"Yes" : L"No");
            pFontWrapper->DrawString(pImmediateContext, settingsStr, 32.0f, 10.0f, 90.0f, 0xffffffff, FW1_IMMEDIATECALL);

            // Timer
            LARGE_INTEGER t1;
            QueryPerformanceCounter(&t1);
            double timePerFrame = static_cast<double>(t1.QuadPart - t0.QuadPart);
            frameTime = static_cast<FLOAT>(timePerFrame / tf.QuadPart);
            fps = static_cast<UINT>((tf.QuadPart / timePerFrame) + 0.5);
            t0 = t1;

            // Present
            pSwapChain->Present(0, 0);
        }
    }

    // Release
    pSwapChain->SetFullscreenState(FALSE, NULL);

    pImmediateContext->ClearState();

    for(size_t i=0; i < strings.size(); ++i)
        strings[i].pTextLayout->Release();

    pTextGeometry->Release();
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
        if(wParam == 'S')
        {
            g_static = !g_static;
            g_staticComplete = false;
        }
        if(wParam == 'T')
            g_usePreconstructed = !g_usePreconstructed;
        if(wParam == 'B')
            g_batchStrings = !g_batchStrings;
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
