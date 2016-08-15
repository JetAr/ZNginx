#ifdef _ENABLE_DX10_

#ifndef _GUTDX10_
#define _GUTDX10_

#include <d3d10.h>
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3dx10.lib")

#include "GutDefs.h"

bool GutInitGraphicsDeviceDX10(GutDeviceSpec *spec=NULL);
bool GutReleaseGraphicsDeviceDX10(void);
bool GutResetGraphicsDeviceDX10(void);

ID3D10Device *GutGetGraphicsDeviceDX10(void);
IDXGISwapChain *GutGetDX10SwapChain(void);

ID3D10RenderTargetView *GutGetDX10RenderTargetView(void);
ID3D10DepthStencilView *GutGetDX10DepthStencilView(void);

void GutSetHLSLShaderMacrosDX10(D3D10_SHADER_MACRO *pDefines);

ID3D10VertexShader* GutLoadVertexShaderDX10_HLSL(const char *filename, const char *entry, const char *profile, ID3D10Blob **pCode=NULL);
ID3D10PixelShader * GutLoadPixelShaderDX10_HLSL(const char *filename, const char *entry, const char *profile);
ID3D10Effect *GutLoadFXShaderDX10(const char *filename);

ID3D10Buffer *GutCreateBufferObject_DX10(int bindmask, D3D10_USAGE usage, int size, void *pInit);
ID3D10Buffer *GutCreateVertexBuffer_DX10(int size, void *pInit = NULL);
ID3D10Buffer *GutCreateIndexBuffer_DX10(int size, void *pInit = NULL);
ID3D10Buffer *GutCreateShaderConstant_DX10(int size, void *pInit = NULL);

ID3D10InputLayout *GutCreateInputLayoutDX10(const sVertexDecl *pVertexDecl, void *CodeSignature, int CodeSize);

bool GutCreateRenderTarget_DX10(int width, int height, DXGI_FORMAT fmt, 
								ID3D10Texture2D **pTexture, ID3D10ShaderResourceView **pRSView, ID3D10RenderTargetView **pDSView);
bool GutCreateDepthStencil_DX10(int width, int height, DXGI_FORMAT fmt, 
								ID3D10Texture2D **pTexture, ID3D10ShaderResourceView **pRSView, ID3D10DepthStencilView **pDSView);

void GutSetDX10DefaultRasterizerDesc(D3D10_RASTERIZER_DESC &desc);
void GutSetDX10DefaultBlendDesc(D3D10_BLEND_DESC &desc);
void GutSetDX10DefaultDepthStencilDesc(D3D10_DEPTH_STENCIL_DESC &desc);
void GutSetDX10DefaultSamplerDesc(D3D10_SAMPLER_DESC &desc);

#endif // _GUTDX10_

#endif // _ENABLE_DX10_