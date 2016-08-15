#include <windows.h>
#include <stdio.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>
#include <d3d9.h>

#include "GutDDS.h"
#include "GutBMP.h"
#include "GutTGA.h"

GLuint GutLoadDDSTexture_OpenGL(const char *filename, sImageInfo *pInfo)
{
	DDS_IMAGE_DATA *pDDSImageData = loadDDSTextureFile(filename);

	if ( pDDSImageData==NULL )
		return 0;

	GLuint TextureID = 0;

	int nHeight     = pDDSImageData->height;
	int nWidth      = pDDSImageData->width;
	int nNumMipMaps = pDDSImageData->numMipMaps;
	nNumMipMaps = nNumMipMaps==0 ? 1 : nNumMipMaps;
	int nBlockSize = 0;
	int nElements = 0;

	GLint data_type = GL_UNSIGNED_BYTE;
	GLint data_fmt = GL_RGBA;
	DWORD format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

	if ( pInfo )
	{
		pInfo->m_iWidth = nWidth;
		pInfo->m_iHeight = nHeight;
	}

	switch(pDDSImageData->format)
	{
	case D3DFMT_DXT1:
		format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		nBlockSize = 8;
		break;
	case D3DFMT_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		nBlockSize = 16;
		break;
	case D3DFMT_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		nBlockSize = 16;
		break;
	case D3DFMT_R8G8B8:
		format = GL_RGB;
		nBlockSize = 0;
		nElements = 3;
		break;
	case D3DFMT_A8R8G8B8:
		format = GL_RGBA;
		data_fmt = GL_RGBA;
		nBlockSize = 0;
		nElements = 4;
		break;
	case D3DFMT_A32B32G32R32F:
		format = GL_RGBA32F_ARB;
		data_fmt = GL_RGBA;
		data_type = GL_FLOAT;
		nElements = 4;
		break;
	case D3DFMT_A16B16G16R16F:
		format = GL_RGBA16F_ARB;
		data_fmt = GL_RGBA;
		data_type = GL_HALF_FLOAT_ARB;
		nElements = 4;
		break;
	}

	glGenTextures( 1, &TextureID );
	glBindTexture( GL_TEXTURE_2D, TextureID );

	if ( nNumMipMaps > 1 )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	int nSize;
	int nOffset = 0;

	// Load the mip-map levels
	for( int i = 0; i < nNumMipMaps; ++i )
	{
		if( nWidth  == 0 ) nWidth  = 1;
		if( nHeight == 0 ) nHeight = 1;

		if ( nBlockSize )
		{
			nSize = ((nWidth+3)/4) * ((nHeight+3)/4) * nBlockSize;

			glCompressedTexImage2DARB( GL_TEXTURE_2D, i, format, 
				nWidth, nHeight, 0, nSize, pDDSImageData->pixels + nOffset );
		}
		else
		{
			nSize = nWidth * nHeight * pDDSImageData->PixelSize;

			glTexImage2D( GL_TEXTURE_2D, i, format, 
				nWidth, nHeight, 0,
				data_fmt, data_type, pDDSImageData->pixels + nOffset );
		}

		nOffset += nSize;

		// Half the image size for the next mip-map level...
		nWidth  = (nWidth  / 2);
		nHeight = (nHeight / 2);
	}

	if( pDDSImageData->pixels != NULL )
		free( pDDSImageData->pixels );

	free( pDDSImageData );

	return TextureID;
}

GLuint GutLoadCompressedCubemapTexture_OpenGL(const char **filename_array)
{
	// NOTE: Unlike "lena.bmp", "lena.dds" actually contains its own mip-map 
	// levels, which are also compressed.
	GLuint TextureID = 0;

	// 產生一個貼圖物件
	glGenTextures( 1, &TextureID );
	// 使用g_TextureID貼圖物件
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID );

	GLuint table[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for ( int f=0; f<6; f++ )
	{
		const char *filename = filename_array[f];
		DDS_IMAGE_DATA *pDDSImageData = loadDDSTextureFile(filename);

		if ( pDDSImageData==NULL )
		{
			glDeleteTextures(1, &TextureID);
			TextureID = 0;
			break;
		}

		int nHeight     = pDDSImageData->height;
		int nWidth      = pDDSImageData->width;
		int nNumMipMaps = pDDSImageData->numMipMaps;

		if ( nNumMipMaps > 1 )
		{
			glTexParameteri(table[f], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(table[f], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(table[f], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(table[f], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		int nBlockSize;

		if( pDDSImageData->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
			nBlockSize = 8;
		else
			nBlockSize = 16;

		int nSize;
		int nOffset = 0;

		// Load the mip-map levels
		for( int i = 0; i < nNumMipMaps; ++i )
		{
			if( nWidth  == 0 ) nWidth  = 1;
			if( nHeight == 0 ) nHeight = 1;

			nSize = ((nWidth+3)/4) * ((nHeight+3)/4) * nBlockSize;

			glCompressedTexImage2DARB( table[f],
				i,
				pDDSImageData->format,
				nWidth,
				nHeight,
				0,
				nSize,
				pDDSImageData->pixels + nOffset );

			nOffset += nSize;

			// Half the image size for the next mip-map level...
			nWidth  = (nWidth  / 2);
			nHeight = (nHeight / 2);
		}

		if( pDDSImageData->pixels != NULL )
		{
			free( pDDSImageData->pixels );
		}

		free( pDDSImageData );
	}

	return TextureID;
}

GLuint GutLoadNoCompressedTexture_OpenGL(const char *filename, sImageInfo *pInfo, bool mipmap_enabled)
{
	GLuint textureID;
	// 產生一個貼圖物件
	glGenTextures( 1, &textureID );
	// 使用g_TextureID貼圖物件
	glBindTexture( GL_TEXTURE_2D, textureID );

	char file_ext[16];
	_splitpath(filename, NULL, NULL, NULL, file_ext);
	strlwr(file_ext);

	GutImage *pLoader = NULL;

	if ( !strcmp(file_ext, ".tga") )
	{
		pLoader = new TGAImg;
	}
	else if ( !strcmp(file_ext, ".bmp") )
	{
		pLoader = new BMPImg;
	}
	else
	{
		glDeleteTextures(1, &textureID);
		return 0;
	}

	if ( !pLoader->Load(filename) )
	{
		return 0;
	}

	int w = pLoader->GetWidth();
	int h = pLoader->GetHeight();
	int bpp = pLoader->GetBPP();

	if ( pInfo )
	{
		pInfo->m_iWidth = w;
		pInfo->m_iHeight = h;
	}

	GLuint image_format = GL_RGBA;
	int elements = 4;

	switch(bpp)
	{
	case 8:
		image_format = GL_LUMINANCE;
		elements = 1;
		break;
	case 24:
		image_format = GL_RGB;
		elements = 3;
		break;
	case 32:
		image_format = GL_RGBA;
		elements = 4;
		break;
	}

	//gluBuild2DMipmaps(GL_TEXTURE_2D, elements, w, h, image_format, GL_UNSIGNED_BYTE, pLoader->GetImg());

	int p = w > h ? w : h;
	int mipmap_levels = 1;

	if ( mipmap_enabled )
	{
		int mask = 0x01;
		for ( int i=0; i<16; i++, mipmap_levels++, mask<<=1 )
		{
			if ( p & mask )
				break;
		}
	}

	if ( mipmap_levels > 1 )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	for ( int i=0; i<mipmap_levels; i++)
	{
		glTexImage2D( GL_TEXTURE_2D, i, elements, 
			pLoader->GetWidth(), pLoader->GetHeight(), 0,
			image_format, GL_UNSIGNED_BYTE, pLoader->GetImg() );
		if ( !pLoader->DownSampling() )
			break;
	}

	delete pLoader;

	return textureID;
}

GLuint GutLoadNoCompressedCubemapTexture_OpenGL(const char **filename_array, bool mipmap_enabled)
{
	GLuint textureID;

	// 產生一個貼圖物件
	glGenTextures( 1, &textureID );
	// 使用g_TextureID貼圖物件
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID );

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint table[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for ( int f=0; f<6; f++ )
	{
		const char *filename= filename_array[f];

		char file_ext[16];
		_splitpath(filename, NULL, NULL, NULL, file_ext);
		strlwr(file_ext);

		GutImage *pLoader = NULL;

		if ( !strcmp(file_ext, ".tga") )
		{
			pLoader = new TGAImg;
		}
		else if ( !strcmp(file_ext, ".bmp") )
		{
			pLoader = new BMPImg;
		}
		else
		{
			glDeleteTextures(1, &textureID);
			return 0;
		}

		if ( !pLoader->Load(filename) )
		{
			delete pLoader;
			return 0;
		}

		int w = pLoader->GetWidth();
		int h = pLoader->GetHeight();
		int bpp = pLoader->GetBPP();
		unsigned char *pImageBuffer = pLoader->GetImg();

		GLuint image_format = GL_RGBA;
		int elements = 4;

		switch(bpp)
		{
		case 8:
			image_format = GL_LUMINANCE;
			elements = 1;
			break;
		case 24:
			image_format = GL_RGB;
			elements = 3;
			break;
		case 32:
			image_format = GL_RGBA;
			elements = 4;
			break;
		}

		gluBuild2DMipmaps(table[f], elements, w, h, image_format, GL_UNSIGNED_BYTE, pImageBuffer);

		glTexParameteri(table[f], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(table[f], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	return textureID;
}

GLuint GutLoadTexture_OpenGL(const char *filename, sImageInfo *pInfo, bool mipmap_enabled = true)
{
	char file_ext[16];
	_splitpath(filename, NULL, NULL, NULL, file_ext);
	strlwr(file_ext);

	if ( !strcmp(file_ext, ".dds") )
	{
		return GutLoadDDSTexture_OpenGL(filename, pInfo);
	}
	else if ( !strcmp(file_ext, ".tga") )
	{
		return GutLoadNoCompressedTexture_OpenGL(filename, pInfo, mipmap_enabled);
	}
	else if ( !strcmp(file_ext, ".bmp") )
	{
		return GutLoadNoCompressedTexture_OpenGL(filename, pInfo, mipmap_enabled);
	}
	else
	{
		return 0;
	}
}

GLuint GutLoadCubemapTexture_OpenGL(const char *filename)
{
	char file_ext[16];
	_splitpath(filename, NULL, NULL, NULL, file_ext);
	strlwr(file_ext);

	if ( strcmp(file_ext, ".dds") )
	{
		return 0;
	}

	DDS_IMAGE_DATA *pDDSImageData = loadDDSTextureFile(filename);

	if ( pDDSImageData==NULL )
		return 0;

	GLuint TextureID = 0;

	int nHeight     = pDDSImageData->height;
	int nWidth      = pDDSImageData->width;
	int nNumMipMaps = pDDSImageData->numMipMaps;
	int nBlockSize = 16;
	int nElements = 0;

	GLint data_type = GL_UNSIGNED_BYTE;
	GLint data_fmt = GL_RGBA;
	DWORD format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

	switch(pDDSImageData->format)
	{
	case D3DFMT_DXT1:
		format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		nBlockSize = 8;
		break;
	case D3DFMT_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		nBlockSize = 16;
		break;
	case D3DFMT_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		nBlockSize = 16;
		break;
	case D3DFMT_R8G8B8:
		format = GL_RGB;
		nBlockSize = 0;
		nElements = 3;
		break;
	case D3DFMT_A8R8G8B8:
		format = GL_RGBA;
		data_fmt = GL_RGBA;
		nBlockSize = 0;
		nElements = 4;
		break;
	case D3DFMT_A32B32G32R32F:
		format = GL_RGBA32F_ARB;
		data_fmt = GL_RGBA;
		data_type = GL_FLOAT;
		nElements = 4;
		break;
	case D3DFMT_A16B16G16R16F:
		format = GL_RGBA16F_ARB;
		data_fmt = GL_RGBA;
		data_type = GL_FLOAT;
		nElements = 4;
		break;
	}

	glGenTextures( 1, &TextureID );

	glBindTexture( GL_TEXTURE_CUBE_MAP, TextureID );
	if ( nNumMipMaps > 1 )
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	}
	else
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}

	int nSize;
	int nOffset = 0;

	// Load the mip-map levels
	unsigned char *pBuffer = pDDSImageData->pixels;

	GLuint table[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for ( int f=0; f<6; f++ )
	{
		nHeight = pDDSImageData->height;
		nWidth = pDDSImageData->width;

		for( int i = 0; i < nNumMipMaps; ++i )
		{
			if( nWidth  == 0 ) nWidth  = 1;
			if( nHeight == 0 ) nHeight = 1;

			if ( nBlockSize )
			{
				nSize = ((nWidth+3)/4) * ((nHeight+3)/4) * nBlockSize;

				glCompressedTexImage2DARB( table[f], i, format, 
					nWidth, nHeight, 0, nSize, pBuffer);
			}
			else
			{
				nSize = nWidth * nHeight * pDDSImageData->PixelSize;

				glTexImage2D( table[f], i, format, 
					nWidth, nHeight, 0,
					data_fmt, data_type, pBuffer);
			}

			nOffset += nSize;
			pBuffer += nSize;
			// Half the image size for the next mip-map level...
			nWidth  = (nWidth  / 2);
			nHeight = (nHeight / 2);
		}
	}

	if( pDDSImageData->pixels != NULL )
		free( pDDSImageData->pixels );

	free( pDDSImageData );

	return TextureID;
}

GLuint GutLoadCubemapTexture_OpenGL(const char **filename_array, bool mipmap_enabled = true)
{
	const char *filename = filename_array[0];
	char file_ext[16];
	_splitpath(filename, NULL, NULL, NULL, file_ext);
	strlwr(file_ext);

	if ( !strcmp(file_ext, ".dds") )
	{
		return GutLoadCompressedCubemapTexture_OpenGL(filename_array);
	}
	else if ( !strcmp(file_ext, ".tga") )
	{
		return GutLoadNoCompressedCubemapTexture_OpenGL(filename_array, mipmap_enabled);
	}
	else if ( !strcmp(file_ext, ".bmp") )
	{
		return GutLoadNoCompressedCubemapTexture_OpenGL(filename_array, mipmap_enabled);
	}
	else
	{
		return 0;
	}
}

GLuint GutLoadVolumeTexture_OpenGL(const char *filename)
{
	DDS_IMAGE_DATA *pDDSImageData = loadDDSTextureFile(filename);

	if ( pDDSImageData==NULL )
		return NULL;

	GLuint textureID;

	// 產生一個貼圖物件
	glGenTextures( 1, &textureID );
	// 使用g_TextureID貼圖物件
	glBindTexture(GL_TEXTURE_3D, textureID );

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int nHeight     = pDDSImageData->height;
	int nWidth      = pDDSImageData->width;
	int nDepth		= pDDSImageData->depth;

	// 拷備貼圖
	glTexImage3D(GL_TEXTURE_3D, 0, 1, 
		nWidth, nHeight, nDepth, 0,
		GL_LUMINANCE, GL_UNSIGNED_BYTE, pDDSImageData->pixels );

	// free pDDSImageData
	if( pDDSImageData->pixels != NULL )
	{
		free( pDDSImageData->pixels );
	}
	free( pDDSImageData );

	return textureID;
}