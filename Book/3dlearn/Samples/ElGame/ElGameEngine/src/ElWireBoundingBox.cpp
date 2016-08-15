#include "ElApplicationPCH.h"
#include "ElWireBoundingBox.h"
#include "ElDeviceManager.h"

ElWireBoundingBox::ElWireBoundingBox()
: mColor(D3DCOLOR_XRGB(255, 255, 255))
{

}

ElWireBoundingBox::~ElWireBoundingBox()
{

}

void ElWireBoundingBox::setBoundingBoxColor(ColorValue color)
{
	mColor = color;
}

void ElWireBoundingBox::setupBoundingBoxVertices(const ElAxisAlignedBox& aab)
{
	if (mVertices.empty())
		mVertices.assign(24);

	D3DXVECTOR3 max = aab.getMaximum();
	D3DXVECTOR3 min = aab.getMinimum();

	float maxx = max.x;
	float maxy = max.y;
	float maxz = max.z;

	float minx = min.x;
	float miny = min.y;
	float minz = min.z;

	int i = 0;

	// line 0
	mVertices[i].x = minx;
	mVertices[i].y = miny;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	mVertices[++i].x = maxx;
	mVertices[i].y = miny;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	// line 1
	mVertices[++i].x = minx;
	mVertices[i].y = miny;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	mVertices[++i].x = minx;
	mVertices[i].y = miny;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	// line 2
	mVertices[++i].x = minx;
	mVertices[i].y = miny;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	mVertices[++i].x = minx;
	mVertices[i].y = maxy;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	// line 3
	mVertices[++i].x = minx;
	mVertices[i].y = maxy;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	mVertices[++i].x = minx;
	mVertices[i].y = maxy;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	// line 4
	mVertices[++i].x = minx;
	mVertices[i].y = maxy;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	mVertices[++i].x = maxx;
	mVertices[i].y = maxy;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	// line 5
	mVertices[++i].x = maxx;
	mVertices[i].y = miny;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	mVertices[++i].x = maxx;
	mVertices[i].y = miny;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	// line 6
	mVertices[++i].x = maxx;
	mVertices[i].y = miny;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	mVertices[++i].x = maxx;
	mVertices[i].y = maxy;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	// line 7
	mVertices[++i].x = minx;
	mVertices[i].y = maxy;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	mVertices[++i].x = maxx;
	mVertices[i].y = maxy;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	// line 8
	mVertices[++i].x = minx;
	mVertices[i].y = maxy;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	mVertices[++i].x = minx;
	mVertices[i].y = miny;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	// line 9
	mVertices[++i].x = maxx;
	mVertices[i].y = maxy;
	mVertices[i].z = minz;
	mVertices[i].color = mColor;
	mVertices[++i].x = maxx;
	mVertices[i].y = maxy;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	// line 10
	mVertices[++i].x = maxx;
	mVertices[i].y = miny;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	mVertices[++i].x = maxx;
	mVertices[i].y = maxy;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	// line 11
	mVertices[++i].x = minx;
	mVertices[i].y = miny;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
	mVertices[++i].x = maxx;
	mVertices[i].y = miny;
	mVertices[i].z = maxz;
	mVertices[i].color = mColor;
}

void ElWireBoundingBox::render(ElCamera* camera, TimeValue t)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (SUCCEEDED(d3dDevice->BeginScene()))
	{
		// AABB is normally rendered after world transform, so we do not
		// need to transform it anymore
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		d3dDevice->SetTransform(D3DTS_WORLD, &mat);
		
		d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		
		d3dDevice->SetFVF(VERTEX::FVF);

		d3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 12, mVertices.base(), sizeof(VERTEX));

		d3dDevice->EndScene();
		d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
}