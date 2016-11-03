// MeasureText.cpp

// This example shows how to measure text
// IDWriteTextLayout::GetOverhangMetrics gives the bounding box of a text-layout

// The IFW1FontWrapper::MeasureString can be used to measure a string without constructing a text-layout
// (It constructs one and uses GetOverhangMetrics behind the scenes)

// This example requires feature level 10.0 because of how the rectangle is filled
// The actual measuring is done by DirectWrite and not dependent on D3D11

#include <D3D11.h>
#include <D3Dcompiler.h>
#include <cmath>
#include "FW1FontWrapper.h"

#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "FW1FontWrapper.lib")

// Helper class to draw a colored rectangle
class RectDrawer {
	public:
		RectDrawer();
		~RectDrawer();
		
		bool init(ID3D11Device *pDevice);
		void drawRect(ID3D11DeviceContext *pContext, float left, float top, float right, float bottom, UINT32 color);
	
	private:
		struct ShaderConstants {
			FLOAT TransformMatrix[16];
			FLOAT Color[4];
		};
		ID3D11PixelShader *m_pPixelShader;
		ID3D11VertexShader *m_pVertexShader;
		ID3D11Buffer *m_pBuffer;
		bool m_inited;
};


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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
		TEXT("D3D11 Font Wrapper - Measure Text"),
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
	
	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_10_0};
	IDXGISwapChain *pSwapChain;
	ID3D11Device *pDevice;
	ID3D11DeviceContext *pImmediateContext;
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hResult = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		featureLevels,
		sizeof(featureLevels)/sizeof(featureLevels[0]),
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		&featureLevel,
		&pImmediateContext
	);
	if(FAILED(hResult)) {
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
	if(FAILED(hResult)) {
		MessageBox(NULL, TEXT("FW1CreateFactory"), TEXT("Error"), MB_OK);
		return 0;
	}
	
	IFW1FontWrapper *pFontWrapper;
	hResult = pFW1Factory->CreateFontWrapper(pDevice, L"Arial", &pFontWrapper);
	if(FAILED(hResult)) {
		MessageBox(NULL, TEXT("CreateFontWrapper"), TEXT("Error"), MB_OK);
		return 0;
	}
	
	pFW1Factory->Release();
	
	// Get DWrite factory
	IDWriteFactory *pDWriteFactory;
	hResult = pFontWrapper->GetDWriteFactory(&pDWriteFactory);
	
	// Create the default DWrite text format for the text layout
	IDWriteTextFormat *pTextFormat;
	hResult = pDWriteFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_OBLIQUE,
		DWRITE_FONT_STRETCH_NORMAL,
		32.0f,
		L"",
		&pTextFormat
	);
	
	// Create a text layout for a string
	IDWriteTextLayout *pTextLayout;
	const WCHAR str[] = L"\"My Text Layout String\"\n\n123 !!!\n~~\n__ test";
	hResult = pDWriteFactory->CreateTextLayout(
		str,
		sizeof(str)/sizeof(str[0]),
		pTextFormat,
		0.0f,
		0.0f,
		&pTextLayout
	);
	pTextLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	
	pDWriteFactory->Release();
	
	// Helper to draw a rectangle
	RectDrawer *rectDrawer = new RectDrawer;
	rectDrawer->init(pDevice);
	
	// Main loop
	ShowWindow(hWnd, nCmdShow);
	
	bool loop = true;
	while(loop) {
		MSG msg;
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
			if(msg.message == WM_QUIT)
				loop = false;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			pImmediateContext->OMSetRenderTargets(1, &pRTV, NULL);
			
			FLOAT backgroundColor[4] = {0.2f, 0.3f, 0.4f, 1.0f};
			pImmediateContext->ClearRenderTargetView(pRTV, backgroundColor);
			
			FLOAT posX = 100.0f;
			FLOAT posY = 50.0f;
			
			// Get the layout measurements
			DWRITE_OVERHANG_METRICS overhangMetrics;
			pTextLayout->GetOverhangMetrics(&overhangMetrics);
			
			// Fill a rect to show the formatted text box
			rectDrawer->drawRect(
				pImmediateContext,
				floor(posX - overhangMetrics.left),
				floor(posY - overhangMetrics.top),
				ceil(posX + overhangMetrics.right),
				ceil(posY + overhangMetrics.bottom),
				0xff889977
			);
			
			// Draw the text layout
			pFontWrapper->DrawTextLayout(pImmediateContext, pTextLayout, posX, posY, 0xffffffff, 0);
			
			
			// Let the font-wrapper measure when no manually constructed layout exists
			// MeasureString takes the same flags and settings as DrawString
			posX = 500.0f;
			posY = 70.0f;
			FW1_RECTF layoutRect = {posX, posY, posX, posY};
			
			FW1_RECTF rect = pFontWrapper->MeasureString(str, L"Arial", 64.0f, &layoutRect, FW1_NOWORDWRAP);
			rectDrawer->drawRect(pImmediateContext, rect.Left, rect.Top, rect.Right, rect.Bottom, 0xffff9977);
			
			pFontWrapper->DrawString(pImmediateContext, str, L"Arial", 64.0f, &layoutRect, 0xffffffff, NULL, NULL, FW1_NOWORDWRAP);
			
			// Present
			pSwapChain->Present(0, 0);
		}
	}
	
	// Release
	pSwapChain->SetFullscreenState(FALSE, NULL);
	
	pImmediateContext->ClearState();
	
	delete rectDrawer;
	
	pTextLayout->Release();
	
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
		return 0;
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


// RectDrawer helper class

RectDrawer::RectDrawer() :
	m_pBuffer(NULL),
	m_pPixelShader(NULL),
	m_pVertexShader(NULL),
	m_inited(false)
{
}
		
RectDrawer::~RectDrawer() {
	if(m_pBuffer) m_pBuffer->Release();
	if(m_pPixelShader) m_pPixelShader->Release();
	if(m_pVertexShader) m_pVertexShader->Release();
}
		
bool RectDrawer::init(ID3D11Device *pDevice) {
	const char vsQuad[] =
	"cbuffer ShaderConstants : register(b0) {"
	"	float4x4 TransformMatrix : packoffset(c0);"
	"};"
	"float4 VS(uint VertexIndex : SV_VertexID) : SV_Position{"
	"	const float2 corners[4] = {"
	"		float2(0.0f, 1.0f),"
	"		float2(0.0f, 0.0f),"
	"		float2(1.0f, 1.0f),"
	"		float2(1.0f, 0.0f)"
	"	};"
	"	return mul(TransformMatrix, float4(corners[VertexIndex].xy, 0.0f, 1.0f));"
	"}";
	ID3DBlob *pCode;
	HRESULT hResult = D3DCompile(vsQuad, sizeof(vsQuad), NULL, NULL, NULL, "VS", "vs_4_0", 0, 0, &pCode, NULL);
	if(FAILED(hResult)) return false;
	hResult = pDevice->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVertexShader);
	pCode->Release();
	if(FAILED(hResult)) return false;
	
	const char psColor[] =
	"cbuffer ShaderConstants : register(b0) {"
	"	float4 Color : packoffset(c4);"
	"};"
	"float4 PS() : SV_Target {"
	"	return Color;"
	"}";
	hResult = D3DCompile(psColor, sizeof(psColor), NULL, NULL, NULL, "PS", "ps_4_0", 0, 0, &pCode, NULL);
	if(FAILED(hResult)) return false;
	hResult = pDevice->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPixelShader);
	pCode->Release();
	if(FAILED(hResult)) return false;
	
	D3D11_BUFFER_DESC bufferDesc = {0};
	bufferDesc.ByteWidth = sizeof(ShaderConstants);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hResult = pDevice->CreateBuffer(&bufferDesc, NULL, &m_pBuffer);
	if(FAILED(hResult)) return false;
	
	m_inited = true;
	
	return true;
}
		
void RectDrawer::drawRect(ID3D11DeviceContext *pContext, float left, float top, float right, float bottom, UINT32 color) {
	if(!m_inited)
		return;
	
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pContext->PSSetShader(m_pPixelShader, NULL, 0);
	pContext->PSSetConstantBuffers(0, 1, &m_pBuffer);
	pContext->VSSetShader(m_pVertexShader, NULL, 0);
	pContext->VSSetConstantBuffers(0, 1, &m_pBuffer);
	pContext->GSSetShader(NULL, NULL, 0);
	
	ShaderConstants constants = {0};
	
	D3D11_VIEWPORT vp;
	UINT nvp = 1;
	pContext->RSGetViewports(&nvp, &vp);
	constants.TransformMatrix[0] = (right - left) * 2.0f / vp.Width;
	constants.TransformMatrix[12] = -1.0f + left * 2.0f / vp.Width;
	constants.TransformMatrix[5] = (bottom - top) * -2.0f / vp.Height;
	constants.TransformMatrix[13] = 1.0f + top * -2.0f / vp.Height;
	constants.TransformMatrix[10] = 1.0f;
	constants.TransformMatrix[15] = 1.0f;
	
	for(int i=0;i<4;++i)
		constants.Color[i] = static_cast<FLOAT>((color >> (i << 3)) & 0xff) / 255.0f;
	
	pContext->UpdateSubresource(m_pBuffer, 0, NULL, &constants, 0, 0);
	
	pContext->Draw(4, 0);
}
