#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutModel_DX10.h"
#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVSConstBuffer	= NULL;
static ID3D10Buffer			*g_pVertexBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;

static ID3D10RasterizerState *g_pRasterizerState= NULL;
static ID3D10SamplerState	*g_pSamplerState = NULL;

static ID3D10Texture2D		*g_pTexture = NULL;
static ID3D10Texture2D		*g_pDepthStencilTexture = NULL;

static ID3D10ShaderResourceView *g_pTextureView = NULL;
static ID3D10RenderTargetView	*g_pRGBAView = NULL;
static ID3D10DepthStencilView	*g_pDepthStencilView = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_mirror_view_matrix;

static CGutModel_DX10 g_Model_DX10;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Vertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/texture_matrix_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// 載入Pixel Shader
	g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/texture_matrix_dx10.hlsl", "PS", "ps_4_0");
	if ( NULL==g_pPixelShader )
		return false;

	CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl");

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX10.ConvertToDX10Model(&g_Model);

	// 設定Vertex資料格式
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

	if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), 
		pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
		return false;

	SAFE_RELEASE(pVSCode);

	D3D10_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(cbDesc));

	// 配置Vertex Buffer
	{
		cbDesc.ByteWidth = sizeof(Vertex_V) * 4;
		cbDesc.Usage = D3D10_USAGE_DYNAMIC ;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA subDesc;
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = g_Quad;

		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pVertexBuffer ) )
			return false;
	}

	// 配置Vertex Shader 常數記憶體
	{
		cbDesc.ByteWidth = sizeof(Matrix4x4) * 2;
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;

		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pVSConstBuffer ) )
			return false;
	}

	// 配置RGBA動態貼圖
	{
		D3D10_TEXTURE2D_DESC dstex;
		dstex.Width = 512;
		dstex.Height = 512;
		dstex.MipLevels = 1;
		dstex.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dstex.SampleDesc.Count = 1;
		dstex.SampleDesc.Quality = 0;
		dstex.Usage = D3D10_USAGE_DEFAULT;
		dstex.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		dstex.CPUAccessFlags = 0;
		dstex.MiscFlags = 0;
		dstex.ArraySize = 1;

		g_pDevice->CreateTexture2D( &dstex, NULL, &g_pTexture );

		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
		SRVDesc.Format = dstex.Format;
		SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		// for shader
		g_pDevice->CreateShaderResourceView(g_pTexture, &SRVDesc, &g_pTextureView);

		D3D10_RENDER_TARGET_VIEW_DESC DescRT;
		DescRT.Format = dstex.Format;
		DescRT.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		DescRT.Texture2D.MipSlice = 0;
		// for rendertarget
		g_pDevice->CreateRenderTargetView(g_pTexture, &DescRT, &g_pRGBAView);
	}

	// 配置DepthStencil動態貼圖
	{
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );

		desc.Width =  512;
		desc.Height =  512;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.BindFlags = D3D10_BIND_DEPTH_STENCIL;

		g_pDevice->CreateTexture2D( &desc, NULL, &g_pDepthStencilTexture );

		D3D10_DEPTH_STENCIL_VIEW_DESC target_desc;
		ZeroMemory( &target_desc, sizeof(target_desc) );

		target_desc.Format = desc.Format;
		target_desc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		target_desc.Texture2D.MipSlice = 0;

		g_pDevice->CreateDepthStencilView(g_pDepthStencilTexture, &target_desc, &g_pDepthStencilView);
	}

	// Rasterizer State物件
	{
		D3D10_RASTERIZER_DESC rasterizer_state_desc;

		rasterizer_state_desc.FillMode = D3D10_FILL_SOLID;
		//rasterizer_state_desc.CullMode = D3D10_CULL_BACK;
		rasterizer_state_desc.CullMode = D3D10_CULL_NONE;
		rasterizer_state_desc.FrontCounterClockwise = true;
		rasterizer_state_desc.DepthBias = 0;
		rasterizer_state_desc.DepthBiasClamp = 0.0f;
		rasterizer_state_desc.SlopeScaledDepthBias = 0.0f;
		rasterizer_state_desc.DepthClipEnable = false;
		rasterizer_state_desc.ScissorEnable = false;
		rasterizer_state_desc.MultisampleEnable = false;
		rasterizer_state_desc.AntialiasedLineEnable = false;

		if ( D3D_OK != g_pDevice->CreateRasterizerState(&rasterizer_state_desc, &g_pRasterizerState) )
			return false;

		g_pDevice->RSSetState(g_pRasterizerState);
	}

	// sampler state / texture filter
	{
		D3D10_SAMPLER_DESC sampler_desc;
		ZeroMemory(&sampler_desc, sizeof(sampler_desc));

		sampler_desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;

		g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState);

		for ( int i=0; i<4; i++ )
		{
			g_pDevice->PSSetSamplers(i, 1, &g_pSamplerState);
		}
	}

	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pRasterizerState);
	SAFE_RELEASE(g_pVSConstBuffer);
	SAFE_RELEASE(g_pSamplerState);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();

	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, aspect, 0.1f, 100.0f);
}

void RenderModelDX10(bool mirror, Vector4 *pPlane)
{
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();

	Matrix4x4 view_matrix;
	Matrix4x4 proj_matrix;
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	if ( mirror )
	{
		Vector4 vEye = g_Control.GetCameraPosition();
		Vector4 vUp = g_Control.m_vUp;
		Vector4 vLookAt = g_Control.m_vLookAt;

		Vector4 mirror_eye = MirrorPoint(vEye, *pPlane);
		Vector4 mirror_lookat = MirrorPoint(vLookAt, *pPlane);
		Vector4 mirror_up = MirrorPoint(vUp, *pPlane);

		view_matrix = GutMatrixLookAtRH(mirror_eye, mirror_lookat, mirror_up);
		g_mirror_view_matrix = view_matrix;
	}
	else
	{
		view_matrix = g_Control.GetViewMatrix();
	}

	// CGutModel_DX10中有提供一些模擬Fixed Piepline的函式
	CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
	CGutModel_DX10::SetViewMatrix(view_matrix);
	CGutModel_DX10::SetWorldMatrix(world_matrix);
	CGutModel_DX10::UpdateMatrix();

	g_Model_DX10.Render();
}

void RenderFrameDX10(void)
{
	Vector4 vColorDarkBlue(0.0f, 0.0f, 0.5f, 1.0f);
	Vector4 vColorBlue(0.0f, 0.0f, 0.8f, 1.0f);
	Vector4 vPlane(0.0f, 0.0f, 1.0f, -g_mirror_z);

	D3D10_VIEWPORT mainVP, VP;

	// 取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer


	// 在動態貼圖中畫出鏡射的茶壼		
	{
		UINT nViewports = 1;
		g_pDevice->RSGetViewports(&nViewports, &mainVP);

		// new rendertarget viewport size
		VP.TopLeftX = VP.TopLeftY = 0;
		VP.Width = VP.Height = 512;
		VP.MinDepth = 0.0f;
		VP.MaxDepth = 1.0f;

		ID3D10ShaderResourceView *null_views[4] = {NULL, NULL, NULL, NULL};
		g_pDevice->PSSetShaderResources(0, 4, null_views);
		// 使用代表動態貼圖的RenderTarget
		g_pDevice->OMSetRenderTargets(1, &g_pRGBAView, g_pDepthStencilView);
		//
		g_pDevice->RSSetViewports(1, &VP);
		// 清除顏色
		g_pDevice->ClearRenderTargetView(g_pRGBAView, (float *)&vColorDarkBlue);
		// 清除Depth/Stencil buffer
		g_pDevice->ClearDepthStencilView(g_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
		// 畫鏡射的茶壼
		RenderModelDX10(true, &vPlane);
	}

	// 在主Framebuffer中畫出正常的茶壼
	{
		// 使用主framebuffer
		g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
		//
		g_pDevice->RSSetViewports(1, &mainVP);
		// 清除顏色
		g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vColorBlue);
		// 清除Depth/Stencil buffer
		g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
		// 畫茶壼
		RenderModelDX10(false, &vPlane);
	}

	// 在主framebuffer中畫出鏡面
	{
		UINT stride = sizeof(Vertex_V);
		UINT offset = 0;
		// 設定vertex shader
		g_pDevice->VSSetShader(g_pVertexShader);
		// 設定pixel shader
		g_pDevice->PSSetShader(g_pPixelShader);
		// 設定Shader Constants
		g_pDevice->VSSetConstantBuffers(0, 1, &g_pVSConstBuffer);
		// 設定vertex資料格式
		g_pDevice->IASetInputLayout(g_pVertexLayout);
		// 設定三角形頂點索引值資料排列是triangle strip
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		// Vertex Buffer
		g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
		// 計算texture matrix
		Matrix4x4 uv_offset_matrix;
		uv_offset_matrix.Scale_Replace(0.5f, -0.5f, 1.0f);
		uv_offset_matrix[3].Set(0.5f, 0.5f, 0.0f, 1.0f);
		Matrix4x4 texture_matrix = g_mirror_view_matrix * g_proj_matrix * uv_offset_matrix;
		// 計算矩陣
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 wvp_matrix = view_matrix * g_proj_matrix;
		// 更新shader參數
		Matrix4x4 *pConstData;
		g_pVSConstBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		pConstData[0] = wvp_matrix;
		pConstData[1] = texture_matrix;
		g_pVSConstBuffer->Unmap();
		// 套用第1張貼圖
		g_pDevice->PSSetShaderResources(0, 1, &g_pTextureView);
		// 畫出鏡面
		g_pDevice->Draw(4, 0);
	}

	// 等待硬體掃結束, 然後才更新畫面
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_