#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutTexture_DX10.h"

#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;
static ID3D10InputLayout	*g_pQuadVertexLayout	= NULL;

static ID3D10Buffer			*g_pSunVertexBuffer		= NULL;
static ID3D10Buffer			*g_pEarthVertexBuffer	= NULL;
static ID3D10Buffer			*g_pMoonVertexBuffer	= NULL;
static ID3D10Buffer			*g_pSphereIndexBuffer	= NULL;
static ID3D10Buffer			*g_pQuadVertexBuffer = NULL;
static ID3D10Buffer			*g_pConstantBuffer	= NULL;

static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10VertexShader	*g_pTexVertexShader	= NULL;
static ID3D10PixelShader	*g_pTexPixelShader	= NULL;

static ID3D10RasterizerState *g_pRasterizerState= NULL;

static ID3D10Texture2D *g_pTexture = NULL;
static ID3D10Texture2D *g_pDepthStencilTexture = NULL;
static ID3D10SamplerState	*g_pSamplerState = NULL;

static ID3D10ShaderResourceView *g_pTextureView = NULL;
static ID3D10RenderTargetView	*g_pRGBAView = NULL;
static ID3D10DepthStencilView	*g_pDepthStencilView = NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	// 載入畫星球用的Shader
	{
		ID3D10Blob *pVSCode = NULL;
		const char *shaer = "../../shaders/vertex_color_dx10.hlsl";
		// 載入Vertex Shader
		g_pVertexShader = GutLoadVertexShaderDX10_HLSL(shaer, "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pVertexShader )
			return false;
		// 載入Pixel Shader
		g_pPixelShader = GutLoadPixelShaderDX10_HLSL(shaer, "PS", "ps_4_0");
		if ( NULL==g_pPixelShader )
			return false;

		// 設定Vertex資料格式
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	// 載入畫動態貼圖用的Shader
	{
		ID3D10Blob *pVSCode = NULL;
		const char *shader = "../../shaders/texture_dx10.hlsl";
		// 載入Vertex Shader
		g_pTexVertexShader = GutLoadVertexShaderDX10_HLSL(shader, "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pTexVertexShader )
			return false;
		// 載入Pixel Shader
		g_pTexPixelShader = GutLoadPixelShaderDX10_HLSL(shader, "PS", "ps_4_0");
		if ( NULL==g_pTexPixelShader )
			return false;

		// 設定Vertex資料格式
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pQuadVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	D3D10_BUFFER_DESC cbDesc;
	D3D10_SUBRESOURCE_DATA sbDesc;

	{
		// sun vertex buffer
		cbDesc.ByteWidth = sizeof(Vertex_VC) * g_iNumSphereVertices;
		cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		cbDesc.CPUAccessFlags = 0;
		cbDesc.MiscFlags = 0;
		// 開啟Vertex Buffer時同時把資料拷貝過去
		sbDesc.pSysMem = g_pSunVertices;
		// 配置一塊可以存放Vertex的記憶體, 也就是Vertex Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pSunVertexBuffer ) )
			return false;

		// 開啟Vertex Buffer時同時把資料拷貝過去
		sbDesc.pSysMem = g_pEarthVertices;
		// 配置一塊可以存放Vertex的記憶體, 也就是Vertex Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pEarthVertexBuffer ) )
			return false;

		// 開啟Vertex Buffer時同時把資料拷貝過去
		sbDesc.pSysMem = g_pMoonVertices;
		// 配置一塊可以存放Vertex的記憶體, 也就是Vertex Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pMoonVertexBuffer ) )
			return false;
	}

	{
		// 設定一塊用來放Index的記憶體.
		cbDesc.ByteWidth = sizeof(unsigned short) * g_iNumSphereIndices;
		cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
		cbDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		cbDesc.CPUAccessFlags = 0;
		cbDesc.MiscFlags = 0;
		// 開啟Index Buffer時同時把資料拷貝過去
		sbDesc.pSysMem = g_pSphereIndices;
		// 配置一塊可以存放Index的記憶體, 也就是Index Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pSphereIndexBuffer ) )
			return false;
	}

	{
		// vertex buffer
		cbDesc.ByteWidth = sizeof(g_Quad);
		cbDesc.Usage = D3D10_USAGE_DYNAMIC ;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA subDesc;
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = g_Quad;
		// 配置一塊可以存放Vertex的記憶體, 也就是Vertex Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pQuadVertexBuffer ) )
			return false;
	}

	{
		// 配置Shader常數記憶體
		cbDesc.ByteWidth = sizeof(Matrix4x4);
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
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

		D3D10_SAMPLER_DESC sampler_desc;
		ZeroMemory(&sampler_desc, sizeof(sampler_desc));
		// 座標解讀模式為wrap
		sampler_desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
		// anisotropic filter
		sampler_desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;

		g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState);
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

	{
		// Rasterizer state物件
		D3D10_RASTERIZER_DESC rasterizer_state_desc;

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

	// 計算轉換矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pSunVertexBuffer);
	SAFE_RELEASE(g_pEarthVertexBuffer);
	SAFE_RELEASE(g_pMoonVertexBuffer);
	SAFE_RELEASE(g_pSphereIndexBuffer);
	SAFE_RELEASE(g_pConstantBuffer);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pRasterizerState);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();

	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, aspect, 0.1f, 100.0f);
}

static void RenderSolarSystem(void)
{
	UINT stride = sizeof(Vertex_VC);
	UINT offset = 0;

	// 設定vertex shader
	g_pDevice->VSSetShader(g_pVertexShader);
	// 設定pixel shader
	g_pDevice->PSSetShader(g_pPixelShader);
	// 設定vertex資料格式
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// 設定index buffer
	g_pDevice->IASetIndexBuffer(g_pSphereIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// 設定三角形頂點索引值資料排列是triangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Vector4 eye(0.0f, 0.0f, 15.0f); 
	Vector4 lookat(0.0f, 0.0f, 0.0f); 
	Vector4 up(0.0f, 1.0f, 0.0f); 

	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(60.0f, 1.0f, 0.1f, 100.0f);

	Matrix4x4 view_matrix = GutMatrixLookAtRH(eye, lookat, up);
	Matrix4x4 view_proj_matrix = view_matrix * projection_matrix;
	Matrix4x4 world_view_proj_matrix;

	// sun
	world_view_proj_matrix = g_sun_matrix * view_proj_matrix;
	// 設定shader參數
	Matrix4x4 *pConstData;
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// 
	g_pDevice->IASetVertexBuffers(0, 1, &g_pSunVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);

	// earth
	world_view_proj_matrix = g_earth_matrix * view_proj_matrix;
	// 設定shader參數
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// 
	g_pDevice->IASetVertexBuffers(0, 1, &g_pEarthVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);

	// moon
	world_view_proj_matrix = g_moon_matrix * view_proj_matrix;
	// 設定shader參數
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// 
	g_pDevice->IASetVertexBuffers(0, 1, &g_pMoonVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);
}

void RenderFrameDX10(void)
{
	Vector4 vColorBlue(0.0f, 0.0f, 1.0f, 1.0f);
	Vector4 vColorGray(0.5f, 0.5f, 0.5f, 1.0f);

	// `取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件`
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
	// front/back buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain();
	// `設定vertex shader讀取參數的記憶體位罝`
	g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	ID3D10ShaderResourceView *null_views[4] = {NULL, NULL, NULL, NULL};

	// `更新動態貼圖`
	{
		g_pDevice->PSSetShaderResources(0, 4, null_views);
		g_pDevice->OMSetRenderTargets(1, &g_pRGBAView, g_pDepthStencilView);
		// `清除顏色`
		g_pDevice->ClearRenderTargetView(g_pRGBAView, (float *)&vColorGray);
		// `清除`Depth/Stencil buffer
		g_pDevice->ClearDepthStencilView(g_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

		RenderSolarSystem();

		g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	}
	//  `使用動態貼圖`
	{
		UINT stride = sizeof(Vertex_VT);
		UINT offset = 0;
		// `設定`vertex shader
		g_pDevice->VSSetShader(g_pTexVertexShader);
		// `設定`pixel shader
		g_pDevice->PSSetShader(g_pTexPixelShader);
		// `設定vertex資料格式`
		g_pDevice->IASetInputLayout(g_pQuadVertexLayout);
		// `清除顏色`
		g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vColorBlue);
		// `清除`Depth/Stencil buffer
		g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
		// `設定三角形頂點索引值資料排列是triangle strip`
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		// 
		g_pDevice->IASetVertexBuffers(0, 1, &g_pQuadVertexBuffer, &stride, &offset);
		// `計算矩陣`
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
		Matrix4x4 world_view_proj_matrix = object_matrix * view_matrix * g_proj_matrix;
		// `更新shader參數`
		Matrix4x4 *pConstData;
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		*pConstData = world_view_proj_matrix;
		g_pConstantBuffer->Unmap();
		// `套用第1張貼圖`
		g_pDevice->PSSetShaderResources(0, 1, &g_pTextureView);
		g_pDevice->PSSetSamplers(0, 1, &g_pSamplerState);
		// `畫出看板`
		g_pDevice->Draw(4, 0);
	}

	// 等待硬體掃結束, 然後才更新畫面
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_