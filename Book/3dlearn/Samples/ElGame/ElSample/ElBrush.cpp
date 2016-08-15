#include "ElSamplePCH.h"
#include "ElDefines.h"
#include "ElBrush.h"
#include "ElTerrain.h"
#include "ElDeviceManager.h"

ElBrush::ElBrush() :
mNumVertex(0),
mInnerRadius(0.0f),
mOuterRadius(0.0f),
mTerrain(NULL)
{

}

ElBrush::~ElBrush()
{

}

bool ElBrush::create(ElTerrain* terrain, unsigned int numVertex, float innerRadius, float outerRadius)
{
	if (!terrain)
		return false;

	mNumVertex = numVertex;
	mInnerRadius = innerRadius;
	mOuterRadius = outerRadius;
	mTerrain = terrain;

	mVertices.assign(mNumVertex);

	return true;
}

void ElBrush::render(D3DXVECTOR3 center)
{
	float factor = (2.0f * D3DX_PI) / ((float)mNumVertex - 1.0f);
	
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (SUCCEEDED(d3dDevice->BeginScene()))
	{
		D3DXMATRIX m;
		D3DXMatrixTranslation(&m, 0.0f, 0.0f, 0.0f);
		d3dDevice->SetTransform(D3DTS_WORLD, &m);
		
		d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		d3dDevice->SetFVF(VERTEX::FVF);

		// Inner Draw
		for (unsigned int i = 0; i < mNumVertex; ++i)
		{
			float fAngle = i * factor;

			float x = center.x + sin(fAngle) * mInnerRadius;
			float z = center.z + cos(fAngle) * mInnerRadius;
			float y = mTerrain->getHeightAtPoint(x, z);

			mVertices[i].x = x;
			mVertices[i].y = y;
			mVertices[i].z = z;

			mVertices[i].color = D3DCOLOR_XRGB(255, 255, 0);
		}

		d3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, mNumVertex - 1, mVertices.base(), sizeof(VERTEX));

		// Outer Draw
		for (unsigned int i = 0; i < mNumVertex; ++i)
		{
			float fAngle = i * factor;

			float x = center.x + sin(fAngle) * mOuterRadius;
			float z = center.z + cos(fAngle) * mOuterRadius;
			float y = mTerrain->getHeightAtPoint(x, z);

			mVertices[i].x = x;
			mVertices[i].y = y;
			mVertices[i].z = z;

			mVertices[i].color = D3DCOLOR_XRGB(255, 0, 0);
		}
		
		d3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, mNumVertex - 1, mVertices.base(), sizeof(VERTEX));

		d3dDevice->EndScene();
		d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	}
}








