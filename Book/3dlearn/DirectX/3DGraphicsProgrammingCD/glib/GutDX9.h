#ifndef _GUTDX9_
#define _GUTDX9_

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "GutDefs.h"

bool GutInitGraphicsDeviceDX9(GutDeviceSpec *spec=NULL);
bool GutReleaseGraphicsDeviceDX9(void);
bool GutResetGraphicsDeviceDX9(void);

LPDIRECT3DVERTEXSHADER9 GutLoadVertexShaderDX9_HLSL(const char *filename, const char *entry, const char *profile);
LPDIRECT3DPIXELSHADER9  GutLoadPixelShaderDX9_HLSL(const char *filename, const char *entry, const char *profile);

LPDIRECT3DVERTEXSHADER9 GutLoadVertexShaderDX9_Binary(const char *filename);
LPDIRECT3DPIXELSHADER9  GutLoadPixelShaderDX9_Binary(const char *filename);

LPDIRECT3DVERTEXSHADER9 GutLoadVertexShaderDX9_ASM(const char *filename);
LPDIRECT3DPIXELSHADER9  GutLoadPixelShaderDX9_ASM(const char *filename);

LPD3DXEFFECT GutLoadFXShaderDX9(const char *filename);

LPDIRECT3DDEVICE9 GutGetGraphicsDeviceDX9(void);

void GutSetupLightDX9(int index, sGutLight &light);

#endif // _GUTDX9_
