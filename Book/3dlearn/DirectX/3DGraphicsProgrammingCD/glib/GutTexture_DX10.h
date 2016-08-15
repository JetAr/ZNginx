#ifndef _GUTTEXTURE_DX10_
#define _GUTTEXTURE_DX10_

#include "GutImage.h"

ID3D10ShaderResourceView *GutLoadTexture_DX10(const char *filename, sImageInfo *pInfo=NULL, bool mipmap_enabled = true);
ID3D10ShaderResourceView *GutLoadCubemapTexture_DX10(const char **filename_array, bool mipmap_enabled = true);
ID3D10ShaderResourceView *GutLoadCubemapTexture_DX10(const char *filename);
ID3D10ShaderResourceView *GutLoadVolumeTexture_DX10(const char *filename);

#endif // _GUTTEXTURE_DX9_