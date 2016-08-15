#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElTerrain.h"
#include "ElRayTerrainQuery.h"
#include "ElDeviceManager.h"
#include "ElShaderTerrain.h"
#include <fstream>

#define ElTerrainShaderFile			"..\\Media\\Terrain\\Terrain.fx"

ElTerrain::ElTerrain()
: mLod(true)
, mNumTilePerSide(0)
, mSideLengthScalar(1.0f)
, mHeightScalar(2.0f)
, mBaseHeight(0.0f)
, mNumVertexPerSide(0)
, mSlopeNormalFactor(4.5f)
, mTopLod(0)
, mLodFactor1(4.0f)
, mLodFactor2(2.5f)
, mNumVertex(0)
, mNumMaxIndex(0)
, mNumIndex(0)
, mVertexDeclaration(NULL)
, mVertexBuffer(NULL)
, mIndexBuffer(NULL)
, mQuadTree(NULL)
, mDiffuse(NULL)
, mAmbient(D3DCOLOR_ARGB(255, 255, 255, 255))
, mShader(NULL)
{
	memset((void*)mTexture, NULL, sizeof(mTexture));
}

void ElTerrain::Patch::set(VertexIndex center, unsigned int numVertexPerSide)
{
	// center
	fan.center = center.y * numVertexPerSide + center.x;

	// corner
	VertexIndex corner[4];

	corner[0].x = center.x - 1;
	corner[0].y = center.y + 1;
	fan.corner[0] = corner[0].y * numVertexPerSide + corner[0].x;

	corner[1].x = center.x + 1;
	corner[1].y = center.y + 1;
	fan.corner[1] = corner[1].y * numVertexPerSide + corner[1].x;

	corner[2].x = center.x + 1;
	corner[2].y = center.y - 1;
	fan.corner[2] = corner[2].y * numVertexPerSide + corner[2].x;

	corner[3].x = center.x - 1;
	corner[3].y = center.y - 1;
	fan.corner[3] = corner[3].y * numVertexPerSide + corner[3].x;

	// edge
	VertexIndex edge[4];

	edge[0].x = center.x;
	edge[0].y = center.y + 1;
	fan.edge[0] = edge[0].y * numVertexPerSide + edge[0].x;

	edge[1].x = center.x + 1;
	edge[1].y = center.y;
	fan.edge[1] = edge[1].y * numVertexPerSide + edge[1].x;

	edge[2].x = center.x;
	edge[2].y = center.y - 1;
	fan.edge[2] = edge[2].y * numVertexPerSide + edge[2].x;

	edge[3].x = center.x - 1;
	edge[3].y = center.y;
	fan.edge[3] = edge[3].y * numVertexPerSide + edge[3].x;
}

void ElTerrain::QuadPatch::set(VertexIndex center, int lod, unsigned int numVertexPerSide)
{
	// lod
	this->lod = lod;
	
	// center
	this->center = center;
	fan.center = center.y * numVertexPerSide + center.x;
	
	int step = (int)pow((long double)2, lod);

	// corner
	VertexIndex corner[4];

	corner[0].x = center.x - step;
	corner[0].y = center.y + step;
	fan.corner[0] = corner[0].y * numVertexPerSide + corner[0].x;

	corner[1].x = center.x + step;
	corner[1].y = center.y + step;
	fan.corner[1] = corner[1].y * numVertexPerSide + corner[1].x;

	corner[2].x = center.x + step;
	corner[2].y = center.y - step;
	fan.corner[2] = corner[2].y * numVertexPerSide + corner[2].x;

	corner[3].x = center.x - step;
	corner[3].y = center.y - step;
	fan.corner[3] = corner[3].y * numVertexPerSide + corner[3].x;

	// edge
	VertexIndex edge[4];

	edge[0].x = center.x;
	edge[0].y = center.y + step;
	fan.edge[0] = edge[0].y * numVertexPerSide + edge[0].x;

	edge[1].x = center.x + step;
	edge[1].y = center.y;
	fan.edge[1] = edge[1].y * numVertexPerSide + edge[1].x;

	edge[2].x = center.x;
	edge[2].y = center.y - step;
	fan.edge[2] = edge[2].y * numVertexPerSide + edge[2].x;

	edge[3].x = center.x - step;
	edge[3].y = center.y;
	fan.edge[3] = edge[3].y * numVertexPerSide + edge[3].x;
}

ElTerrain::~ElTerrain()
{
	destroyQuadTree();

	ElSafeDelete(mShader);

	ElSafeRelease(mIndexBuffer);
	ElSafeRelease(mVertexBuffer);
	ElSafeRelease(mVertexDeclaration);
}

bool ElTerrain::create(unsigned int size, bool lod /* = true */, float tileSizeLength /* = 1.0f */)
{
	assert(size >= 1);

	mNumTilePerSide		= (unsigned int)pow((long double)2, (int)size);
	mSideLengthScalar	= tileSizeLength;
	mLod				= lod;
	mTopLod				= size - 1;

	// setup variables and data
	calculateVariables();
	preAllocMemory();
	createVertexDel();

	// build vertices
	buildVertices();

	// build structure for organization of indices
	if (mLod)
	{
		buildQuadTree();
		buildQuadTreeBound();
	}
	else
		buildPatchList();

	// create shader
	if (!createShader())
		return false;

	return true;
}

void ElTerrain::update(D3DLIGHT9* diffuse, ColorValue ambient)
{
	mDiffuse = diffuse;
	mAmbient = ambient;
}

void ElTerrain::render(ElCamera* camera, TimeValue t)
{
	buildIndices(camera);
	if (mNumIndex == 0)
		return;

	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->SetVertexDeclaration(mVertexDeclaration);
	d3dDevice->SetStreamSource(0, mVertexBuffer, 0, sizeof(VERTEX));
	d3dDevice->SetIndices(mIndexBuffer);
	mShader->render(camera);
}

float ElTerrain::getHeightAtPoint(float x, float z, D3DXVECTOR3* normal /* = 0 */)
{
	// Scale down by the transformation that makes the cell_spacing equal to one.
	x /= mSideLengthScalar;
	z /= mSideLengthScalar;

	// From now on, we will interpret our positive z-axis as going in the 'down' direction, 
	// rather than the 'up' direction. This allows to extract the row and column simply by 
	// 'flooring' x and z:
	float col = floorf(x);
	float row = floorf(z);

	// ensures row and col are valid
	if (row < 0)
		row = 0;

	if (row >= mNumTilePerSide - 1)
		row = mNumTilePerSide - 1;

	if (col < 0)
		col = 0;

	if (col >= mNumTilePerSide - 1)
		col = mNumTilePerSide - 1;

	// get the heights of the quad we're in
	int row_idx = (int)row;
	int col_idx = (int)col;

	float AHeight = mVertices[row_idx * mNumVertexPerSide + col_idx].y;
	float BHeight = mVertices[row_idx * mNumVertexPerSide + (col_idx + 1)].y;
	float CHeight = mVertices[(row_idx + 1) * mNumVertexPerSide + col_idx].y;
	float DHeight = mVertices[(row_idx + 1) * mNumVertexPerSide + (col_idx + 1)].y;

	D3DXVECTOR3 ANormal, BNormal, CNormal, DNormal;
	if (normal)
	{
		ANormal = mVertices[row_idx * mNumVertexPerSide + col_idx].n;
		BNormal = mVertices[row_idx * mNumVertexPerSide + (col_idx + 1)].n;
		CNormal = mVertices[(row_idx + 1) * mNumVertexPerSide + col_idx].n;
		DNormal = mVertices[(row_idx + 1) * mNumVertexPerSide + (col_idx + 1)].n;
	}

	//
	// Find the triangle we are in:
	// 
	// 1)  C   D
	//	   *---*
	//     | / |
	//     *---*  
	//     A   B	
	//
	//
	// 2)  C   D
	//     *---*
	//     | \ |
	//     *---*  
	//     A   B
	//
	float dx = x - col;
	float dz = z - row;

	float height = 0.0f;

	if (normal)
		*normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (row_idx % 2 == col_idx % 2) // situation 1
	{
		if (dz > dx)	// upper triangle ACD
		{
			float uy = DHeight - CHeight;	// C->D
			float vy = AHeight - CHeight;	// C->A

			height = CHeight + lerp(0.0f, uy, dx) + lerp(0.0f, vy, 1.0f - dz);

			if (normal)
			{
				D3DXVECTOR3 un = DNormal - CNormal;	// C->D
				D3DXVECTOR3 vn = ANormal - CNormal;	// C->A

				*normal = CNormal + lerp(D3DXVECTOR3(0.0f, 0.0f, 0.0f), un, dx) + lerp(D3DXVECTOR3(0.0f, 0.0f, 0.0f), vn, 1.0f - dz);
			}
		}
		else	// lower triangle ABD
		{
			float uy = AHeight - BHeight;	// B->A
			float vy = DHeight - BHeight;	// B->D

			height = BHeight + lerp(0.0f, uy, 1.0f - dx) + lerp(0.0f, vy, dz);

			if (normal)
			{
				D3DXVECTOR3 un = ANormal - BNormal;	// B->A
				D3DXVECTOR3 vn = DNormal - BNormal; // B->D

				*normal = BNormal + lerp(D3DXVECTOR3(0.0f, 0.0f, 0.0f), un, 1.0 - dx) + lerp(D3DXVECTOR3(0.0f, 0.0f, 0.0f), vn, dz);
			}
		}
	}
	else	// situation 2
	{
		if (dz < 1.0f - dx)	// lower triangle ACB
		{
			float uy = BHeight - AHeight;	// A->B
			float vy = CHeight - AHeight;	// A->C

			height = AHeight + lerp(0.0f, uy, dx) + lerp(0.0f, vy, dz);

			if (normal)
			{
				D3DXVECTOR3 un = BNormal - ANormal; // A->B
				D3DXVECTOR3 vn = CNormal - ANormal; // A->C

				*normal = ANormal + lerp(D3DXVECTOR3(0.0f, 0.0f, 0.0f), un, dx) + lerp(D3DXVECTOR3(0.0f, 0.0f, 0.0f), vn, dz);
			}
		}
		else	// upper triangle BCD
		{
			float uy = CHeight - DHeight; // D->C
			float vy = BHeight - DHeight; // D->B

			height = DHeight + lerp(0.0f, uy, 1.0f - dx) + lerp(0.0f, vy, 1.0f - dz);

			if (normal)
			{
				D3DXVECTOR3 un = CNormal - DNormal;	// D->C
				D3DXVECTOR3 vn = BNormal - DNormal;	// D->B

				*normal = DNormal + lerp(D3DXVECTOR3(0.0f, 0.0f, 0.0f), un, 1.0f - dx) + lerp(D3DXVECTOR3(0.0f, 0.0f, 0.0f), vn, 1.0f - dz);
			}
		}
	}

	return height;
}

void ElTerrain::increaseLodFactor1()
{
	mLodFactor1 *= 2.0f;
}

void ElTerrain::decreaseLodFactor1()
{
	mLodFactor1 /= 2.0f;
}

void ElTerrain::increaseLodFactor2()
{
	mLodFactor2 *= 2.0f;
}

void ElTerrain::decreaseLodFactor2()
{
	mLodFactor2 /= 2.0f;
}

bool ElTerrain::setLayerMap(int idx, LPCTSTR filename)
{
	assert(idx >= 0 && idx < ElTerrainLayers);
	mTexture[idx].loadFromFile(filename);

	return true;
}

bool ElTerrain::setHeightMap(LPCTSTR filename)
{
	mHeightMap.assign(mNumVertex);
	memset((void*)mHeightMap.base(), 0, sizeof(unsigned char) * mNumVertex);

	std::ifstream kFile(filename, std::ios_base::binary);
	if (!kFile.good())
		return false;
	kFile.read((char*)mHeightMap.base(), mNumVertex);
	kFile.close();

	// cause the height map has been re-assigned, we should rebuild vertices
	buildVertices();
	if (mLod)
		buildQuadTreeBound();

	return true;
}

void ElTerrain::intersects(ElRayTerrainQuery& rayTerrainQuery)
{
	const ElRay& ray = rayTerrainQuery.getRay();
	float dist, u, v;

	for (unsigned int i = 0; i < mNumIndex; ++i)
	{
		D3DXVECTOR3 v0 = D3DXVECTOR3(mVertices[mIndices[i]].x, mVertices[mIndices[i]].y, mVertices[mIndices[i]].z); ++i;
		D3DXVECTOR3 v1 = D3DXVECTOR3(mVertices[mIndices[i]].x, mVertices[mIndices[i]].y, mVertices[mIndices[i]].z); ++i;
		D3DXVECTOR3 v2 = D3DXVECTOR3(mVertices[mIndices[i]].x, mVertices[mIndices[i]].y, mVertices[mIndices[i]].z);

		if (ray.intersects(v0, v1, v2, dist, u, v))
		{
			D3DXVECTOR3 intersection = v0 + u * (v1 - v0) + v * (v2 - v0);
			rayTerrainQuery.queryResult(intersection, dist);
		}
	}
}

void ElTerrain::calculateVariables()
{
	mNumVertexPerSide = mNumTilePerSide + 1;
	mNumVertex = mNumVertexPerSide * mNumVertexPerSide;
	mNumMaxIndex = mNumTilePerSide * mNumTilePerSide * 6;
}

void ElTerrain::createVertexDel()
{
	D3DVERTEXELEMENT9 COMMONVERTEXDEC[] =
	{
		// 1st stream - position, diffuse
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },

		D3DDECL_END()
	};

	ElSafeRelease(mVertexDeclaration);
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->CreateVertexDeclaration(COMMONVERTEXDEC, &mVertexDeclaration);
}

void ElTerrain::preAllocMemory()
{
	mVertices.assign(mNumVertex);
	mIndices.assign(mNumMaxIndex);

	if (mLod)
	{
		mQuadQueue0.assign(mNumMaxIndex);
		m_nQuadQueueSize0 = 0;
		mQuadQueue1.assign(mNumMaxIndex);
		m_nQuadQueueSize1 = 0;
	}
}

void ElTerrain::buildVertices()
{
	for (unsigned int i = 0; i < mNumVertexPerSide; ++i)
	{
		for (unsigned int j = 0; j < mNumVertexPerSide; ++j)
		{
			unsigned int idx = mNumVertexPerSide * i + j;

			// position
			mVertices[idx].x = mSideLengthScalar * (float)j;
			mVertices[idx].y = mHeightMap.empty() ? 0.0f : (mHeightScalar * (float)mHeightMap[idx] * 0.25f);
			mVertices[idx].z = mSideLengthScalar * (float)i;
			
			// uv
			mVertices[idx].u0 = (float)j;
			mVertices[idx].v0 = (float)i;
			mVertices[idx].u1 = (float)j / (float)mNumTilePerSide;
			mVertices[idx].v1 = (float)i / (float)mNumTilePerSide;
		}
	}

	// normal calculate using slope algorithm
	for (unsigned int i = 0; i < mNumVertexPerSide; ++i)
	{
		for (unsigned int j = 0; j < mNumVertexPerSide; ++j)
		{
			unsigned int idx = mNumVertexPerSide * i + j;
			unsigned int neighbor0 = mNumVertexPerSide * (i == mNumTilePerSide ? i : i + 1) + j;
			unsigned int neighbor1 = mNumVertexPerSide * (i == 0 ? i : i - 1) + j;
			unsigned int neighbor2 = mNumVertexPerSide * i + (j == mNumTilePerSide ? j : j + 1);
			unsigned int neighbor3 = mNumVertexPerSide * i + (j == 0 ? j : j - 1);

			mVertices[idx].n.x = mVertices[neighbor0].y - mVertices[neighbor1].y;
			mVertices[idx].n.y = mSlopeNormalFactor;
			mVertices[idx].n.z = mVertices[neighbor2].y - mVertices[neighbor3].y;

			D3DXVec3Normalize(&mVertices[idx].n, &mVertices[idx].n);
		}
	}

	unsigned int size = sizeof(VERTEX) * mNumVertex;

	ElSafeRelease(mVertexBuffer);
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVertexBuffer, NULL);

	void* buff = NULL;
	mVertexBuffer->Lock(0, size, &buff, 0);
	memcpy(buff, mVertices.base(), size);
	mVertexBuffer->Unlock();
}

void ElTerrain::buildIndices(ElCamera* camera)
{
	if (mLod)
	{
		markQuadTree(camera);
		renderQuadTree();
	}
	else
		renderPatchList();

	if (mNumIndex == 0)
		return;

	unsigned int size = sizeof(DWORD) * mNumIndex;

	ElSafeRelease(mIndexBuffer);
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->CreateIndexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &mIndexBuffer, NULL);

	void* buff = NULL;
	mIndexBuffer->Lock(0, size, &buff, 0);
	memcpy(buff, mIndices.base(), size);
	mIndexBuffer->Unlock();
}

void ElTerrain::buildPatchList()
{
	unsigned int numPatchPerSide = mNumTilePerSide / 2;
	unsigned int numPatches = numPatchPerSide * numPatchPerSide;
	mPatchList.assign(numPatches);

	for (unsigned int i = 0; i < numPatchPerSide; ++i)
	{
		for (unsigned int j = 0; j < numPatchPerSide; ++j)
		{
			unsigned int idx = numPatchPerSide * i + j;
			
			VertexIndex center;
			center.x = j * 2 + 1;
			center.y = i * 2 + 1;

			mPatchList[idx].set(center, mNumVertexPerSide);
		}
	}
}

void ElTerrain::renderPatchList()
{
	mNumIndex = 0;

	unsigned int numPatchPerSide = mNumTilePerSide / 2;

	for (unsigned int i = 0; i < numPatchPerSide; ++i)
	{
		for (unsigned int j = 0; j < numPatchPerSide; ++j)
		{
			unsigned int idx = numPatchPerSide * i + j;

			mIndices[mNumIndex++] = mPatchList[idx].fan.center;
			mIndices[mNumIndex++] = mPatchList[idx].fan.corner[0];
			mIndices[mNumIndex++] = mPatchList[idx].fan.edge[0];

			mIndices[mNumIndex++] = mPatchList[idx].fan.center;
			mIndices[mNumIndex++] = mPatchList[idx].fan.edge[0];
			mIndices[mNumIndex++] = mPatchList[idx].fan.corner[1];

			mIndices[mNumIndex++] = mPatchList[idx].fan.center;
			mIndices[mNumIndex++] = mPatchList[idx].fan.corner[1];
			mIndices[mNumIndex++] = mPatchList[idx].fan.edge[1];

			mIndices[mNumIndex++] = mPatchList[idx].fan.center;
			mIndices[mNumIndex++] = mPatchList[idx].fan.edge[1];
			mIndices[mNumIndex++] = mPatchList[idx].fan.corner[2];

			mIndices[mNumIndex++] = mPatchList[idx].fan.center;
			mIndices[mNumIndex++] = mPatchList[idx].fan.corner[2];
			mIndices[mNumIndex++] = mPatchList[idx].fan.edge[2];

			mIndices[mNumIndex++] = mPatchList[idx].fan.center;
			mIndices[mNumIndex++] = mPatchList[idx].fan.edge[2];
			mIndices[mNumIndex++] = mPatchList[idx].fan.corner[3];

			mIndices[mNumIndex++] = mPatchList[idx].fan.center;
			mIndices[mNumIndex++] = mPatchList[idx].fan.corner[3];
			mIndices[mNumIndex++] = mPatchList[idx].fan.edge[3];

			mIndices[mNumIndex++] = mPatchList[idx].fan.center;
			mIndices[mNumIndex++] = mPatchList[idx].fan.edge[3];
			mIndices[mNumIndex++] = mPatchList[idx].fan.corner[0];
		}
	}
}

void ElTerrain::buildQuadTree()
{
	// build quad tree
	int centerIdx = mNumTilePerSide / 2;
	VertexIndex center;
	center.x = centerIdx;
	center.y = centerIdx;

	ElSafeDelete(mQuadTree);
	mQuadTree = ElNew QuadPatch;
	buildQuadNode(mQuadTree, center, mTopLod);

	// build quad mark map
	mQuardMarkMap.assign(mNumVertex);
}

void ElTerrain::buildQuadNode(QuadPatch* node, VertexIndex center, int lod)
{
	if (node)
	{
		node->set(center, lod, mNumVertexPerSide);

		if (lod > 0)
		{
			int childLod = lod - 1;
			int childStep = (int)pow((long double)2, childLod);
			VertexIndex kChildCenter;
			
			node->children[0] = ElNew QuadPatch;
			kChildCenter.x = node->center.x - childStep;
			kChildCenter.y = node->center.y + childStep;
			buildQuadNode(node->children[0], kChildCenter, childLod);

			node->children[1] = ElNew QuadPatch;
			kChildCenter.x = node->center.x + childStep;
			kChildCenter.y = node->center.y + childStep;
			buildQuadNode(node->children[1], kChildCenter, childLod);

			node->children[2] = ElNew QuadPatch;
			kChildCenter.x = node->center.x + childStep;
			kChildCenter.y = node->center.y - childStep;
			buildQuadNode(node->children[2], kChildCenter, childLod);

			node->children[3] = ElNew QuadPatch;
			kChildCenter.x = node->center.x - childStep;
			kChildCenter.y = node->center.y - childStep;
			buildQuadNode(node->children[3], kChildCenter, childLod);
		}
	}
}

void ElTerrain::buildQuadTreeBound()
{
	buildQuadNodeBound(mQuadTree);
}

float ElTerrain::buildQuadNodeBound(QuadPatch* node)
{
	float fHeight = mBaseHeight;
	
	if (node)
	{
		// bound
		fHeight = max(mVertices[node->fan.center].y,
			max(mVertices[node->fan.edge[0]].y,
			max(mVertices[node->fan.edge[1]].y,
			max(mVertices[node->fan.edge[2]].y,
			max(mVertices[node->fan.edge[3]].y,
			max(mVertices[node->fan.corner[0]].y,
			max(mVertices[node->fan.corner[1]].y,
			max(mVertices[node->fan.corner[2]].y, mVertices[node->fan.corner[3]].y))))))));

		float fChildHeight0 = buildQuadNodeBound(node->children[0]);
		float fChildHeight1 = buildQuadNodeBound(node->children[1]);
		float fChildHeight2 = buildQuadNodeBound(node->children[2]);
		float fChildHeight3 = buildQuadNodeBound(node->children[3]);
		float fChildHeight = max(fChildHeight0, max(fChildHeight1, max(fChildHeight2, fChildHeight3)));

		fHeight = max(fHeight, fChildHeight);

		node->boundHeight = fHeight;

		// roughness
		if (node->lod > 0)
		{
			float fSideLen = pow((long double)2, (node->lod + 1)) * mSideLengthScalar;

			float fDiffHeight0 = fabs(mVertices[node->fan.edge[0]].y - (mVertices[node->fan.corner[0]].y + mVertices[node->fan.corner[1]].y) / 2.0f);
			float fDiffHeight1 = fabs(mVertices[node->fan.edge[1]].y - (mVertices[node->fan.corner[1]].y + mVertices[node->fan.corner[2]].y) / 2.0f);
			float fDiffHeight2 = fabs(mVertices[node->fan.edge[2]].y - (mVertices[node->fan.corner[2]].y + mVertices[node->fan.corner[3]].y) / 2.0f);
			float fDiffHeight3 = fabs(mVertices[node->fan.edge[3]].y - (mVertices[node->fan.corner[3]].y + mVertices[node->fan.corner[0]].y) / 2.0f);
			float fDiffHeight4 = fabs(mVertices[node->fan.center].y - (mVertices[node->fan.corner[0]].y + mVertices[node->fan.corner[2]].y) / 2.0f);
			float fDiffHeight5 = fabs(mVertices[node->fan.center].y - (mVertices[node->fan.corner[1]].y + mVertices[node->fan.corner[3]].y) / 2.0f);

			node->roughness = (fDiffHeight0 + fDiffHeight1 + fDiffHeight2 + fDiffHeight3 + fDiffHeight4 + fDiffHeight5) / fSideLen;
		}
		else
			node->roughness = 0.0f;
	}

	return fHeight;
}

void ElTerrain::markQuadTree(ElCamera* camera)
{
	memset((void*)mQuardMarkMap.base(), QUADMARK_NULL, sizeof(unsigned char) * mNumVertex);

	m_nQuadQueueSize0 = 0;
	m_nQuadQueueSize1 = 0;
	
	markQuadNode(mQuadTree, camera);
}

void ElTerrain::markQuadNode(QuadPatch* node, ElCamera* camera)
{
	if (node)
	{
		mQuadQueue0[m_nQuadQueueSize0++] = node;

		int nLodLevel = node->lod;
		while (nLodLevel >= 0)
		{
			for (int i = 0; i < m_nQuadQueueSize0; ++i)
			{
				QuadPatch* pkQueueNode = mQuadQueue0[i];

				if (!isQuadNodeVisible(pkQueueNode, camera))
					markQuadNodeCut(pkQueueNode);
				else if (!isQuadNodeNeedSplit(pkQueueNode, camera))
					mQuardMarkMap[pkQueueNode->fan.center] = QUADMARK_DRAW;
				else
				{
					if (isQuadNodeCanSplit(pkQueueNode, i))
					{
						mQuardMarkMap[pkQueueNode->fan.center] = QUADMARK_SPLIT;

						for (int i = 0; i < 4; ++i)
						{
							if (pkQueueNode->children[i])
								mQuadQueue1[m_nQuadQueueSize1++] = pkQueueNode->children[i];
						}
					}
					else
						mQuardMarkMap[pkQueueNode->fan.center] = QUADMARK_DRAW;
				}
			}

			// swap
			mQuadQueue0.swap(mQuadQueue1);

			m_nQuadQueueSize0 = m_nQuadQueueSize1;
			m_nQuadQueueSize1 = 0;

			// next level
			--nLodLevel;
		}
	}
}

void ElTerrain::markQuadNodeCut(QuadPatch* node)
{
	if (node)
	{
		int step = (int)pow((long double)2, node->lod);

		unsigned int x0 = node->center.x - step;
		unsigned int x1 = node->center.x + step;
		unsigned int y0 = node->center.y - step;
		unsigned int y1 = node->center.y + step;

		size_t unBuffSize = (step * 2 - 2) * sizeof(unsigned char);
		for (unsigned int i = y0 + 1; i < y1; ++i)
			memset((void*)&mQuardMarkMap[i * mNumVertexPerSide + x0 + 1], QUADMARK_CUT, unBuffSize);
	}
}

bool ElTerrain::isQuadNodeVisible(QuadPatch*node, ElCamera* camera)
{
	if (!camera)
		return false;
	
	float fXRadius = mVertices[node->fan.center].x - mVertices[node->fan.corner[0]].x;
	float fYHalfRadius = (node->boundHeight - mBaseHeight) / 2.0f;
	float fZRadius = mVertices[node->fan.center].z - mVertices[node->fan.corner[3]].z;
	return camera->isVisible(D3DXVECTOR3(mVertices[node->fan.center].x, mBaseHeight + fYHalfRadius, mVertices[node->fan.center].z), D3DXVECTOR3(fXRadius, fYHalfRadius, fZRadius));
}

bool ElTerrain::isQuadNodeNeedSplit(QuadPatch* node, ElCamera* camera)
{
	if (node->lod > 0)
	{
		// distence from eye to center of the patch
		D3DXVECTOR3 center;
		center.x = mVertices[node->fan.center].x;
		center.y = mVertices[node->fan.center].y;
		center.z = mVertices[node->fan.center].z;

		D3DXVECTOR3 kCameraPos(0.0f, 0.0f, 0.0f);
		if (camera)
			kCameraPos = camera->getRealPosition();

		D3DXVECTOR3 kDistence = center - kCameraPos;

		float fDistence = D3DXVec3Length(&kDistence);

		// side length of the patch
		float fSideLen = pow((long double)2, (node->lod + 1)) * mSideLengthScalar;

		// weight
		float fWeight2 = mLodFactor2 * node->roughness;

		float fWeight = fDistence / (fSideLen * mLodFactor1 * max(fWeight2, 1.0f));

		if (fWeight < 1.0f)
			return true;
	}

	return false;
}

bool ElTerrain::isQuadNodeCanSplit(QuadPatch* node, int queuePos)
{
	int neighborStep = (int)pow((long double)2, (node->lod + 1));

	int neighborCenterX, neighborCenterY;
	DWORD neighborIndex;

	// top neighbor
	bool topExist = true;

	neighborCenterY = node->center.y + neighborStep;
	if (neighborCenterY < mNumVertexPerSide)
	{
		neighborCenterX = node->center.x;

		neighborIndex = neighborCenterY * mNumVertexPerSide + neighborCenterX;

		if (mQuardMarkMap[neighborIndex] == QUADMARK_CUT || isQuadNodeInQueue(neighborIndex, queuePos))
			topExist = true;
		else
			topExist = false;
	}
	else
		topExist = true;

	// right neighbor
	bool rightExist = true;

	neighborCenterX = node->center.x + neighborStep;
	if (neighborCenterX < mNumVertexPerSide)
	{
		neighborCenterY = node->center.y;

		neighborIndex = neighborCenterY * mNumVertexPerSide + neighborCenterX;

		if (mQuardMarkMap[neighborIndex] == QUADMARK_CUT || isQuadNodeInQueue(neighborIndex, queuePos))
			rightExist = true;
		else
			rightExist = false;
	}
	else
		rightExist = true;

	// bottom neighbor
	bool bottomExist = true;

	neighborCenterY = node->center.y - neighborStep;
	if (neighborCenterY > 0)
	{
		neighborCenterX = node->center.x;

		neighborIndex = neighborCenterY * mNumVertexPerSide + neighborCenterX;

		if (mQuardMarkMap[neighborIndex] == QUADMARK_CUT || isQuadNodeInQueue(neighborIndex, queuePos))
			bottomExist = true;
		else
			bottomExist = false;
	}
	else
		bottomExist = true;

	// left neighbor
	bool leftExist = true;

	neighborCenterX = node->center.x - neighborStep;
	if (neighborCenterX > 0)
	{
		neighborCenterY = node->center.y;

		neighborIndex = neighborCenterY * mNumVertexPerSide + neighborCenterX;

		if (mQuardMarkMap[neighborIndex] == QUADMARK_CUT || isQuadNodeInQueue(neighborIndex, queuePos))
			leftExist = true;
		else
			leftExist = false;
	}
	else
		leftExist = true;

	return topExist && rightExist && bottomExist && leftExist;
}

bool ElTerrain::isQuadNodeInQueue(DWORD index, int startQueuePos)
{
	bool bExist = false;

	int nForIndex = startQueuePos;
	int nBackIndex = startQueuePos;

	while (nForIndex >= 0 || nBackIndex < m_nQuadQueueSize0)
	{
		if (nForIndex >= 0)
		{
			--nForIndex;;

			if (nForIndex >= 0)
			{
				if (mQuadQueue0[nForIndex]->fan.center == index)
				{
					bExist = true;
					break;
				}
			}
		}

		if (nBackIndex < m_nQuadQueueSize0)
		{
			++nBackIndex;

			if (nBackIndex < m_nQuadQueueSize0)
			{
				if (mQuadQueue0[nBackIndex]->fan.center == index)
				{
					bExist = true;
					break;
				}
			}
		}
	}

	return bExist;
}

void ElTerrain::renderQuadTree()
{
	mNumIndex = 0;

	renderQuadNode(mQuadTree);
}

void ElTerrain::renderQuadNode(QuadPatch* node)
{
	if (node)
	{
		if (mQuardMarkMap[node->fan.center] == QUADMARK_DRAW)
		{
			int neighborStep = (int)pow((long double)2, (node->lod + 1));
			
			int neighborCenterX, neighborCenterY;
			DWORD neighborIndex;

			// top neighbor
			bool topSplit = true;
			
			neighborCenterY = node->center.y + neighborStep;
			if (neighborCenterY < mNumVertexPerSide)
			{
				neighborCenterX = node->center.x;

				neighborIndex = neighborCenterY * mNumVertexPerSide + neighborCenterX;

				if (mQuardMarkMap[neighborIndex] != QUADMARK_NULL)
					topSplit = true;
				else
					topSplit = false;
			}
			else
				topSplit = true;

			// right neighbor
			bool rightSplit = true;

			neighborCenterX = node->center.x + neighborStep;
			if (neighborCenterX < mNumVertexPerSide)
			{
				neighborCenterY = node->center.y;

				neighborIndex = neighborCenterY * mNumVertexPerSide + neighborCenterX;

				if (mQuardMarkMap[neighborIndex] != QUADMARK_NULL)
					rightSplit = true;
				else
					rightSplit = false;
			}
			else
				rightSplit = true;

			// bottom neighbor
			bool bottomSplit = true;

			neighborCenterY = node->center.y - neighborStep;
			if (neighborCenterY > 0)
			{
				neighborCenterX = node->center.x;

				neighborIndex = neighborCenterY * mNumVertexPerSide + neighborCenterX;

				if (mQuardMarkMap[neighborIndex] != QUADMARK_NULL)
					bottomSplit = true;
				else
					bottomSplit = false;
			}
			else
				bottomSplit = true;

			// left neighbor
			bool leftSplit = true;

			neighborCenterX = node->center.x - neighborStep;
			if (neighborCenterX > 0)
			{
				neighborCenterY = node->center.y;

				neighborIndex = neighborCenterY * mNumVertexPerSide + neighborCenterX;

				if (mQuardMarkMap[neighborIndex] != QUADMARK_NULL)
					leftSplit = true;
				else
					leftSplit = false;
			}
			else
				leftSplit = true;

			// add triangles to index buffer to render
			if (topSplit)
			{
				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.corner[0];
				mIndices[mNumIndex++] = node->fan.edge[0];

				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.edge[0];
				mIndices[mNumIndex++] = node->fan.corner[1];
			}
			else
			{
				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.corner[0];
				mIndices[mNumIndex++] = node->fan.corner[1];
			}

			if (rightSplit)
			{
				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.corner[1];
				mIndices[mNumIndex++] = node->fan.edge[1];

				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.edge[1];
				mIndices[mNumIndex++] = node->fan.corner[2];
			}
			else
			{
				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.corner[1];
				mIndices[mNumIndex++] = node->fan.corner[2];
			}

			if (bottomSplit)
			{
				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.corner[2];
				mIndices[mNumIndex++] = node->fan.edge[2];

				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.edge[2];
				mIndices[mNumIndex++] = node->fan.corner[3];
			}
			else
			{
				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.corner[2];
				mIndices[mNumIndex++] = node->fan.corner[3];
			}

			if (leftSplit)
			{
				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.corner[3];
				mIndices[mNumIndex++] = node->fan.edge[3];

				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.edge[3];
				mIndices[mNumIndex++] = node->fan.corner[0];
			}
			else
			{
				mIndices[mNumIndex++] = node->fan.center;
				mIndices[mNumIndex++] = node->fan.corner[3];
				mIndices[mNumIndex++] = node->fan.corner[0];
			}
		}
		else if (mQuardMarkMap[node->fan.center] == QUADMARK_SPLIT)
		{
			for (int i = 0; i < 4; ++i)
				renderQuadNode(node->children[i]);
		}
	}
}

void ElTerrain::drawPrimitive()
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mVertices.size(), 0, mIndices.size() / 3);
}

void ElTerrain::destroyQuadTree()
{
	// destroy quad tree
	destroyQuadNode(mQuadTree);
}

void ElTerrain::destroyQuadNode(QuadPatch* node)
{
	if (node)
	{
		for (int i = 0; i < 4; ++i)
			destroyQuadNode(node->children[i]);
		ElSafeDelete(node);
	}
}

bool ElTerrain::createShader()
{
	ElSafeDelete(mShader);
	mShader = ElNew ElShaderTerrain(this);

	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (!mShader->create(ElTerrainShaderFile))
		return false;

	return true;
}
