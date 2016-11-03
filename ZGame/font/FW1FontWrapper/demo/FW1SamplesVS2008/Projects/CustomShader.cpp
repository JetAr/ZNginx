// CustomShader.cpp

// This example shows how to override shaders used by the font-wrapper

#include <D3D11.h>
#include <D3Dcompiler.h>
#include "FW1FontWrapper.h"

#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "FW1FontWrapper.lib")


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main
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
                    TEXT("D3D11 Font Wrapper - Custom Shader"),
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

    pFW1Factory->Release();

    // Get the default font render states
    IFW1GlyphRenderStates *pRenderStates;
    pFontWrapper->GetRenderStates(&pRenderStates);

    // Replacement pixel shader
    const char psStr[] =
        "cbuffer ShaderConstants : register(b0) {\r\n"
        "	float4 TextColor : packoffset(c4);\r\n"
        "};\r\n"
        "\r\n"
        "SamplerState sampler0 : register(s0);\r\n"
        "Texture2D<float> tex0 : register(t0);\r\n"
        "\r\n"
        "struct PSIn {\r\n"
        "	float4 Position : SV_Position;\r\n"
        "	float4 GlyphColor : COLOR;\r\n"
        "	float2 TexCoord : TEXCOORD;\r\n"
        "};\r\n"
        "\r\n"
        "float4 PS(PSIn Input) : SV_Target {\r\n"
        "	float a = tex0.Sample(sampler0, Input.TexCoord);\r\n"
        "	\r\n"
        "	if(a == 0.0f)\r\n"
        "		discard;\r\n"
        "	\r\n"
        "	float4 col = sin(Input.TexCoord.xy * 100.0f).xyxy;\r\n"
        "	return a * col;\r\n"
        "}\r\n"
        "";

    ID3DBlob *pPSCode;
    D3DCompile(psStr, sizeof(psStr), NULL, NULL, NULL, "PS", "ps_4_0_level_9_1", 0, 0, &pPSCode, NULL);
    ID3D11PixelShader *pPixelShader;
    pDevice->CreatePixelShader(pPSCode->GetBufferPointer(), pPSCode->GetBufferSize(), NULL, &pPixelShader);
    pPSCode->Release();

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
            // Render frame
            pImmediateContext->OMSetRenderTargets(1, &pRTV, NULL);

            FLOAT backgroundColor[4] = {0.2f, 0.3f, 0.4f, 1.0f};
            pImmediateContext->ClearRenderTargetView(pRTV, backgroundColor);

            // Set the default rendering states
            pRenderStates->SetStates(pImmediateContext, 0);

            // Override pixel shader
            pImmediateContext->PSSetShader(pPixelShader, NULL, 0);

            pFontWrapper->DrawString(
                pImmediateContext,
                L"中 Centered Text",
                128.0f,
                vp.Width/2.0f,
                vp.Height/2.0f,
                0xffffaa00,
                FW1_CENTER | FW1_VCENTER | FW1_STATEPREPARED// Use the FW1_STATEPREPARED flag
            );

            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hWnd, &pt);
            pFontWrapper->DrawString(
                pImmediateContext,
                L"!",
                L"Webdings",
                256.0f,
                static_cast<FLOAT>(pt.x),
                static_cast<FLOAT>(pt.y),
                0xffffaa7f,
                FW1_CENTER | FW1_VCENTER | FW1_IMMEDIATECALL
            );

            // Present
            pSwapChain->Present(0, 0);
        }
    }

    // Release
    pSwapChain->SetFullscreenState(FALSE, NULL);

    pImmediateContext->ClearState();

    pPixelShader->Release();

    pRenderStates->Release();
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
