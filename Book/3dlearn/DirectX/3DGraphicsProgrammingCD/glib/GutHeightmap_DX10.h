#ifndef _GUTHEIGHTMAP_DX10_
#define _GUTHEIGHTMAP_DX10_

#include <d3d10.h>

#include "GutHeightmap.h"
#include "GutModel_DX10.h"

class CGutHeightmapDX10 : public CGutHeightmap
{
private:
	ID3D10Buffer *m_pShaderConstant[3];
	ID3D10Buffer *m_pVertexBuffer;
	ID3D10Buffer *m_pIndexBuffer;
	ID3D10VertexShader *m_pVertexShader;
	ID3D10PixelShader *m_pPixelShader;
	ID3D10InputLayout *m_pVertexLayout;

public:
	CGutHeightmapDX10(void);
	~CGutHeightmapDX10();

	virtual void Release(void);
	virtual bool BuildMesh(int x_grids, int y_grids);

	bool LoadShader(const char *filename);
	void Render(Matrix4x4 &world_matrix, Matrix4x4 &view_matrix, Matrix4x4 &proj_matrix);
};

#endif // _GUTHEIGHTMAP_DX10_