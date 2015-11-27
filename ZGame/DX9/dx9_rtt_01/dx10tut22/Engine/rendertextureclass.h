////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURECLASS_H_
#define _RENDERTEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D10Device*, int, int);
	void Shutdown();

	void SetRenderTarget(ID3D10Device*, ID3D10DepthStencilView*);
	void ClearRenderTarget(ID3D10Device*, ID3D10DepthStencilView*, float, float, float, float);
	ID3D10ShaderResourceView* GetShaderResourceView();

private:
	ID3D10Texture2D* m_renderTargetTexture;
	ID3D10RenderTargetView* m_renderTargetView;
	ID3D10ShaderResourceView* m_shaderResourceView;
};

#endif
