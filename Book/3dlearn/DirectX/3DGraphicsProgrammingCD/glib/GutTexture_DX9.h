#ifndef _GUTTEXTURE_DX9_
#define _GUTTEXTURE_DX9_

#include "GutImage.h"

LPDIRECT3DTEXTURE9 GutLoadTexture_DX9(const char *filename, sImageInfo *info=NULL, bool mipmap_enabled = true);
LPDIRECT3DVOLUMETEXTURE9 GutLoadVolumeTexture_DX9(const char *filename);
LPDIRECT3DCUBETEXTURE9 GutLoadCubemapTexture_DX9(const char **filename_array, bool mipmap_enabled = true);
LPDIRECT3DCUBETEXTURE9 GutLoadCubemapTexture_DX9(const char *filename);

void GutDrawScreenQuad_DX9(float x0, float y0, float width, float height, sImageInfo *pInfo = NULL);
void GutDrawFullScreenQuad_DX9(sImageInfo *pInfo = NULL);

#endif // _GUTTEXTURE_DX9_