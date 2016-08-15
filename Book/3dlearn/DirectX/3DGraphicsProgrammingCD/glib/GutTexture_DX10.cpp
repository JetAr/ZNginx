#ifdef _ENABLE_DX10_

#include <stdio.h>

#include <ddraw.h>
#include <d3d9types.h>

#include <d3d10.h>
#include <d3dx10.h>

#include "Gut.h"
#include "GutDX10.h"
#include "GutImage.h"
#include "GutBMP.h"
#include "GutTGA.h"
#include "GutDDS.h"

ID3D10ShaderResourceView *GutLoadDDSTexture_DX10(const char *filename, sImageInfo *pInfo)
{
	// 取得Direct3D10裝置
	ID3D10Device *device = GutGetGraphicsDeviceDX10();
	DDS_IMAGE_DATA *pDDSImageData = loadDDSTextureFile(filename);

	if ( pDDSImageData==NULL )
		return NULL;

	// 只示范DXn模式, 其它模式交給Direct3D10函式來載入.
	if ( !pDDSImageData->bBlock )
	{
		//
		if( pDDSImageData->pixels != NULL )
		{
			free( pDDSImageData->pixels );
		}
		free( pDDSImageData );

		ID3D10ShaderResourceView *pView = NULL;
		D3DX10CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &pView, NULL);

		return pView;
	}


	int nBlockSize = 8;
	DXGI_FORMAT format = DXGI_FORMAT_BC1_UNORM;

	switch(pDDSImageData->format)
	{
	case D3DFMT_DXT1:
	case D3DFMT_DXT2:
		nBlockSize = 8;
		format = DXGI_FORMAT_BC1_UNORM;
		break;
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
		nBlockSize = 16;
		format = DXGI_FORMAT_BC2_UNORM;
		break;
	case D3DFMT_DXT5:
		nBlockSize = 16;
		format = DXGI_FORMAT_BC3_UNORM;
		break;
	}

	ID3D10Texture2D *pTexture = NULL;

	int nHeight     = pDDSImageData->height;
	int nWidth      = pDDSImageData->width;
	int nNumMipMaps = pDDSImageData->numMipMaps;

	if ( pInfo )
	{
		pInfo->m_iWidth = nWidth;
		pInfo->m_iHeight = nHeight;
	}

	int nSize;
	int nOffset = 0;

	unsigned char *layers[20];

	D3D10_SUBRESOURCE_DATA init_data[20];
	ZeroMemory(init_data, sizeof(init_data));

	// 載入mipmap
	for( int i = 0; i < nNumMipMaps; ++i )
	{
		if( nWidth  == 0 ) nWidth  = 1;
		if( nHeight == 0 ) nHeight = 1;
		int nBlocks_Per_Row = (nWidth+3)/4;

		nSize = ((nWidth+3)/4) * ((nHeight+3)/4) * nBlockSize;
		init_data[i].pSysMem = pDDSImageData->pixels + nOffset;
		init_data[i].SysMemPitch = nBlocks_Per_Row * nBlockSize;

		nOffset += nSize;

		// mipmap下一層的解析度會減半
		nWidth  = (nWidth  / 2);
		nHeight = (nHeight / 2);
	}

	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.Width =  pDDSImageData->width;
	desc.Height =  pDDSImageData->height;
	desc.MipLevels = nNumMipMaps;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Format = format;
	desc.Usage = D3D10_USAGE_IMMUTABLE;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	device->CreateTexture2D( &desc, init_data, &pTexture );

	D3D10_SHADER_RESOURCE_VIEW_DESC view_desc;
	ZeroMemory( &view_desc, sizeof(view_desc));

	view_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	view_desc.Format = format;
	view_desc.Texture2D.MostDetailedMip = 0;
	view_desc.Texture2D.MipLevels = pDDSImageData->numMipMaps;

	ID3D10ShaderResourceView *pView = NULL;

	device->CreateShaderResourceView(pTexture, &view_desc, &pView);

	if( pDDSImageData->pixels != NULL )
	{
		free( pDDSImageData->pixels );
	}

	free( pDDSImageData );

	return pView;
}

ID3D10ShaderResourceView *GutLoadNoCompressedTexture_DX10(const char *filename, sImageInfo *pInfo, bool mipmap_enabled)
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

	if ( !pLoader->Load(filename) )
	{
		return NULL;
	}

	// 取得Direct3D 10裝置
	ID3D10Device *device = GutGetGraphicsDeviceDX10();

	int bytes_per_pixel = pLoader->GetBPP()/8;
	int w = pLoader->GetWidth();
	int h = pLoader->GetHeight();
	int p = w > h ? w : h;

	if ( pInfo )
	{
		pInfo->m_iWidth = w;
		pInfo->m_iHeight = h;
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

	unsigned char *layers[20];
	int layers_w[20];
	int layers_h[20];

	for (int i=0; i<mipmap_levels; i++)
	{
		// 把mipmap每個圖層的貼圖資料復制,并轉換到ABGR排列的記憶體內
		layers_w[i] = pLoader->GetWidth();
		layers_h[i] = pLoader->GetHeight();

		layers[i] = new unsigned char[pLoader->GetWidth() * pLoader->GetHeight() * 4];
		unsigned char *target = layers[i];
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
					target[0] = source[0];
					target[1] = source[1];
					target[2] = source[2];
					target[3] = 255;
					source +=3;
					break;
				case 4:
					target[0] = source[0];
					target[1] = source[1];
					target[2] = source[2];
					target[3] = source[3];
					source +=4;
					break;
				}

				target+=4;
			}
		}

		if ( !pLoader->DownSampling() )
			break;
	}

	delete pLoader;

	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.Width = w;
	desc.Height = h;
	desc.MipLevels = mipmap_levels;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D10_USAGE_IMMUTABLE;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	// 透過設定初值的方法來拷備貼圖資料
	D3D10_SUBRESOURCE_DATA init_data[20];

	for ( int i=0; i<mipmap_levels; i++ )
	{
		// 放入每個圖層的資料
		init_data[i].pSysMem = layers[i];
		init_data[i].SysMemPitch = layers_w[i] * 4;
		init_data[i].SysMemSlicePitch = 0;
	}

	ID3D10Texture2D *pTexture = NULL;
	device->CreateTexture2D( &desc, init_data, &pTexture );

	for ( int i=0; i<mipmap_levels; i++ )
	{
		delete [] layers[i];
	}

	D3D10_SHADER_RESOURCE_VIEW_DESC view_desc;
	ZeroMemory(&view_desc, sizeof(view_desc));

	view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	view_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	view_desc.Texture2D.MostDetailedMip = 0;
	view_desc.Texture2D.MipLevels = mipmap_levels;

	ID3D10ShaderResourceView *pView = NULL;

	device->CreateShaderResourceView(pTexture, &view_desc, &pView);

	return pView;
}

ID3D10ShaderResourceView *GutLoadCubemapTexture_DX10(const char **texture_array, bool mipmap_enabled)
{
	struct sMipmapInfo
	{
		unsigned char *pBuffer;
		int w,h;
	};

	sMipmapInfo mipmap_info[6][20];
	ZeroMemory(&mipmap_info[0][0], sizeof(mipmap_info));

	int mipmap_levels = 1;

	// 取得Direct3D10裝置
	ID3D10Device *device = GutGetGraphicsDeviceDX10();

	D3D10_SUBRESOURCE_DATA init_data[6*15];
	int index = 0;

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

		if ( !pLoader->Load(filename) )
		{
			return NULL;
		}

		// 取得Direct3D10裝置
		ID3D10Device *device = GutGetGraphicsDeviceDX10();

		int bytes_per_pixel = pLoader->GetBPP()/8;
		int w = pLoader->GetWidth();
		int h = pLoader->GetHeight();
		int p = w > h ? w : h;
		mipmap_levels = 1;

		if ( mipmap_enabled )
		{
			int mask = 0x01;
			for ( int i=0; i<16; i++, mipmap_levels++, mask<<=1 )
			{
				if ( p & mask )
					break;
			}
		}

		for (int i=0; i<mipmap_levels; i++)
		{
			// 把mipmap每個圖層的貼圖資料復制,并轉換到ABGR排列的記憶體內
			mipmap_info[f][i].w = pLoader->GetWidth();
			mipmap_info[f][i].h = pLoader->GetHeight();
			mipmap_info[f][i].pBuffer = new unsigned char[pLoader->GetWidth() * pLoader->GetHeight() * 4];

			unsigned char *target = mipmap_info[f][i].pBuffer;
			unsigned char *source = pLoader->GetImg();

			// 放入每個圖層的資料
			init_data[index].pSysMem = mipmap_info[f][i].pBuffer;
			init_data[index].SysMemPitch = mipmap_info[f][i].w * 4;
			init_data[index].SysMemSlicePitch = 0;
			index++;

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
						target[0] = source[0];
						target[1] = source[1];
						target[2] = source[2];
						target[3] = 255;
						source +=3;
						break;
					case 4:
						target[0] = source[0];
						target[1] = source[1];
						target[2] = source[2];
						target[3] = source[3];
						source +=4;
						break;
					}

					target+=4;
				}
			}

			if ( !pLoader->DownSampling() )
				break;
		}

		delete pLoader;
	}

	// 設定貼圖格式
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.Width = mipmap_info[0][0].w;
	desc.Height = mipmap_info[0][0].h;
	desc.MipLevels = mipmap_levels;
	desc.ArraySize = 6;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D10_USAGE_IMMUTABLE;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

	// 透過設定初值的方法來拷備貼圖資料
	ID3D10Texture2D *pTexture = NULL;
	device->CreateTexture2D( &desc, init_data, &pTexture );

	for ( int f=0; f<6; f++ )
	{
		for ( int i=0; i<mipmap_levels; i++ )
		{
			delete [] mipmap_info[f][i].pBuffer;
		}
	}

	// 配置給Shader使用的資源型態
	ID3D10ShaderResourceView *pEnvMapSRV;

	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory( &SRVDesc, sizeof(SRVDesc) );

	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
	SRVDesc.TextureCube.MipLevels = mipmap_levels;
	SRVDesc.TextureCube.MostDetailedMip = 0;

	if ( S_OK != device->CreateShaderResourceView(pTexture, &SRVDesc, &pEnvMapSRV) )
		return NULL;

	return pEnvMapSRV;
}

ID3D10ShaderResourceView *GutLoadTexture_DX10(const char *filename, sImageInfo *pInfo, bool mipmap_enabled = true)
{
	char file_ext[16];
	_splitpath(filename, NULL, NULL, NULL, file_ext);
	strlwr(file_ext);

	if ( !strcmp(file_ext, ".dds") )
	{
		return GutLoadDDSTexture_DX10(filename, pInfo);
	}
	else if ( !strcmp(file_ext, ".tga") )
	{
		return GutLoadNoCompressedTexture_DX10(filename, pInfo, mipmap_enabled);
	}
	else if ( !strcmp(file_ext, ".bmp") )
	{
		return GutLoadNoCompressedTexture_DX10(filename, pInfo, mipmap_enabled);
	}
	else
	{
		return NULL;
	}
}

ID3D10ShaderResourceView *GutLoadCubemapTexture_DX10(const char *filename)
{
	// 取得Direct3D 10裝置
	ID3D10Device *device = GutGetGraphicsDeviceDX10();

	char file_ext[16];
	_splitpath(filename, NULL, NULL, NULL, file_ext);
	strlwr(file_ext);

	ID3D10ShaderResourceView *pTextureView = NULL;

	if ( !strcmp(file_ext, ".dds") )
	{
		D3DX10CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &pTextureView, NULL);
	}

	return pTextureView;
}

ID3D10ShaderResourceView *GutLoadVolumeTexture_DX10(const char *filename)
{
	// 取得Direct3D 10裝置
	ID3D10Device *device = GutGetGraphicsDeviceDX10();

	DDS_IMAGE_DATA *pDDSImageData = loadDDSTextureFile(filename);

	if ( pDDSImageData==NULL )
		return NULL;

	D3D10_TEXTURE3D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.Width = pDDSImageData->width;
	desc.Height = pDDSImageData->height;
	desc.Depth = pDDSImageData->depth;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.Usage = D3D10_USAGE_IMMUTABLE;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	// 透過設定初值的方法來拷備貼圖資料
	D3D10_SUBRESOURCE_DATA init_data;
	init_data.pSysMem = pDDSImageData->pixels;
	init_data.SysMemPitch = desc.Width;
	init_data.SysMemSlicePitch = desc.Width * desc.Height;

	ID3D10Texture3D *pTexture = NULL;
	device->CreateTexture3D( &desc, &init_data, &pTexture );

	D3D10_SHADER_RESOURCE_VIEW_DESC view_desc;
	ZeroMemory(&view_desc, sizeof(view_desc));

	view_desc.Format = DXGI_FORMAT_R8_UNORM;
	view_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE3D;
	view_desc.Texture3D.MostDetailedMip = 0;
	view_desc.Texture3D.MipLevels = 1;

	ID3D10ShaderResourceView *pView = NULL;
	device->CreateShaderResourceView(pTexture, &view_desc, &pView);

	return pView;
}

#endif // _ENABLE_DX10_
