//----------------------------------------------------------------------------------
// File:        src\nvidiautils/DeviceManager.cpp
// SDK Version: v1.2 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

#include "DeviceManager.h"
#include <WinUser.h>
#include <Windows.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#define WINDOW_CLASS_NAME   L"NvDX11"

#define WINDOW_STYLE_NORMAL         (WS_OVERLAPPEDWINDOW | WS_VISIBLE)
#define WINDOW_STYLE_FULLSCREEN     (WS_POPUP | WS_SYSMENU | WS_VISIBLE)

// A singleton, sort of... To pass the events from WindowProc to the object.
DeviceManager* g_DeviceManagerInstance = NULL;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(g_DeviceManagerInstance)
        return g_DeviceManagerInstance->MsgProc(hWnd, uMsg, wParam, lParam);
    else
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


HRESULT
DeviceManager::CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, LPWSTR title)
{
    g_DeviceManagerInstance = this;
    m_WindowTitle = title;

    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEX windowClass = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProc,
                        0L, 0L, hInstance, NULL, NULL, NULL, NULL, WINDOW_CLASS_NAME, NULL };

    RegisterClassEx(&windowClass);

    UINT windowStyle = params.startFullscreen 
        ? WINDOW_STYLE_FULLSCREEN
        : params.startMaximized 
            ? (WINDOW_STYLE_NORMAL | WS_MAXIMIZE) 
            : WINDOW_STYLE_NORMAL;
        
    RECT rect = { 0, 0, params.backBufferWidth, params.backBufferHeight };
    AdjustWindowRect(&rect, windowStyle, FALSE);

    m_hWnd = CreateWindowEx(
        0,
        WINDOW_CLASS_NAME, 
        title, 
        windowStyle, 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
        GetDesktopWindow(),
        NULL,
        hInstance,
        NULL
    );

    if(!m_hWnd)
    {
#ifdef DEBUG
        DWORD errorCode = GetLastError();    
        printf("CreateWindowEx error code = 0x%x\n", errorCode);
#endif

        MessageBox(NULL, L"Cannot create window", m_WindowTitle.c_str(), MB_OK | MB_ICONERROR);
        return E_FAIL;
    }

    UpdateWindow(m_hWnd);

    HRESULT hr = E_FAIL;

    RECT clientRect;
    GetClientRect(m_hWnd, &clientRect);
    UINT width = clientRect.right - clientRect.left;
    UINT height = clientRect.bottom - clientRect.top;

    ZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));
    m_SwapChainDesc.BufferCount = params.swapChainBufferCount;
    m_SwapChainDesc.BufferDesc.Width = width;
    m_SwapChainDesc.BufferDesc.Height = height;
    m_SwapChainDesc.BufferDesc.Format = params.swapChainFormat;
    m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = params.refreshRate;
    m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
    m_SwapChainDesc.BufferUsage = params.swapChainUsage;
    m_SwapChainDesc.OutputWindow = m_hWnd;
    m_SwapChainDesc.SampleDesc.Count = params.swapChainSampleCount;
    m_SwapChainDesc.SampleDesc.Quality = params.swapChainSampleQuality;
    m_SwapChainDesc.Windowed = !params.startFullscreen;
    m_SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    hr = D3D11CreateDeviceAndSwapChain(
        NULL,                   // pAdapter
        params.driverType,      // DriverType
        NULL,                   // Software
        params.createDeviceFlags, // Flags
        &params.featureLevel,   // pFeatureLevels
        1,                      // FeatureLevels
        D3D11_SDK_VERSION,      // SDKVersion
        &m_SwapChainDesc,       // pSwapChainDesc
        &m_SwapChain,           // ppSwapChain
        &m_Device,              // ppDevice
        NULL,                   // pFeatureLevel
        &m_ImmediateContext     // ppImmediateContext
    );
    
    if(FAILED(hr))
        return hr;

    m_DepthStencilDesc.ArraySize = 1;
    m_DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    m_DepthStencilDesc.CPUAccessFlags = 0;
    m_DepthStencilDesc.Format = params.depthStencilFormat;
    m_DepthStencilDesc.Width = width;
    m_DepthStencilDesc.Height = height;
    m_DepthStencilDesc.MipLevels = 1;
    m_DepthStencilDesc.MiscFlags = 0;
    m_DepthStencilDesc.SampleDesc.Count = params.swapChainSampleCount;
    m_DepthStencilDesc.SampleDesc.Quality = 0;
    m_DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    
    hr = CreateRenderTargetAndDepthStencil();

    if(FAILED(hr))
        return hr;

    DeviceCreated();
    BackBufferResized();

    return S_OK;
}

void
DeviceManager::Shutdown() 
{   
    if(m_SwapChain && GetWindowState() == kWindowFullscreen)
        m_SwapChain->SetFullscreenState(false, NULL);

    DeviceDestroyed();

    SAFE_RELEASE(m_BackBufferRTV);
    SAFE_RELEASE(m_DepthStencilDSV);
    SAFE_RELEASE(m_DepthStencilBuffer);

    g_DeviceManagerInstance = NULL;
    SAFE_RELEASE(m_SwapChain);
    SAFE_RELEASE(m_Device);

    if(m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }
}

HRESULT
DeviceManager::CreateRenderTargetAndDepthStencil()
{
    HRESULT hr;

    ID3D11Texture2D *backBuffer = NULL; 
    hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
        return hr;

    hr = m_Device->CreateRenderTargetView(backBuffer, NULL, &m_BackBufferRTV);
    backBuffer->Release();
    if (FAILED(hr))
        return hr;

    if(m_DepthStencilDesc.Format != DXGI_FORMAT_UNKNOWN)
    {
        hr = m_Device->CreateTexture2D(&m_DepthStencilDesc, NULL, &m_DepthStencilBuffer);
        if (FAILED(hr))
            return hr;

        hr = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, NULL, &m_DepthStencilDSV);
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}

void
DeviceManager::MessageLoop() 
{
    MSG msg = {0};

    LARGE_INTEGER perfFreq, previousTime;
    QueryPerformanceFrequency(&perfFreq);
    QueryPerformanceCounter(&previousTime);
    
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            LARGE_INTEGER newTime;
            QueryPerformanceCounter(&newTime);
            
            double elapsedSeconds = (m_FixedFrameInterval >= 0) 
                ? m_FixedFrameInterval  
                : (double)(newTime.QuadPart - previousTime.QuadPart) / (double)perfFreq.QuadPart;

            if(m_SwapChain && GetWindowState() != kWindowMinimized)
            {
                Animate(elapsedSeconds);
                Render(); 
                m_SwapChain->Present(m_SyncInterval, 0);
                Sleep(0);
            }
            else
            {
                // Release CPU resources when idle
                Sleep(1);
            }

            {
                m_vFrameTimes.push_back(elapsedSeconds);
                double timeSum = 0;
                for(auto it = m_vFrameTimes.begin(); it != m_vFrameTimes.end(); it++)
                    timeSum += *it;

                if(timeSum > m_AverageTimeUpdateInterval)
                {
                    m_AverageFrameTime = timeSum / (double)m_vFrameTimes.size();
                    m_vFrameTimes.clear();
                }
            }

            previousTime = newTime;
        }
    }
}

LRESULT 
DeviceManager::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_DESTROY:
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_SYSKEYDOWN:
            if(wParam == VK_F4)
            {
                PostQuitMessage(0);
                return 0;
            }
            break;

        case WM_SIZE:
            // Ignore the WM_SIZE event if there is no device,
            // or if the window has been minimized (size == 0),
            // or if it has been restored to the previous size
            if (m_Device 
                && (lParam != 0) 
                && (LOWORD(lParam) != m_SwapChainDesc.BufferDesc.Width || HIWORD(lParam) != m_SwapChainDesc.BufferDesc.Height))
            {
                ID3D11RenderTargetView *nullRTV = NULL;
                m_ImmediateContext->OMSetRenderTargets(1, &nullRTV, NULL);
                SAFE_RELEASE(m_BackBufferRTV);
                SAFE_RELEASE(m_DepthStencilDSV);
                SAFE_RELEASE(m_DepthStencilBuffer);

                if (m_SwapChain)
                {
                    // Resize the swap chain
                    m_SwapChainDesc.BufferDesc.Width = LOWORD(lParam);
                    m_SwapChainDesc.BufferDesc.Height = HIWORD(lParam);
                    m_SwapChain->ResizeBuffers(m_SwapChainDesc.BufferCount, m_SwapChainDesc.BufferDesc.Width, 
                                               m_SwapChainDesc.BufferDesc.Height, m_SwapChainDesc.BufferDesc.Format, 
                                               m_SwapChainDesc.Flags);
                    
                    m_DepthStencilDesc.Width = LOWORD(lParam);
                    m_DepthStencilDesc.Height = HIWORD(lParam);

                    CreateRenderTargetAndDepthStencil();

                    BackBufferResized();
                }
            }
    }

    if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST || 
        uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST )
    {
        // processing messages front-to-back
        for(auto it = m_vControllers.begin(); it != m_vControllers.end(); it++)
        {
            if((*it)->IsEnabled())
            {
                // for kb/mouse messages, 0 means the message has been handled
                if(0 == (*it)->MsgProc(hWnd, uMsg, wParam, lParam))
                    return 0;
            }
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void
DeviceManager::Render()
{
    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (float)m_SwapChainDesc.BufferDesc.Width, (float)m_SwapChainDesc.BufferDesc.Height, 0.0f, 1.0f };

    // rendering back-to-front
    for(auto it = m_vControllers.rbegin(); it != m_vControllers.rend(); it++)
    {
        if((*it)->IsEnabled())
        {
            m_ImmediateContext->OMSetRenderTargets(1, &m_BackBufferRTV, m_DepthStencilDSV);
            m_ImmediateContext->RSSetViewports(1, &viewport);

            (*it)->Render(m_Device, m_ImmediateContext, m_BackBufferRTV, m_DepthStencilDSV);
        }
    }

    m_ImmediateContext->OMSetRenderTargets(0, NULL, NULL);
}

void
DeviceManager::Animate(double fElapsedTimeSeconds)
{
    for(auto it = m_vControllers.begin(); it != m_vControllers.end(); it++)
    {
        if((*it)->IsEnabled())
        {
            (*it)->Animate(fElapsedTimeSeconds);
        }
    }
}

void
DeviceManager::DeviceCreated()
{
    for(auto it = m_vControllers.begin(); it != m_vControllers.end(); it++)
    {
        (*it)->DeviceCreated(m_Device);
    }
}

void
DeviceManager::DeviceDestroyed()
{
    for(auto it = m_vControllers.begin(); it != m_vControllers.end(); it++)
    {
        (*it)->DeviceDestroyed();
    }
}

void
DeviceManager::BackBufferResized()
{
    if(m_SwapChain == NULL)
        return;

    DXGI_SURFACE_DESC backSD;
    backSD.Format = m_SwapChainDesc.BufferDesc.Format;
    backSD.Width = m_SwapChainDesc.BufferDesc.Width;
    backSD.Height = m_SwapChainDesc.BufferDesc.Height;
    backSD.SampleDesc = m_SwapChainDesc.SampleDesc;

    for(auto it = m_vControllers.begin(); it != m_vControllers.end(); it++)
    {
        (*it)->BackBufferResized(m_Device, &backSD);
    }
}

HRESULT
DeviceManager::ChangeBackBufferFormat(DXGI_FORMAT format, UINT sampleCount)
{
    HRESULT hr = E_FAIL;

    if((format == DXGI_FORMAT_UNKNOWN || format == m_SwapChainDesc.BufferDesc.Format) &&
       (sampleCount == 0 || sampleCount == m_SwapChainDesc.SampleDesc.Count))
        return S_FALSE;

    if(m_Device)
    {
        bool fullscreen = (GetWindowState() == kWindowFullscreen);
        if(fullscreen)
            m_SwapChain->SetFullscreenState(false, NULL);

        IDXGISwapChain* newSwapChain = NULL;
        DXGI_SWAP_CHAIN_DESC newSwapChainDesc = m_SwapChainDesc;

        if(format != DXGI_FORMAT_UNKNOWN)
            newSwapChainDesc.BufferDesc.Format = format;
        if(sampleCount != 0)
            newSwapChainDesc.SampleDesc.Count = sampleCount;

        IDXGIAdapter* pDXGIAdapter = GetDXGIAdapter();

        IDXGIFactory* pDXGIFactory = NULL;
        pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pDXGIFactory));

        hr = pDXGIFactory->CreateSwapChain(m_Device, &newSwapChainDesc, &newSwapChain);

        pDXGIFactory->Release();
        pDXGIAdapter->Release();
        
        if (FAILED(hr))
        {
            if(fullscreen)
                m_SwapChain->SetFullscreenState(true, NULL);

            return hr;
        }

        SAFE_RELEASE(m_BackBufferRTV);
        SAFE_RELEASE(m_SwapChain);
        SAFE_RELEASE(m_DepthStencilBuffer);
        SAFE_RELEASE(m_DepthStencilDSV);

        m_SwapChain = newSwapChain;
        m_SwapChainDesc = newSwapChainDesc;

        m_DepthStencilDesc.SampleDesc.Count = sampleCount;

        if(fullscreen)                
            m_SwapChain->SetFullscreenState(true, NULL);

        CreateRenderTargetAndDepthStencil();
        BackBufferResized();
    }

    return S_OK;
}

void
DeviceManager::AddControllerToFront(IVisualController* pController) 
{ 
    m_vControllers.remove(pController);
    m_vControllers.push_front(pController);
}

void
DeviceManager::AddControllerToBack(IVisualController* pController) 
{
    m_vControllers.remove(pController);
    m_vControllers.push_back(pController);
}

void
DeviceManager::RemoveController(IVisualController* pController) 
{ 
    m_vControllers.remove(pController);
}

HRESULT 
DeviceManager::ResizeWindow(int width, int height)
{
    if(m_SwapChain == NULL)
        return E_FAIL;

    RECT rect;
    GetWindowRect(m_hWnd, &rect);

    ShowWindow(m_hWnd, SW_RESTORE);

    if(!MoveWindow(m_hWnd, rect.left, rect.top, width, height, true))
        return E_FAIL;

    // No need to call m_SwapChain->ResizeBackBuffer because MoveWindow will send WM_SIZE, which calls that function.

    return S_OK;
}

HRESULT            
DeviceManager::EnterFullscreenMode(int width, int height)
{
    if(m_SwapChain == NULL)
        return E_FAIL;

    if(GetWindowState() == kWindowFullscreen)
        return S_FALSE;
    
    if(width <= 0 || height <= 0)
    {
        width = m_SwapChainDesc.BufferDesc.Width;
        height = m_SwapChainDesc.BufferDesc.Height;
    }
     
    SetWindowLong(m_hWnd, GWL_STYLE, WINDOW_STYLE_FULLSCREEN);
    MoveWindow(m_hWnd, 0, 0, width, height, true);

    HRESULT hr = m_SwapChain->SetFullscreenState(true, NULL);

    if(FAILED(hr)) 
    {
        SetWindowLong(m_hWnd, GWL_STYLE, WINDOW_STYLE_NORMAL);
        return hr;
    }

    UpdateWindow(m_hWnd);
    m_SwapChain->GetDesc(&m_SwapChainDesc);

    return S_OK;
}

HRESULT            
DeviceManager::LeaveFullscreenMode(int windowWidth, int windowHeight)
{
    if(m_SwapChain == NULL)
        return E_FAIL;

    if(GetWindowState() != kWindowFullscreen)
        return S_FALSE;
     
    HRESULT hr = m_SwapChain->SetFullscreenState(false, NULL);
    if(FAILED(hr)) return hr;

    SetWindowLong(m_hWnd, GWL_STYLE, WINDOW_STYLE_NORMAL);

    if(windowWidth <= 0 || windowHeight <= 0)
    {
        windowWidth = m_SwapChainDesc.BufferDesc.Width;
        windowHeight = m_SwapChainDesc.BufferDesc.Height;
    }
    
    RECT rect = { 0, 0, windowWidth, windowHeight };
    AdjustWindowRect(&rect, WINDOW_STYLE_NORMAL, FALSE);
    MoveWindow(m_hWnd, 0, 0, rect.right - rect.left, rect.bottom - rect.top, true);
    UpdateWindow(m_hWnd);

    m_SwapChain->GetDesc(&m_SwapChainDesc);

    return S_OK;
}

HRESULT            
DeviceManager::ToggleFullscreen()
{
    if(GetWindowState() == kWindowFullscreen)
        return LeaveFullscreenMode();
    else
        return EnterFullscreenMode();
}

DeviceManager::WindowState
DeviceManager::GetWindowState() 
{ 
    if(m_SwapChain && !m_SwapChainDesc.Windowed)
        return kWindowFullscreen;

    if(m_hWnd == INVALID_HANDLE_VALUE)
        return kWindowNone;

    if(IsZoomed(m_hWnd))
        return kWindowMaximized;

    if(IsIconic(m_hWnd))
        return kWindowMinimized;

    return kWindowNormal;
}

HRESULT
DeviceManager::GetDisplayResolution(int& width, int& height)
{
    if(m_hWnd != INVALID_HANDLE_VALUE)
    {
        HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO info;
        info.cbSize = sizeof(MONITORINFO);

        if(GetMonitorInfo(monitor, &info))
        {
            width = info.rcMonitor.right - info.rcMonitor.left;
            height = info.rcMonitor.bottom - info.rcMonitor.top;
            return S_OK;
        }
    }

    return E_FAIL;
}

IDXGIAdapter*   
DeviceManager::GetDXGIAdapter()
{   
    if(!m_Device)
        return NULL;

    IDXGIDevice* pDXGIDevice = NULL;
    m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDXGIDevice));

    IDXGIAdapter* pDXGIAdapter = NULL;
    pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDXGIAdapter));

    pDXGIDevice->Release();

    return pDXGIAdapter;
}
