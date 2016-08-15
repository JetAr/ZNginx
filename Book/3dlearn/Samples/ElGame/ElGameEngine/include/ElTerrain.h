#ifndef __ElTerrain_H__
#define __ElTerrain_H__

#include "ElD3DHeaders.h"
#include "ElArray.h"
#include "ElTexture.h"
#include "ElCamera.h"
#include "ElRenderable.h"

#define ElTerrainLayers			4

class ElShaderTerrain;
class ElRayTerrainQuery;

class ElTerrain : public ElRenderable
{
	friend class ElShaderTerrain;

public:
	struct VERTEX
	{
		float			x, y, z;
		D3DXVECTOR3		n;
		float			u0, v0;
		float			u1, v1;
	};

	struct Fan
	{
		DWORD			center;
		DWORD			corner[4];
		DWORD			edge[4];
	};

	struct VertexIndex
	{
		int				x, y;
	};

	struct Patch
	{
		Fan				fan;
		void set(VertexIndex center, unsigned int numVertexPerSide);
	};

	enum QuadMark
	{
		QUADMARK_NULL = 0,
		QUADMARK_SPLIT,
		QUADMARK_DRAW,
		QUADMARK_CUT
	};

	struct QuadPatch
	{
		Fan				fan;

		VertexIndex		center;
		int				lod;
		float			boundHeight;
		float			roughness;

		QuadPatch* children[4];

		QuadPatch()
		{
			memset((void*)children, 0, sizeof(children));
		}
		void set(VertexIndex center, int lod, unsigned int numVertexPerSide);
	};

public:
	ElTerrain();
	virtual ~ElTerrain();

	bool create(unsigned int size, bool lod = true, float tileSizeLength = 1.0f);
	void update(D3DLIGHT9* diffuse, ColorValue ambient);
	virtual void render(ElCamera* camera, TimeValue t);
	
	void increaseLodFactor1();
	void decreaseLodFactor1();
	void increaseLodFactor2();
	void decreaseLodFactor2();

	bool setLayerMap(int idx, LPCTSTR filename);
	bool setHeightMap(LPCTSTR filename);

	float getHeightAtPoint(float x, float z, D3DXVECTOR3* normal = 0);
	void intersects(ElRayTerrainQuery& rayTerrainQuery);

protected:
	void calculateVariables();
	void preAllocMemory();
	void createVertexDel();
	
	void buildVertices();
	void buildIndices(ElCamera* camera);

	void buildPatchList();
	void renderPatchList();

	void buildQuadTree();
	void buildQuadNode(QuadPatch* node, VertexIndex center, int lod);
	void buildQuadTreeBound();
	float buildQuadNodeBound(QuadPatch* node);
	
	void markQuadTree(ElCamera* camera);
	void markQuadNode(QuadPatch* node, ElCamera* camera);
	void markQuadNodeCut(QuadPatch* node);
	bool isQuadNodeVisible(QuadPatch*node, ElCamera* camera);
	bool isQuadNodeNeedSplit(QuadPatch* node, ElCamera* camera);
	bool isQuadNodeCanSplit(QuadPatch* node, int queuePos);
	bool isQuadNodeInQueue(DWORD index, int startQueuePos);

	void renderQuadTree();
	void renderQuadNode(QuadPatch* node);
	
	void drawPrimitive();

	void destroyQuadTree();
	void destroyQuadNode(QuadPatch* node);

	bool createShader();

protected:
	// data
	bool							mLod;
	unsigned int					mNumTilePerSide;
	float							mSideLengthScalar;
	float							mHeightScalar;
	float							mBaseHeight;
	unsigned int					mNumVertexPerSide;

	unsigned int					mNumVertex;
	unsigned int					mNumMaxIndex;
	unsigned int					mNumIndex;
	ElArray<VERTEX>					mVertices;
	ElArray<DWORD>					mIndices;

	IDirect3DVertexDeclaration9*	mVertexDeclaration;
	IDirect3DVertexBuffer9*			mVertexBuffer;
	IDirect3DIndexBuffer9*			mIndexBuffer;
	ElTexture						mTexture[ElTerrainLayers];

	float							mSlopeNormalFactor;
	ElArray<unsigned char>			mHeightMap;

	// patch list
	ElArray<Patch>					mPatchList;

	// quad tree
	QuadPatch*						mQuadTree;
	ElArray<unsigned char>			mQuardMarkMap;

	int								mTopLod;
	float							mLodFactor1;
	float							mLodFactor2;

	ElArray<QuadPatch*>				mQuadQueue0;
	unsigned int					m_nQuadQueueSize0;
	ElArray<QuadPatch*>				mQuadQueue1;
	unsigned int					m_nQuadQueueSize1;

	// light
	D3DLIGHT9*						mDiffuse;
	ColorValue						mAmbient;

	// shader
	ElShaderTerrain*				mShader;
};

#endif //__ElTerrain_H__