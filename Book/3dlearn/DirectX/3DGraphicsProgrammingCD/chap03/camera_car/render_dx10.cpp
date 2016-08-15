#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pPyramidVertexBuffer	= NULL;
static ID3D10Buffer			*g_pPyramidIndexBuffer	= NULL;

static ID3D10Buffer			*g_pRoadVertexBuffer	= NULL;
static ID3D10Buffer			*g_pRoadIndexBuffer		= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10RasterizerState *g_pRasterizerState= NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Vertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/vertex_color_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// 載入Pixel Shader
	g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/vertex_color_dx10.hlsl", "PS", "ps_4_0");
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

	// 設定一塊可以用來放Vertex的記憶體.
	D3D10_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(g_pyramid_vertices);
	cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
	cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	// 開啟Vertex Buffer時同時把資料拷貝過去
	D3D10_SUBRESOURCE_DATA sbDesc;
	sbDesc.pSysMem = g_pyramid_vertices;
	// 配置一塊可以存放Vertex的記憶體, 也就是Vertex Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pPyramidVertexBuffer ) )
		return false;

	cbDesc.ByteWidth = sizeof(g_road_vertices);
	sbDesc.pSysMem = g_road_vertices;
	// 配置一塊可以存放Vertex的記憶體, 也就是Vertex Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pRoadVertexBuffer ) )
		return false;

	// 設定一塊可以用來放Index的記憶體.
	cbDesc.ByteWidth = sizeof(g_pyramid_trianglelist_indices);
	cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
	cbDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	// 開啟Index Buffer時同時把資料拷貝過去
	sbDesc.pSysMem = g_pyramid_trianglelist_indices;
	// 配置一塊可以存放Index的記憶體, 也就是Index Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pPyramidIndexBuffer ) )
		return false;

	cbDesc.ByteWidth = sizeof(g_road_trianglestrip_indices);
	sbDesc.pSysMem = g_road_trianglestrip_indices;
	// 配置一塊可以存放Index的記憶體, 也就是Index Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pRoadIndexBuffer ) )
		return false;

	// 配置Shader讀取參數的記憶體空間
	cbDesc.ByteWidth = sizeof(Matrix4x4);
	cbDesc.Usage = D3D10_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
		return false;

	// 計算出一個可以轉換到鏡頭座標系的矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);

	// 開?rasterizer state物件
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

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pPyramidVertexBuffer);
	SAFE_RELEASE(g_pPyramidIndexBuffer);
	SAFE_RELEASE(g_pRoadVertexBuffer);
	SAFE_RELEASE(g_pRoadIndexBuffer);
	SAFE_RELEASE(g_pConstantBuffer);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pRasterizerState);

	return true;
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UINT stride = sizeof(Vertex_VC);
	UINT offset = 0;

	// `取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件`
	//frame buffer
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); 
	//depth/stencil buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
	// front/back buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 

	// `清除顏色`
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// `清除Depth/Stencil buffer`
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	// `設定vertex shader`
	g_pDevice->VSSetShader(g_pVertexShader);
	// `設定pixel shader`
	g_pDevice->PSSetShader(g_pPixelShader);
	// `設定vertex shader讀取參數的記憶體位罝`
	g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// `設定vertex資料格式`
	g_pDevice->IASetInputLayout(g_pVertexLayout);

	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 view_proj_matrix = view_matrix * g_proj_matrix;

	Vector4 border(-15.0f, 0.0f, -15.0f);
	Vector4 grid_position = border;

	const int grids_x = 30;
	const int grids_z = 30;

	for ( int x=0; x<grids_x; x++ )
	{
		grid_position[2] = border[2];
		int grid_x = x & 0x07;

		for ( int z=0; z<grids_z; z++ )
		{
			// `設定轉換矩陣`
			Matrix4x4 object_matrix;
			object_matrix.Translate_Replace(grid_position);

			int grid_z = z & 0x07;

			char c = g_map[grid_x][grid_z];
			if ( c==0 )
			{
				Matrix4x4 world_view_proj_matrix = object_matrix * view_proj_matrix;
				// `設定shader參數`
				Matrix4x4 *pConstData;
				g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
				*pConstData = world_view_proj_matrix;
				g_pConstantBuffer->Unmap();
				// `設定vertex buffer`
				g_pDevice->IASetVertexBuffers(0, 1, &g_pRoadVertexBuffer, &stride, &offset);
				// `設定index buffer`
				g_pDevice->IASetIndexBuffer(g_pRoadIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
				// `設定三角形頂點索引值資料排列是triangle strip`
				g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				// `畫出道路`
				g_pDevice->DrawIndexed(4, 0, 0);
			}
			else
			{
				object_matrix.Scale_Replace(1.0f, (float) c, 1.0f);
				object_matrix[3] = grid_position;

				Matrix4x4 world_view_proj_matrix = object_matrix * view_proj_matrix;
				// `設定shader參數`
				Matrix4x4 *pConstData;
				g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
				*pConstData = world_view_proj_matrix;
				g_pConstantBuffer->Unmap();
				// `設定vertex buffer`
				g_pDevice->IASetVertexBuffers(0, 1, &g_pPyramidVertexBuffer, &stride, &offset);
				// `設定index buffer`
				g_pDevice->IASetIndexBuffer(g_pPyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
				// `設定三角形頂點索引值資料排列是triangle list`
				g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				// `畫出金字塔`
				g_pDevice->DrawIndexed(12, 0, 0);
			}

			grid_position[2] += 1.0f;
		}

		grid_position[0] += 1.0f;
	}

	// `等待硬體掃結束, 然後才更新畫面.`
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_