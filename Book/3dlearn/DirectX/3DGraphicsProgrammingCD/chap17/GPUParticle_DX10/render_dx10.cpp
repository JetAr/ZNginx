

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_DX10.h"
#include "GutTexture_DX10.h"

static ID3D10Device	*g_pDevice = NULL;
static ID3D10Effect	*g_pSimFX = NULL;
static ID3D10Effect *g_pRenderFX = NULL;
static ID3D10Buffer *g_pParticleArray[2] = { NULL, NULL };

static ID3D10InputLayout *g_pLayout = NULL;
static ID3D10ShaderResourceView *g_pNoiseTexture = NULL;
static ID3D10ShaderResourceView *g_pParticleTexture = NULL;

const int g_max_particles = 65536;
int g_num_particles = 0;

static Matrix4x4 g_proj_matrix;

bool ReInitResourceDX10(void)
{
	int w,h; GutGetWindowSize(w, h);

	float fAspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(72, fAspect, 0.1f, 100.0f);

	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	ReInitResourceDX10();

	g_pSimFX = GutLoadFXShaderDX10("GPUParticle_dx10.fx");
	if ( NULL==g_pSimFX )
		return false;

	g_pRenderFX = GutLoadFXShaderDX10("GPUParticleRender_dx10.fx");
	if ( NULL==g_pRenderFX )
		return false;

	g_pNoiseTexture = GutLoadTexture_DX10("../../textures/noise_512x512.tga");
	if ( NULL==g_pNoiseTexture )
		return false;

	g_pParticleTexture = GutLoadTexture_DX10("../../textures/particle.tga");
	if ( NULL==g_pParticleTexture )
		return false;

	const D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,16, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	ID3D10EffectTechnique *pShader = g_pSimFX->GetTechniqueByName("Simulate");
	D3D10_PASS_DESC PassDesc;
	pShader->GetPassByIndex( 0 )->GetDesc( &PassDesc );

	g_pDevice->CreateInputLayout(layout, 2, 
		PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pLayout);

	unsigned char *pInit = new unsigned char[g_max_particles*32];
	memset(pInit, 0, g_max_particles*32);

	for ( int i=0; i<2; i++ )
	{
		g_pParticleArray[i] = GutCreateBufferObject_DX10(
			D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT, 
			D3D10_USAGE_DEFAULT, g_max_particles * 32, pInit);
	}

	delete [] pInit;

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pSimFX);
	SAFE_RELEASE(g_pRenderFX);
	SAFE_RELEASE(g_pLayout);

	SAFE_RELEASE(g_pParticleArray[0]);
	SAFE_RELEASE(g_pParticleArray[1]);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	ReInitResourceDX10();
}

void Simulation(void)
{
	const float fEmitRate = 65536/10;

	g_num_particles = fEmitRate * g_fTimeElapsed;

	if ( g_num_particles > g_max_particles )
	{
		g_num_particles = g_max_particles;
	}

	//g_num_particles = g_max_particles;

	UINT offset[1] = { 0 };
	UINT stride[1] = { 32 };
	ID3D10Buffer *pDummy[1] = {NULL};

	ID3D10EffectTechnique *pShader = NULL;
	pShader = g_pSimFX->GetTechniqueByName("Simulate");

	ID3D10EffectShaderResourceVariable *noisetex_var = g_pSimFX->GetVariableByName("NoiseTex")->AsShaderResource();
	ID3D10EffectVectorVariable *force_var = g_pSimFX->GetVariableByName("Force")->AsVector();
	ID3D10EffectVectorVariable *liferange_var = g_pSimFX->GetVariableByName("LifeRange")->AsVector();
	ID3D10EffectVectorVariable *rand_var = g_pSimFX->GetVariableByName("Rand")->AsVector();
	ID3D10EffectVectorVariable *speedrange_var = g_pSimFX->GetVariableByName("SpeedRange")->AsVector();
	ID3D10EffectVectorVariable *sizerange_var = g_pSimFX->GetVariableByName("SizeRange")->AsVector();
	ID3D10EffectScalarVariable *time_var = g_pSimFX->GetVariableByName("fTimeAdvance")->AsScalar();
	ID3D10EffectScalarVariable *tan_var = g_pSimFX->GetVariableByName("fTan")->AsScalar();

	Vector4 vForce = Vector4(0.0f,-1.0f,0.0f,0.0f) * g_fTimeAdvance;
	Vector4 vLifeRange(1.0f, 2.0f, 0.0f, 0.0f);
	Vector4 vSpeedRange(1.0f, 2.0f, 0.0f, 0.0f);
	Vector4 vSizeRange(0.01f, 0.02f, 0.0f, 0.0f);

	Vector4 vRand[3];
	for ( int i=0; i<3; i++ )
	{
		vRand[i][0] = float(rand()%1024)/1024.0f;
		vRand[i][1] = float(rand()%1024)/1024.0f;
		vRand[i][2] = float(rand()%1024)/1024.0f;
		vRand[i][3] = float(rand()%1024)/1024.0f;
	}

	noisetex_var->SetResource(g_pNoiseTexture);
	force_var->SetFloatVector(&vForce[0]);
	liferange_var->SetFloatVector(&vLifeRange[0]);
	speedrange_var->SetFloatVector(&vSpeedRange[0]);
	sizerange_var->SetFloatVector(&vSizeRange[0]);
	rand_var->SetFloatVectorArray(&vRand[0][0], 0, 3);
	time_var->SetFloat(g_fTimeAdvance);
	tan_var->SetFloat(FastMath::Tan(FastMath::DegToRad(15.0f)));

	g_pDevice->SOSetTargets(1, &g_pParticleArray[1], offset);
	g_pDevice->IASetInputLayout(g_pLayout);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pParticleArray[0], stride, offset);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	pShader->GetPassByIndex(0)->Apply(0);
	g_pDevice->Draw(g_num_particles, 0);

	g_pDevice->SOSetTargets(1, pDummy, offset);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// 取得主畫面
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 

	Simulation();

	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

	UINT offset[1] = { 0 };
	UINT stride[1] = { 32 };

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wvp_matrix = world_matrix * view_matrix * g_proj_matrix;
	Matrix4x4 wv_matrix = world_matrix * view_matrix;
	Matrix4x4 inv_wv_matrix = wv_matrix; inv_wv_matrix.FastInvert();

	ID3D10EffectTechnique *pShader = g_pRenderFX->GetTechniqueByName("RenderParticle");

	ID3D10EffectMatrixVariable *wvp_matrix_var = g_pRenderFX->GetVariableByName("wvp_matrix")->AsMatrix();
	ID3D10EffectMatrixVariable *camera_matrix_var = g_pRenderFX->GetVariableByName("camera_matrix")->AsMatrix();
	ID3D10EffectShaderResourceVariable *tex_var = g_pRenderFX->GetVariableByName("DiffuseTex")->AsShaderResource();

	wvp_matrix_var->SetMatrix( (float*)&wvp_matrix );
	camera_matrix_var->SetMatrix( (float*)&inv_wv_matrix);
	tex_var->SetResource(g_pParticleTexture);

	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pParticleArray[1], stride, offset);
	g_pDevice->IASetInputLayout(g_pLayout);

	pShader->GetPassByIndex(0)->Apply(0);
	g_pDevice->Draw(g_num_particles, 0);

	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 
	pSwapChain->Present(1, 0);

	ID3D10Buffer *pTemp = g_pParticleArray[0];
	g_pParticleArray[0] = g_pParticleArray[1];
	g_pParticleArray[1] = pTemp;
}

