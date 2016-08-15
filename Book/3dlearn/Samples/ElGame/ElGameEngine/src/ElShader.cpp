#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElShader.h"
#include "ElDeviceManager.h"

ElShader::ElShader() :
mEffect(NULL)
{

}

ElShader::~ElShader()
{
	ElSafeRelease(mEffect);
}

bool ElShader::create(LPCSTR shaderFileName)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	ID3DXBuffer* error = 0;
	HRESULT hr = D3DXCreateEffectFromFile(d3dDevice,
		shaderFileName,
		0,
		0,
		D3DXSHADER_DEBUG,
		0,
		&mEffect,
		&error);

	if (error)
	{
		MessageBox(0, (LPCTSTR)error->GetBufferPointer(), 0, 0);
		ElSafeRelease(error);
		return false;
	}

	if (FAILED(hr))
	{
		MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", 0, 0);
		ElSafeRelease(error);
		return false;
	}

	ElSafeRelease(error);

	if (!initialize())
		return false;

	return true;
}

void ElShader::render(ElCamera* camera)
{
	if (camera->isReflected())
		setShaderClipPlane(camera, true);

	renderImp(camera);

	if (camera->isReflected())
		setShaderClipPlane(camera, false);
}

bool ElShader::initialize()
{
	return true;
}

void ElShader::renderImp(ElCamera* camera)
{
	UINT passes = 0;
	mEffect->Begin(&passes, 0);
	for (UINT pass = 0; pass < passes; ++pass)
	{
		mEffect->BeginPass(pass);

		// render scene here

		mEffect->EndPass();
	}
	mEffect->End();
}

void ElShader::setShaderClipPlane(ElCamera* camera, bool shaderMode)
{
	IDirect3DDevice9* d3dDevice = NULL;
	mEffect->GetDevice(&d3dDevice);
	
	const D3DXPLANE& reflectPlane = camera->getReflectionPlane();
	if (shaderMode)
	{
		D3DXMATRIX tempMatrix = camera->getViewMatrix();

		//transform the plane into view space
		D3DXPLANE viewSpacePlane;
		D3DXMatrixInverse(&tempMatrix, NULL, &tempMatrix);
		D3DXMatrixTranspose(&tempMatrix, &tempMatrix);
		D3DXPlaneTransform(&viewSpacePlane, &reflectPlane, &tempMatrix);

		//transform the plane into clip space, or projection space
		D3DXPLANE clipSpacePlane;
		d3dDevice->GetTransform(D3DTS_PROJECTION, &tempMatrix);
		D3DXMatrixInverse(&tempMatrix, NULL, &tempMatrix);
		D3DXMatrixTranspose(&tempMatrix, &tempMatrix);
		D3DXPlaneTransform(&clipSpacePlane, &viewSpacePlane, &tempMatrix);

		d3dDevice->SetClipPlane(0, clipSpacePlane);
	}
	else
		d3dDevice->SetClipPlane(0, reflectPlane);
}

