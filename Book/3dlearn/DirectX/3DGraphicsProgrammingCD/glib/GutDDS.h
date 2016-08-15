#ifndef _GUTDDS_
#define _GUTDDS_

#include <ddraw.h>

struct DDS_IMAGE_DATA
{
	int		width;
	int		height;
	int		depth;
	int		BufferSize;
	int     numMipMaps;
	int		PixelSize;
	bool	bBlock;
	bool	cubeMap;
	DWORD	format;
	DDSURFACEDESC2 ddsd;

	unsigned char *pixels;
};

DDS_IMAGE_DATA* loadDDSTextureFile( const char *filename );

#endif // _GUTDDS_