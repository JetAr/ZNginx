#ifdef _ENABLE_DX10_

#include "render_data.h"

#include "Gut.h"
#include "GutModel_DX10.h"

static ID3D10Device				*g_pDevice = NULL;

static ID3D10RasterizerState	*g_pRasterizerState = NULL;
static ID3D10SamplerState		*g_pSamplerState = NULL;

static ID3D10Texture2D			*g_pTexture =  NULL;
static ID3D10Texture2D			*g_pDepthStencilTexture = NULL;
static ID3D10ShaderResourceView *g_pCubemapView = NULL;
static ID3D10RenderTargetView	*g_pRGBAView[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
static ID3D10DepthStencilView	*g_pDepthStencilView = NULL;

static CGutModel_DX10 g_Model_DX10[4];

static Matrix4x4 g_proj_matrix;

static const int g_Width = 512;
static const int g_Height = 512;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	// 載入畫模型專用, 模擬fixed pipeline的shader
	if ( !CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl") )
		return false;

	CGutModel::SetTexturePath("../../textures/");

	// 把模型轉換成direct3d10格式
	for ( int m=0; m<4; m++ )
	{
		g_Model_DX10[m].ConvertToDX10Model(&g_Models[m]);
	}

	// 配置Cubemap 6個面的動態貼圖
	{
		// 配置6張貼圖, arrisze = 6, 
		D3D10_TEXTURE2D_DESC dstex;
		ZeroMemory( &dstex, sizeof(dstex) );

		dstex.Width = g_Width;
		dstex.Height = g_Height;
		dstex.MipLevels = 1;
		// size = 6
		dstex.ArraySize = 6;
		dstex.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dstex.SampleDesc.Count = 1;
		dstex.SampleDesc.Quality = 0;
		dstex.Usage = D3D10_USAGE_DEFAULT;
		dstex.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		dstex.CPUAccessFlags = 0;
		// 要記得加上 D3D10_RESOURCE_MISC_TEXTURECUBE
		dstex.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

		g_pDevice->CreateTexture2D( &dstex, NULL, &g_pTexture);

		// shader resource view
		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory( &SRVDesc, sizeof(SRVDesc) );

		SRVDesc.Format = dstex.Format;
		SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
		SRVDesc.Texture2D.MipLevels = 1;

		g_pDevice->CreateShaderResourceView(g_pTexture, &SRVDesc, &g_pCubemapView);

		// render target view
		for ( int i=0; i<6; i++ )
		{
			D3D10_RENDER_TARGET_VIEW_DESC DescRT;
			ZeroMemory( &DescRT, sizeof(DescRT) );

			DescRT.Format = dstex.Format;
			DescRT.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
			DescRT.Texture2DArray.MipSlice = 0;
			DescRT.Texture2DArray.FirstArraySlice = i;
			DescRT.Texture2DArray.ArraySize = 1;

			g_pDevice->CreateRenderTargetView(g_pTexture, &DescRT, &g_pRGBAView[i]);
		}
	}

	// 配置DepthStencil動態貼圖
	{
		// depth stencil的大小必需跟render target相符
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );

		desc.Width =  g_Width;
		desc.Height =  g_Height;
		desc.MipLevels = 1;
		desc.ArraySize = 6;
		desc.SampleDesc.Count = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		g_pDevice->CreateTexture2D( &desc, NULL, &g_pDepthStencilTexture );

		D3D10_DEPTH_STENCIL_VIEW_DESC target_desc;
		ZeroMemory( &target_desc, sizeof(target_desc) );

		target_desc.Format = desc.Format;
		target_desc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DARRAY;
		target_desc.Texture2DArray.MipSlice = 0;
		target_desc.Texture2DArray.ArraySize = 1;

		g_pDevice->CreateDepthStencilView(g_pDepthStencilTexture, &target_desc, &g_pDepthStencilView);
	}

	// Rasterizer State物件
	{
		D3D10_RASTERIZER_DESC rasterizer_state_desc;
		ZeroMemory( &rasterizer_state_desc, sizeof(rasterizer_state_desc) );

		rasterizer_state_desc.FillMode = D3D10_FILL_SOLID;
		rasterizer_state_desc.CullMode = D3D10_CULL_BACK;
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
	SAFE_RELEASE(g_pDepthStencilView);

	for ( int i=0; i<6; i++ )
	{
		SAFE_RELEASE(g_pRGBAView[i]);
	}

	SAFE_RELEASE(g_pCubemapView);
	SAFE_RELEASE(g_pTexture);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	// 重設水平跟垂直方向的視角, 并重新計算投影矩陣
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, aspect, 0.1f, 100.0f);
}

static void RenderSolarSystem(void)
{
	// 地球
	CGutModel_DX10::SetWorldMatrix(g_earth_matrix);
	CGutModel_DX10::UpdateMatrix();
	g_Model_DX10[1].Render();
	// 月亮
	CGutModel_DX10::SetWorldMatrix(g_moon_matrix);
	CGutModel_DX10::UpdateMatrix();
	g_Model_DX10[2].Render();
	// 火星
	CGutModel_DX10::SetWorldMatrix(g_mars_matrix);
	CGutModel_DX10::UpdateMatrix();
	g_Model_DX10[3].Render();
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// `取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件`
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); 
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 

	D3D10_VIEWPORT mainVP, VP;

	CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);

	// `更新cubemap 6個面`
	{
		UINT nViewports = 1;
		g_pDevice->RSGetViewports(&nViewports, &mainVP);

		// new rendertarget viewport size
		VP.TopLeftX = VP.TopLeftY = 0;
		VP.Width = g_Width;
		VP.Height = g_Height;
		VP.MinDepth = 0.0f;
		VP.MaxDepth = 1.0f;

		ID3D10ShaderResourceView *null_views[4] = {NULL, NULL, NULL, NULL};
		g_pDevice->PSSetShaderResources(0, 4, null_views);

		Matrix4x4 mirror_x; mirror_x.Scale_Replace(-1.0f, 1.0f, 1.0f);
		Matrix4x4 cubemap_proj_matrix = mirror_x * GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);;
		CGutModel_DX10::SetProjectionMatrix(cubemap_proj_matrix);

		for ( int i=0; i<6; i++ )
		{
			// `把畫面畫到cubemap其中一個面`
			g_pDevice->OMSetRenderTargets(1, &g_pRGBAView[i], g_pDepthStencilView);
			g_pDevice->RSSetViewports(1, &VP);
			// `清除畫面`
			g_pDevice->ClearRenderTargetView(g_pRGBAView[i], (float *)&vClearColor);
			g_pDevice->ClearDepthStencilView(g_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
			// `更新鏡頭面向`
			Matrix4x4 cubemap_view_matrix = GutMatrixLookAtRH(g_vCubemap_Eye, g_vCubemap_Lookat[i], g_vCubemap_Up[i]);
			CGutModel_DX10::SetViewMatrix(cubemap_view_matrix);
			// `畫行星`
			RenderSolarSystem();
		}

		g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
		g_pDevice->RSSetViewports(nViewports, &mainVP);
	}

	{
		// `清除顏色`
		g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
		g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
		// `設定轉換矩陣`
		CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
		CGutModel_DX10::SetViewMatrix(g_Control.GetViewMatrix());
		CGutModel_DX10::SetInvViewMatrix(g_Control.GetCameraMatrix());
		CGutModel_DX10::SetWorldMatrix(g_sun_matrix);
		CGutModel_DX10::UpdateMatrix();
		// `把Model的第3層貼圖強迫改成cubemap`
		CGutModel_DX10::SetTextureOverwrite(2, g_pCubemapView);
		// `太陽`
		g_Model_DX10[0].Render();
		// `把Model的第3層貼圖還原成模型原始設定`
		CGutModel_DX10::SetTextureOverwrite(2, NULL);
		// `行星`
		RenderSolarSystem();
	}

	// `等待硬體掃結束, 然後才更新畫面.`
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_