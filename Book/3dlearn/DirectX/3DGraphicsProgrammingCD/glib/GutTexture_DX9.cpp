#include <stdio.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <ddraw.h>

#include "Gut.h"
#include "GutDX9.h"
#include "GutImage.h"
#include "GutBMP.h"
#include "GutTGA.h"
#include "GutDDS.h"

LPDIRECT3DTEXTURE9 GutLoadTextureDDS_DX9(const char *filename, sImageInfo *info)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	DDS_IMAGE_DATA *pDDSImageData = loadDDSTextureFile(filename);

	if ( pDDSImageData==NULL )
	{
		return NULL;
	}

	LPDIRECT3DTEXTURE9 pTexture = NULL;

	int nHeight     = pDDSImageData->height;
	int nWidth      = pDDSImageData->width;
	int nNumMipMaps = pDDSImageData->numMipMaps;

	if ( info )
	{
		info->m_iWidth = nWidth;
		info->m_iHeight = nHeight;
	}

	device->CreateTexture(nWidth, nHeight, nNumMipMaps, 0, (D3DFORMAT)pDDSImageData->format, D3DPOOL_MANAGED, &pTexture, NULL);

	int nSize;
	int nOffset = 0;

	// DXn format
	if ( pDDSImageData->bBlock )
	{
		int nBlockSize;

		if( pDDSImageData->format == D3DFMT_DXT1 )
		{
			nBlockSize = 8;
		}
		else
		{
			nBlockSize = 16;
		}

		// 載入mipmap圖層
		for( int i = 0; i < nNumMipMaps; ++i )
		{
			if( nWidth  == 0 ) nWidth  = 1;
			if( nHeight == 0 ) nHeight = 1;

			nSize = ((nWidth+3)/4) * ((nHeight+3)/4) * nBlockSize;

			D3DLOCKED_RECT locked_rect;
			pTexture->LockRect(i, &locked_rect, NULL, 0);

			memcpy(locked_rect.pBits, pDDSImageData->pixels + nOffset, nSize);

			pTexture->UnlockRect(i);

			nOffset += nSize;
			// Half the image size for the next mip-map level...
			nWidth  = (nWidth  / 2);
			nHeight = (nHeight / 2);
		}
	}
	// DDS也可以儲存沒有壓縮的格式
	else
	{
		for( int i = 0; i < nNumMipMaps; ++i )
		{
			if( nWidth  == 0 ) nWidth  = 1;
			if( nHeight == 0 ) nHeight = 1;

			nSize = nWidth * nHeight * pDDSImageData->PixelSize;

			D3DLOCKED_RECT locked_rect;
			pTexture->LockRect(i, &locked_rect, NULL, 0);

			memcpy(locked_rect.pBits, pDDSImageData->pixels + nOffset, nSize);

			pTexture->UnlockRect(i);

			nOffset += nSize;
			// Half the image size for the next mip-map level...
			nWidth  = (nWidth  / 2);
			nHeight = (nHeight / 2);
		}
	}

	if( pDDSImageData->pixels != NULL )
	{
		free( pDDSImageData->pixels );
	}

	free( pDDSImageData );

	return pTexture;
}

LPDIRECT3DTEXTURE9 GutLoadNoCompressedTexture_DX9(const char *filename, sImageInfo *info, bool mipmap_enabled)
{
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
		return 0;
	}

	if ( IMG_OK!=pLoader->Load(filename) )
	{
		return NULL;
	}

	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	int bytes_per_pixel = pLoader->GetBPP()/8;
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	int w = pLoader->GetWidth();
	int h = pLoader->GetHeight();
	int p = w > h ? w : h;

	if ( w==0 || h==0 )
		return NULL;

	if ( info )
	{
		info->m_iWidth = w;
		info->m_iHeight = h;
	}

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

	// 產生貼圖物件
	if ( D3D_OK!=device->CreateTexture(pLoader->GetWidth(), pLoader->GetHeight(), 
		mipmap_levels, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL) )
		return false;

	for (int i=0; i<mipmap_levels; i++)
	{
		// 拷具貼圖資料
		D3DLOCKED_RECT locked_rect;
		pTexture->LockRect(i, &locked_rect, NULL, 0);

		unsigned char *target = (unsigned char *)locked_rect.pBits;
		unsigned char *source = pLoader->GetImg();

		for ( int y=0; y<pLoader->GetHeight(); y++ )
		{
			for ( int x=0; x<pLoader->GetWidth(); x++ )
			{
				switch(bytes_per_pixel)
				{
				case 1:
					target[0] = source[0];
					target[1] = source[0];
					target[2] = source[0];
					target[3] = source[0];
					source++;
					break;
				case 3:
					target[0] = source[2];
					target[1] = source[1];
					target[2] = source[0];
					target[3] = 255;
					source +=3;
					break;
				case 4:
					target[0] = source[2];
					target[1] = source[1];
					target[2] = source[0];
					target[3] = source[3];
					source +=4;
					break;
				}

				target+=4;
			}
		}

		pTexture->UnlockRect(i);

		if ( !pLoader->DownSampling() )
			break;
	}

	delete pLoader;

	return pTexture;
}

LPDIRECT3DCUBETEXTURE9 GutLoadCubemapTexture_DX9(const char *filename)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	LPDIRECT3DCUBETEXTURE9 pCubemapTexture = NULL;

	D3DXCreateCubeTextureFromFileEx(device, filename, 
		0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
		D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pCubemapTexture);

	return pCubemapTexture;
}

LPDIRECT3DCUBETEXTURE9 GutLoadCubemapTexture_DX9(const char **texture_array, bool mipmap_enabled)
{
	// 取得Direct3D9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	LPDIRECT3DCUBETEXTURE9 pCubemapTexture = NULL;

	D3DCUBEMAP_FACES face_table[] =
	{
		D3DCUBEMAP_FACE_POSITIVE_X,
		D3DCUBEMAP_FACE_NEGATIVE_X,
		D3DCUBEMAP_FACE_POSITIVE_Y,
		D3DCUBEMAP_FACE_NEGATIVE_Y,
		D3DCUBEMAP_FACE_POSITIVE_Z,
		D3DCUBEMAP_FACE_NEGATIVE_Z
	};

	// 要載入6張貼圖
	for ( int f=0; f<6; f++ )
	{
		const char *filename = texture_array[f];

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
			return 0;
		}

		// 載入貼圖
		if ( !pLoader->Load(filename) )
		{
			return 0;
		}

		int bytes_per_pixel = pLoader->GetBPP()/8;
		LPDIRECT3DTEXTURE9 pTexture = NULL;

		int w = pLoader->GetWidth();
		int h = pLoader->GetHeight();
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

		if ( pCubemapTexture==NULL )
		{
			device->CreateCubeTexture(w, mipmap_levels, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pCubemapTexture, NULL);
			if ( pCubemapTexture==NULL )
				return NULL;
		}

		for (int i=0; i<mipmap_levels; i++)
		{
			// 拷具貼圖資料
			D3DLOCKED_RECT locked_rect;
			pCubemapTexture->LockRect(face_table[f], i, &locked_rect, NULL, 0);

			unsigned char *target = (unsigned char *)locked_rect.pBits;
			unsigned char *source = pLoader->GetImg();

			for ( int y=0; y<pLoader->GetHeight(); y++ )
			{
				for ( int x=0; x<pLoader->GetWidth(); x++ )
				{
					switch(bytes_per_pixel)
					{
					case 1:
						target[0] = source[0];
						target[1] = source[0];
						target[2] = source[0];
						target[3] = source[0];
						source++;
						break;
					case 3:
						target[0] = source[2];
						target[1] = source[1];
						target[2] = source[0];
						target[3] = 255;
						source +=3;
						break;
					case 4:
						target[0] = source[2];
						target[1] = source[1];
						target[2] = source[0];
						target[3] = source[3];
						source +=4;
						break;
					}

					target+=4;
				}
			}

			pCubemapTexture->UnlockRect(face_table[f], i);

			if ( !pLoader->DownSampling() )
			{
				break;
			}
		}

		delete pLoader;
	}

	return pCubemapTexture;
}

LPDIRECT3DTEXTURE9 GutLoadTexture_DX9(const char *filename, sImageInfo *info, bool mipmap_enabled = true)
{
	char file_ext[16];
	_splitpath(filename, NULL, NULL, NULL, file_ext);
	strlwr(file_ext);

	if ( !strcmp(file_ext, ".dds") )
	{
		return GutLoadTextureDDS_DX9(filename, info);
	}
	else if ( !strcmp(file_ext, ".tga") )
	{
		return GutLoadNoCompressedTexture_DX9(filename, info, mipmap_enabled);
	}
	else if ( !strcmp(file_ext, ".bmp") )
	{
		return GutLoadNoCompressedTexture_DX9(filename, info, mipmap_enabled);
	}
	else
	{
		return NULL;
	}
}

LPDIRECT3DVOLUMETEXTURE9 GutLoadVolumeTexture_DX9(const char *filename)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	DDS_IMAGE_DATA *pDDSImageData = loadDDSTextureFile(filename);

	if ( pDDSImageData==NULL )
		return NULL;

	int nHeight     = pDDSImageData->height;
	int nWidth      = pDDSImageData->width;
	int nDepth		= pDDSImageData->depth;

	LPDIRECT3DVOLUMETEXTURE9 pTexture = NULL;
	device->CreateVolumeTexture(nWidth, nHeight, nDepth, 1, 0 , D3DFMT_L8, D3DPOOL_MANAGED, &pTexture, NULL);

	// 拷備貼圖
	D3DLOCKED_BOX locked_box;
	pTexture->LockBox(0, &locked_box, 0, 0);
	if ( locked_box.pBits )
	{
		memcpy(locked_box.pBits, pDDSImageData->pixels, pDDSImageData->BufferSize);
	}
	pTexture->UnlockBox(0);

	// free pDDSImageData
	if( pDDSImageData->pixels != NULL )
	{
		free( pDDSImageData->pixels );
	}
	free( pDDSImageData );

	return pTexture;
}

void GutDrawScreenQuad_DX9(float x0, float y0, float width, float height, sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 ident_mat; ident_mat.Identity();

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&ident_mat);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&ident_mat);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&ident_mat);

	float fTexelW = 0.0f;
	float fTexelH = 0.0f;

	if ( pInfo )
	{
		fTexelW = 1.0f/(float)pInfo->m_iWidth;
		fTexelH = 1.0f/(float)pInfo->m_iHeight;
	}

	Vertex_V3T2 quad[4];

	quad[0].m_Position[0] = x0;
	quad[0].m_Position[1] = y0;
	quad[0].m_Position[2] = 0.0f;
	quad[0].m_Texcoord[0] = 0.0f + fTexelW*0.5f;
	quad[0].m_Texcoord[1] = 1.0f + fTexelH*0.5f;

	quad[1].m_Position[0] = x0 + width;
	quad[1].m_Position[1] = y0;
	quad[1].m_Position[2] = 0.0f;
	quad[1].m_Texcoord[0] = 1.0f + fTexelW*0.5f;
	quad[1].m_Texcoord[1] = 1.0f + fTexelH*0.5f;

	quad[2].m_Position[0] = x0;
	quad[2].m_Position[1] = y0 + height;
	quad[2].m_Position[2] = 0.0f;
	quad[2].m_Texcoord[0] = 0.0f + fTexelW*0.5f;
	quad[2].m_Texcoord[1] = 0.0f + fTexelH*0.5f;

	quad[3].m_Position[0] = x0 + width;
	quad[3].m_Position[1] = y0 + height;
	quad[3].m_Position[2] = 0.0f;
	quad[3].m_Texcoord[0] = 1.0f + fTexelW*0.5f;
	quad[3].m_Texcoord[1] = 0.0f + fTexelH*0.5f;

	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex_V3T2));
}

void GutDrawFullScreenQuad_DX9(sImageInfo *pInfo)
{
	GutDrawScreenQuad_DX9(-1.0f, -1.0f, 2.0f, 2.0f, pInfo);
}
