#ifdef _ENABLE_DX10_

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_DX10.h"

enum 
{
	BUFFER_POS,
	BUFFER_NORMAL,
	BUFFER_ALBEDO,
	BUFFER_NUM
};

static ID3D10Device	*g_pDevice = NULL;

static ID3D10Effect	*g_pPrepareFX = NULL;
static ID3D10Effect *g_pFinalFX = NULL;
static ID3D10Effect	*g_pEffectFX = NULL;

static ID3D10InputLayout	*g_pPrepareLayout = NULL;
static ID3D10InputLayout	*g_pFinalLayout = NULL;

static ID3D10Buffer			*g_pVertexBuffer = NULL;
static ID3D10Buffer			*g_pLightBuffer = NULL;

static ID3D10Texture2D *g_Textures[BUFFER_NUM];
static ID3D10ShaderResourceView *g_RSView[BUFFER_NUM];
static ID3D10RenderTargetView *g_RTView[BUFFER_NUM];

static ID3D10InputLayout *g_pVertexLayout = NULL;
static ID3D10InputLayout *g_pAmbientLightLayout = NULL;
static ID3D10InputLayout *g_pLightLayout = NULL;

static Matrix4x4 g_proj_matrix;

static CGutModel_DX10 g_Model_DX10;
static CGutModel_DX10 g_Sphere_DX10;

bool ReInitResourceDX10(void)
{
	int w, h;
	GutGetWindowSize(w, h);
	float fAspect = (float)h/(float)w;

	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, fAspect, 0.1f, 100.0f);

	for ( int i=0; i<BUFFER_NUM; i++ )
	{
		GutCreateRenderTarget_DX10(w, h, DXGI_FORMAT_R32G32B32A32_FLOAT, 
			&g_Textures[i], &g_RSView[i], &g_RTView[i]);
	}

	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	g_pPrepareFX = GutLoadFXShaderDX10("../../shaders/DeferredLighting_Prepare_dx10.fx");
	if ( NULL==g_pPrepareFX )
		return false;

	g_pFinalFX = GutLoadFXShaderDX10("../../shaders/DeferredLighting_Final_dx10.fx");
	if ( NULL==g_pFinalFX )
		return false;

	g_pEffectFX = GutLoadFXShaderDX10("../../shaders/Posteffect_dx10.fx");
	if ( NULL==g_pEffectFX )
		return false;

	const sVertexDecl *pVertexDecl = g_Model.GetVertexFormat();

	D3D10_PASS_DESC PassDesc;

	ID3D10EffectTechnique *pShader = g_pPrepareFX->GetTechniqueByName("Prepare");
	pShader->GetPassByIndex(0)->GetDesc(&PassDesc);
	g_pPrepareLayout = GutCreateInputLayoutDX10(pVertexDecl, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize);

	pShader = g_pFinalFX->GetTechniqueByName("PointLight");
	pShader->GetPassByIndex(0)->GetDesc(&PassDesc);
	pVertexDecl = g_Sphere.GetVertexFormat();
	g_pFinalLayout = GutCreateInputLayoutDX10(pVertexDecl, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize);

	// 建立一片矩形, 畫 ambient light 會用到.
	{
		g_pVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(Vertex_VT)*4, g_FullScreenQuad);

		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		ID3D10EffectTechnique *pShader = g_pEffectFX->GetTechniqueByName("DrawIcon");
		pShader->GetPassByIndex(0)->GetDesc(&PassDesc);

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout ) )
			return false;

		pShader = g_pPrepareFX->GetTechniqueByName("AmbientLight");
		pShader->GetPassByIndex(0)->GetDesc(&PassDesc);
		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pAmbientLightLayout ) )
			return false;
	}

	// 建立畫光源位置用的 Vertex Buffer.
	{
		g_pLightBuffer = GutCreateVertexBuffer_DX10(sizeof(Vertex_VC)*NUM_LIGHTS);

		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		ID3D10EffectTechnique *pShader = g_pFinalFX->GetTechniqueByName("DrawLight");
		pShader->GetPassByIndex(0)->GetDesc(&PassDesc);

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pLightLayout ) )
			return false;
	}

	CGutModel::SetTexturePath("../../textures/");

	g_Model_DX10.ConvertToDX10Model(&g_Model);
	g_Sphere_DX10.ConvertToDX10Model(&g_Sphere);

	ReInitResourceDX10();

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pPrepareLayout);
	SAFE_RELEASE(g_pFinalLayout);
	SAFE_RELEASE(g_pLightLayout);
	SAFE_RELEASE(g_pAmbientLightLayout);

	SAFE_RELEASE(g_pPrepareFX);
	SAFE_RELEASE(g_pFinalFX);

	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pLightBuffer);

	for ( int i=0; i<BUFFER_NUM; i++ )
	{
		SAFE_RELEASE(g_Textures[i]);
		SAFE_RELEASE(g_RSView[i]);
		SAFE_RELEASE(g_RTView[i]);
	}

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	for ( int i=0; i<BUFFER_NUM; i++ )
	{
		SAFE_RELEASE(g_Textures[i]);
		SAFE_RELEASE(g_RSView[i]);
		SAFE_RELEASE(g_RTView[i]);
	}

	GutResetGraphicsDeviceDX10();

	ReInitResourceDX10();
}

void DrawImage(ID3D10ShaderResourceView *pTexture, float x, float y, float w, float h)
{
	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	Vector4 vtable[4];

	vtable[0].Set(x, y, 0.0f, 1.0f);
	vtable[1].Set(x+w, y, 0.0f, 1.0f);
	vtable[2].Set(x, y+h, 0.0f, 1.0f);
	vtable[3].Set(x+w, y+h, 0.0f, 1.0f);

	Vector4 ttable[4] =
	{
		Vector4(0.0f, 1.0f, 0.0f, 1.0f),
		Vector4(1.0f, 1.0f, 0.0f, 1.0f),
		Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		Vector4(1.0f, 0.0f, 0.0f, 1.0f)
	};

	ID3D10EffectTechnique *pShader = g_pEffectFX->GetTechniqueByName("DrawIcon");

	ID3D10EffectShaderResourceVariable *pInputTexture = g_pEffectFX->GetVariableByName("Image")->AsShaderResource();
	ID3D10EffectVectorVariable *vertex_table = g_pEffectFX->GetVariableByName("vertex_table")->AsVector();
	ID3D10EffectVectorVariable *texcoord_table = g_pEffectFX->GetVariableByName("texcoord_table")->AsVector();

	pInputTexture->SetResource(pTexture);
	vertex_table->SetFloatVectorArray( (float*)vtable, 0, 4);
	texcoord_table->SetFloatVectorArray( (float*)ttable, 0, 4);

	pShader->GetPassByIndex(0)->Apply(0);

	g_pDevice->IASetInputLayout(g_pVertexLayout);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pDevice->Draw(4, 0);
}

static void DeferredLighting_Prepare(void)
{
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wvp_matrix = world_matrix * view_matrix * g_proj_matrix;

	// main framebuffer/depthbuffer
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView();

	ID3D10ShaderResourceView *pViews[3] = {NULL, NULL, NULL};
	g_pDevice->PSSetShaderResources(0, 3, pViews);

	g_pDevice->OMSetRenderTargets(3, g_RTView, pDepthStencilView);
	g_pDevice->ClearRenderTargetView(g_RTView[0], &vClearColor[0]);
	g_pDevice->ClearRenderTargetView(g_RTView[1], &vClearColor[0]);
	g_pDevice->ClearRenderTargetView(g_RTView[2], &vClearColor[0]);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);

	ID3D10EffectTechnique *pShader = g_pPrepareFX->GetTechniqueByName("Prepare");
	ID3D10EffectMatrixVariable *wvp_matrix_var = g_pPrepareFX->GetVariableByName("wvp_matrix")->AsMatrix();
	ID3D10EffectMatrixVariable *world_matrix_var = g_pPrepareFX->GetVariableByName("world_matrix")->AsMatrix();

	wvp_matrix_var->SetMatrix( (float *)&wvp_matrix );
	world_matrix_var->SetMatrix( (float *)&world_matrix );

	pShader->GetPassByIndex(0)->Apply(0);
	g_pDevice->IASetInputLayout(g_pPrepareLayout);

	g_Model_DX10.Render(SUBMIT_TEXTURES);

	ID3D10RenderTargetView *dummy_views[3] = {pRenderTargetView, NULL, NULL};
	g_pDevice->OMSetRenderTargets(3, dummy_views, pDepthStencilView);
	g_pDevice->ClearRenderTargetView(pRenderTargetView, &vClearColor[0]);
}

static void DeferredLighting_AmbientLight(void)
{
	ID3D10EffectTechnique *pShader = g_pFinalFX->GetTechniqueByName("AmbientLight");
	ID3D10EffectMatrixVariable *wvp_matrix_var = g_pFinalFX->GetVariableByName("wvp_matrix")->AsMatrix();
	ID3D10EffectVectorVariable *lightcolor_var = g_pFinalFX->GetVariableByName("vLightColor")->AsVector();
	ID3D10EffectShaderResourceVariable *worldpos_rsview = g_pFinalFX->GetVariableByName("worldposTexture")->AsShaderResource();
	ID3D10EffectShaderResourceVariable *worldnormal_rsview = g_pFinalFX->GetVariableByName("worldnormalTexture")->AsShaderResource();
	ID3D10EffectShaderResourceVariable *albedo_rsview = g_pFinalFX->GetVariableByName("diffuseTexture")->AsShaderResource();

	worldpos_rsview->SetResource(g_RSView[BUFFER_POS]);
	worldnormal_rsview->SetResource(g_RSView[BUFFER_NORMAL]);
	albedo_rsview->SetResource(g_RSView[BUFFER_ALBEDO]);

	lightcolor_var->SetFloatVector( (float *)&g_Lights[0].m_vAmbient);

	Matrix4x4 ident_matrix; ident_matrix.Identity();
	wvp_matrix_var->SetMatrix( (float*)&ident_matrix );

	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	pShader->GetPassByIndex(0)->Apply(0);

	g_pDevice->IASetInputLayout(g_pAmbientLightLayout);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pDevice->Draw(4, 0);
}

static void DeferredLighting_PointLights(void)
{
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 camera_matrix = g_Control.GetCameraMatrix();
	Matrix4x4 vp_matrix = view_matrix * g_proj_matrix;

	ID3D10EffectTechnique *pShader = g_pFinalFX->GetTechniqueByName("PointLight");
	ID3D10EffectMatrixVariable *wvp_matrix_var = g_pFinalFX->GetVariableByName("wvp_matrix")->AsMatrix();
	ID3D10EffectShaderResourceVariable *worldpos_rsview = g_pFinalFX->GetVariableByName("worldposTexture")->AsShaderResource();
	ID3D10EffectShaderResourceVariable *worldnormal_rsview = g_pFinalFX->GetVariableByName("worldnormalTexture")->AsShaderResource();
	ID3D10EffectShaderResourceVariable *albedo_rsview = g_pFinalFX->GetVariableByName("diffuseTexture")->AsShaderResource();

	ID3D10EffectVectorVariable *camerapos_var = g_pFinalFX->GetVariableByName("vCameraPos")->AsVector();
	ID3D10EffectVectorVariable *lightpos_var = g_pFinalFX->GetVariableByName("vLightPos")->AsVector();
	ID3D10EffectVectorVariable *lightambient_var = g_pFinalFX->GetVariableByName("vLightAmbient")->AsVector();
	ID3D10EffectVectorVariable *lightcolor_var = g_pFinalFX->GetVariableByName("vLightColor")->AsVector();

	worldpos_rsview->SetResource(g_RSView[BUFFER_POS]);
	worldnormal_rsview->SetResource(g_RSView[BUFFER_NORMAL]);
	albedo_rsview->SetResource(g_RSView[BUFFER_ALBEDO]);

	camerapos_var->SetFloatVector( &camera_matrix[3][0] );

	g_pDevice->IASetInputLayout(g_pFinalLayout);

	// deferred lighting
	for ( int i=1; i<NUM_LIGHTS; i++ )
	{
		sGutLight *pLight = &g_Lights[i];

		Matrix4x4 world_matrix;

		world_matrix.Scale_Replace(pLight->m_vPosition.GetWWWW());

		world_matrix[3] = pLight->m_vPosition;
		world_matrix[3][3] = 1.0f;

		Matrix4x4 wvp_matrix = world_matrix * vp_matrix;

		wvp_matrix_var->SetMatrix( (float *)&wvp_matrix );

		lightcolor_var->SetFloatVector( &pLight->m_vDiffuse[0] );
		lightambient_var->SetFloatVector( &pLight->m_vAmbient[0] );
		lightpos_var->SetFloatVector( &pLight->m_vPosition[0] );

		pShader->GetPassByIndex(0)->Apply(0);

		g_Sphere_DX10.Render(0);
	}

	// draw lights
	{
		ID3D10EffectTechnique *pShader = g_pFinalFX->GetTechniqueByName("DrawLight");
		wvp_matrix_var->SetMatrix( (float *)&vp_matrix );

		UINT stride = sizeof(Vertex_VC);
		UINT offset = 0;

		void *pBuffer = NULL;
		g_pLightBuffer->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pBuffer );
		memcpy(pBuffer, g_LightsVC, sizeof(g_LightsVC));
		g_pLightBuffer->Unmap();

		pShader->GetPassByIndex(0)->Apply(0);

		g_pDevice->IASetInputLayout(g_pLightLayout);
		g_pDevice->IASetVertexBuffers(0, 1, &g_pLightBuffer, &stride, &offset);
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		g_pDevice->Draw(NUM_LIGHTS, 0);
	}
}

void RenderFrameDX10(void)
{
	DeferredLighting_Prepare();
	DeferredLighting_AmbientLight();
	DeferredLighting_PointLights();

	float x = -1.0f;
	float y = -1.0f;
	float w = 0.4f;
	float h = 0.4f;

	if ( g_iMode & 0x01 )
	{
		DrawImage(g_RSView[BUFFER_POS], x, y, w, h);
		x+=w;
	}

	if ( g_iMode & 0x02 )
	{
		DrawImage(g_RSView[BUFFER_NORMAL], x, y, w, h);
		x+=w;
	}

	if ( g_iMode & 0x04 )
	{
		DrawImage(g_RSView[BUFFER_ALBEDO], x, y, w, h);
		x+=w;
	}

	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_