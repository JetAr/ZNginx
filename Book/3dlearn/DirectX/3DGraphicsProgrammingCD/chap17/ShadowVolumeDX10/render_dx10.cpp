
#include "render_data.h"

#include "Gut.h"
#include "GutModel_DX10.h"

static ID3D10Device	*g_pDevice = NULL;
static ID3D10Effect *g_pEffect = NULL;
static ID3D10Effect *g_pColorFX = NULL;

static ID3D10Buffer *g_pShadowVolume = NULL;
static ID3D10Buffer *g_pFullScreenQuad = NULL;

static ID3D10InputLayout *g_pLayout = NULL;
static ID3D10InputLayout *g_pQuadLayout = NULL;

static Matrix4x4 g_proj_matrix;
static CGutModel_DX10 g_Box_DX10, g_Wall_DX10;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl");
	CGutModel::SetTexturePath("../../textures/");

	g_Box_DX10.ConvertToDX10Model(&g_Box);
	g_Wall_DX10.ConvertToDX10Model(&g_Wall);

	g_pEffect  = GutLoadFXShaderDX10("shadowvolume_dx10.fx");
	g_pColorFX = GutLoadFXShaderDX10("../../shaders/color_dx10.fx");

	g_ShadowVolume.Preprocess_Adj();

	g_pShadowVolume = GutCreateVertexBuffer_DX10(sizeof(Vector4) * g_ShadowVolume.m_iNumFaces * 6, g_ShadowVolume.m_pShadowVolume);
	g_pFullScreenQuad = GutCreateVertexBuffer_DX10(sizeof(Vertex_V3T2) * 4, g_FullScreenQuad);

	{
		D3D10_INPUT_ELEMENT_DESC decl[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0}
		};

		D3D10_PASS_DESC PassDesc;

		ID3D10EffectTechnique *pShader = g_pEffect->GetTechniqueByName( "ShadowVolume" );
		pShader->GetPassByIndex( 0 )->GetDesc( &PassDesc );

		g_pDevice->CreateInputLayout(decl, 1, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pLayout);
	}

	{
		D3D10_INPUT_ELEMENT_DESC decl[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
		};

		D3D10_PASS_DESC PassDesc;

		ID3D10EffectTechnique *pShader = g_pColorFX->GetTechniqueByName( "OutputColor" );
		pShader->GetPassByIndex( 0 )->GetDesc( &PassDesc );

		g_pDevice->CreateInputLayout(decl, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pQuadLayout);
	}

	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
	g_Box_DX10.Release();
	g_Wall_DX10.Release();

	SAFE_RELEASE(g_pShadowVolume);
	SAFE_RELEASE(g_pFullScreenQuad);

	SAFE_RELEASE(g_pLayout);
	SAFE_RELEASE(g_pQuadLayout);
	SAFE_RELEASE(g_pFullScreenQuad);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();

	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	float fDummy[] = {0, 0, 0, 0};

	// 取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer

	// 清除顏色
	// 清除Depth/Stencil buffer
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

	// 取得轉換矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 ident_matrix = Matrix4x4::IdentityMatrix();

	Matrix4x4 wvp_matrix = world_matrix * view_matrix * g_proj_matrix;
	Matrix4x4 vp_matrix = view_matrix * g_proj_matrix;


	// `畫出茶具組`
	{
		CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
		CGutModel_DX10::SetViewMatrix(view_matrix);
		CGutModel_DX10::SetWorldMatrix(world_matrix);
		CGutModel_DX10::UpdateMatrix();

		g_Box_DX10.Render();
	}
	// `畫出墻壁`
	{
		Matrix4x4 wall_matrix; 
		wall_matrix.Translate_Replace(0.0f, 0.0f, -2.0f);
		wall_matrix.Scale(Vector4(2.0f, 2.0f, 2.0f));

		CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
		CGutModel_DX10::SetViewMatrix(view_matrix);
		CGutModel_DX10::SetWorldMatrix(wall_matrix);
		CGutModel_DX10::UpdateMatrix();

		g_Wall_DX10.Render();
	}
	// shadowvolume stencil buffer pass
	{
		UINT offset = 0;
		UINT stride = sizeof(Vector4);

		// light position & orientation
		Vector4 light_pos(5.0f, 0.0f, 5.0f);
		Vector4 light_lookat(0.0f, 0.0f, 0.0f);
		Vector4 light_up(0.0f, 1.0f, 0.0f);

		// light matrix
		//Matrix4x4 light_proj = GutMatrixPerspectiveRH_OpenGL(120.0f, 1.0f, 1.0f, 100.0f);
		Matrix4x4 light_proj = GutMatrixOrthoRH_OpenGL(10.0f, 10.0f, 1.0f, 100.0f);
		Matrix4x4 light_view = GutMatrixLookAtRH(light_pos, light_lookat, light_up);
		Matrix4x4 light_wvp = world_matrix * light_view * light_proj;

		g_pDevice->IASetInputLayout(g_pLayout);
		g_pDevice->IASetVertexBuffers(0, 1, &g_pShadowVolume, &stride, &offset);
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ);

		ID3D10EffectTechnique *pShader = g_pEffect->GetTechniqueByName( "ShadowVolume" );

		ID3D10EffectMatrixVariable *pWorldViewProjMat = g_pEffect->GetVariableByName( "wvp_matrix" )->AsMatrix();
		ID3D10EffectMatrixVariable *pViewProjMat = g_pEffect->GetVariableByName( "vp_matrix" )->AsMatrix();
		ID3D10EffectMatrixVariable *pWorldMat = g_pEffect->GetVariableByName( "world_matrix" )->AsMatrix();
		ID3D10EffectMatrixVariable *pLightMat = g_pEffect->GetVariableByName( "light_matrix" )->AsMatrix();
		ID3D10EffectVectorVariable *pLightPos = g_pEffect->GetVariableByName( "light_pos" )->AsVector();
		ID3D10EffectScalarVariable *pLightRange = g_pEffect->GetVariableByName( "light_range" )->AsScalar();

		pWorldViewProjMat->SetMatrix( (float*)&wvp_matrix );
		pViewProjMat->SetMatrix( (float*)&vp_matrix );
		pWorldMat->SetMatrix( (float*)&world_matrix );
		pLightMat->SetMatrix( (float*)&light_wvp );
		pLightPos->SetFloatVector( &light_pos[0] );
		pLightRange->SetFloat( 10.0f );

		pShader->GetPassByIndex(0)->Apply(0);

		g_pDevice->Draw( g_ShadowVolume.m_iNumFaces*6, 0 );

		g_pDevice->RSSetState(NULL);
		g_pDevice->OMSetBlendState(NULL, fDummy, 0xffffffff);
		g_pDevice->OMSetDepthStencilState(NULL, 0);
	}
	// cast shadow
	{
		UINT offset = 0;
		UINT stride = sizeof(Vertex_V3T2);
		Vector4 vHalf(0.3f, 0.3f, 0.3f, 1.0f);
		Matrix4x4 ident_mat; ident_mat.Identity();

		g_pDevice->IASetInputLayout(g_pQuadLayout);
		g_pDevice->IASetVertexBuffers(0, 1, &g_pFullScreenQuad, &stride, &offset);
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		ID3D10EffectTechnique *pShader = g_pColorFX->GetTechniqueByName( "Shadow" );
		ID3D10EffectVectorVariable *pColorPtr = g_pColorFX->GetVariableByName( "color" )->AsVector();
		ID3D10EffectMatrixVariable *wvp_matrix = g_pColorFX->GetVariableByName( "wvp_matrix" )->AsMatrix();

		pColorPtr->SetFloatVector(&vHalf[0]);
		wvp_matrix->SetMatrix( (float*)&ident_mat );

		pShader->GetPassByIndex(0)->Apply(0);

		g_pDevice->Draw( 4, 0 );

		g_pDevice->RSSetState(NULL);
		g_pDevice->OMSetDepthStencilState(NULL, 0);
	}

	// 等待硬體掃結束, 然後才更新畫面
	pSwapChain->Present(1, 0);
}

