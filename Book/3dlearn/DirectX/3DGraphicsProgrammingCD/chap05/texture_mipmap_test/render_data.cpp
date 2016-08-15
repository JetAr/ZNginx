#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// 矩形的4個頂點
// Position, Texcoord
Vertex_VT g_Quad[4] =
{
	{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
	{{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
	{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}},
	{{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}}
};

int g_iFilterMode = 3;
int g_iWidth = 512;
int g_iHeight = 512;

// controller object
CGutUserControl g_Control;

unsigned char *g_pImages[10];

bool CreateMipmapImages(void)
{
	int w = g_iWidth;
	int h = g_iHeight;
	int mipmap_level = 0;

	unsigned char color_table[][4] =
	{
		{255,  0,  0,255},
		{  0,255,  0,255},
		{255,255,  0,255},
		{  0,  0,255,255},
		{255,  0,255,255},
		{  0,255,255,255},
		{128,  0,  0,255},
		{  0,128,  0,255},
		{128,128,  0,255},
		{255,255,255,255}
	};

	while(true)
	{
		g_pImages[mipmap_level] = new unsigned char[w*h*4];
		if ( g_pImages[mipmap_level]==NULL )
			return false;

		unsigned char *pDest = g_pImages[mipmap_level];
		unsigned char *pSource = &color_table[mipmap_level][0];

		int pixels = w*h;
		for (int p=0; p<pixels; p++, pDest+=4 )
		{
			pDest[0] = pSource[0];
			pDest[1] = pSource[1];
			pDest[2] = pSource[2];
			pDest[3] = pSource[3];
		}

		if (w==1 && h==1)
			break;

		if ( w>1 ) w/=2;
		if ( h>1 ) h/=2;

		mipmap_level++;
	}

	return true;
}

void ReleaseMipmapImages(void)
{
	for ( int i=0; i<10; i++ )
	{
		if ( g_pImages[i] )
		{
			delete [] g_pImages[i];
			g_pImages[i] = NULL;
		}
	}
}