// TextLayouts.cpp

// Example of drawing DirectWrite text layouts
// This example constructs a few DirectWrite text layouts, with a few different drawing effects and font features
// These are drawn with IFW1FontWrapper::DrawTextLayout

#include <D3D11.h>
#include "FW1FontWrapper.h"

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
                    TEXT("D3D11 Font Wrapper - DirectWrite Text Layouts"),
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

    // Get the DirectWrite factory used by the font-wrapper
    IDWriteFactory *pDWriteFactory;
    hResult = pFontWrapper->GetDWriteFactory(&pDWriteFactory);

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


    // Create a text layout for a string
    IDWriteTextLayout *pTextLayout1;
    const WCHAR str1[] = L"ANCIENT GREEK MYTHOLOGY";
    pDWriteFactory->CreateTextLayout(
        str1,
        sizeof(str1)/sizeof(str1[0]),
        pTextFormat,
        0.0f,
        0.0f,
        &pTextLayout1
    );

    // No word wrapping
    pTextLayout1->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    pTextLayout1->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    // Set up typography features
    IDWriteTypography *pTypography;
    pDWriteFactory->CreateTypography(&pTypography);
    DWRITE_FONT_FEATURE fontFeature;
    fontFeature.nameTag = DWRITE_FONT_FEATURE_TAG_STYLISTIC_ALTERNATES;
    fontFeature.parameter = 1;
    pTypography->AddFontFeature(fontFeature);

    // Apply the typography features to select ranges in the string
    DWRITE_TEXT_RANGE textRange;

    textRange.startPosition = 0;
    textRange.length = 1;
    pTextLayout1->SetTypography(pTypography, textRange);
    textRange.startPosition = 10;
    textRange.length = 2;
    pTextLayout1->SetTypography(pTypography, textRange);
    textRange.startPosition = 18;
    textRange.length = 1;
    pTextLayout1->SetTypography(pTypography, textRange);

    pTypography->Release();

    // Set the font and size
    textRange.startPosition = 0;
    textRange.length = sizeof(str1)/sizeof(str1[0]);
    pTextLayout1->SetFontSize(64.0f, textRange);
    pTextLayout1->SetFontFamilyName(L"Pericles", textRange);


    // Create another text layout
    IDWriteTextLayout *pTextLayout2;
    const WCHAR str2[] = L"A B C D E F G H I J K L M N\nA B C D E F G H I J K L M N";
    pDWriteFactory->CreateTextLayout(
        str2,
        sizeof(str2)/sizeof(str2[0]),
        pTextFormat,
        0.0f,
        0.0f,
        &pTextLayout2
    );
    pTextLayout2->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    pTextLayout2->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    textRange.startPosition = 0;
    textRange.length = sizeof(str2)/sizeof(str2[0]);
    pTextLayout2->SetFontSize(48.0f, textRange);
    pTextLayout2->SetFontFamilyName(L"Pescadero", textRange);

    // Add font feature
    fontFeature.nameTag = DWRITE_FONT_FEATURE_TAG_SWASH;
    fontFeature.parameter = 1;
    pDWriteFactory->CreateTypography(&pTypography);
    pTypography->AddFontFeature(fontFeature);
    textRange.startPosition = 28;
    textRange.length = sizeof(str2)/sizeof(str2[0]) - 28;
    pTextLayout2->SetTypography(pTypography, textRange);
    pTypography->Release();

    // Set color overrides for select ranges
    IFW1ColorRGBA *pColorRed;
    pFW1Factory->CreateColor(0xff0000ff, &pColorRed);
    IFW1ColorRGBA *pColorGreen;
    pFW1Factory->CreateColor(0xff00ff00, &pColorGreen);

    textRange.startPosition = 2;
    textRange.length = 8;
    pTextLayout2->SetDrawingEffect(pColorRed, textRange);

    textRange.startPosition = 22;
    textRange.length = 15;
    pTextLayout2->SetDrawingEffect(pColorGreen, textRange);

    pColorRed->Release();
    pColorGreen->Release();


    // And a third text layout..
    IDWriteTextLayout *pTextLayout3;
    const WCHAR codeStr[] = L"#include <iostream>\n\nint main() {\n    int num;\n    std::cin >> num;\n\n    for(int i=0; i < num; ++i)\n        std::cout << \"Hello World!\\n\";\n\n    return 0;\n}\n";
    pDWriteFactory->CreateTextLayout(
        codeStr,
        sizeof(codeStr)/sizeof(codeStr[0]),
        pTextFormat,
        0,
        0.0f,
        &pTextLayout3
    );
    pTextLayout3->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    textRange.startPosition = 0;
    textRange.length = sizeof(codeStr)/sizeof(codeStr[0]);
    pTextLayout3->SetFontSize(13.333333333f, textRange);
    pTextLayout3->SetFontFamilyName(L"Courier New", textRange);


    pFW1Factory->Release();
    pDWriteFactory->Release();

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

            // Draw the text-layouts
            pFontWrapper->DrawTextLayout(pImmediateContext, pTextLayout1, vp.Width/2.0f, 250.0f, 0xffffffff, 0);

            pFontWrapper->DrawTextLayout(pImmediateContext, pTextLayout2, vp.Width/2.0f, 350.0f, 0xff00aaff, FW1_IMMEDIATECALL);

            pFontWrapper->DrawTextLayout(
                pImmediateContext,
                pTextLayout3,
                vp.Width/2.0f-100.0f,
                20.0f,
                0xffffffff,
                FW1_ALIASED | FW1_IMMEDIATECALL
            );

            // Present
            pSwapChain->Present(0, 0);
        }
    }

    // Release
    pSwapChain->SetFullscreenState(FALSE, NULL);

    pImmediateContext->ClearState();

    pTextLayout1->Release();
    pTextLayout2->Release();
    pTextLayout3->Release();

    pTextFormat->Release();

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
