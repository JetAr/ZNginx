#include "ElSamplePCH.h"
#include "GUITrueTypeFont.h"
#include "GUITexture.h"
#include "GUIRenderer.h"
#include "ELGUISystem.h"
#include "ElDeviceManager.h"
#include "ELGUIImagesetManager.h"

#define GLYPHS_PER_PAGE					1024
#define ElTrueTypeFontImageset			"%s_auto_glyph_imageset_%d"
#define ElTrueTypeFoneImage				"auto_glyph_image_%d"

using namespace ELGUI;

ElTrueTypeFont::ElTrueTypeFont(const std::string& face_name, const std::string& name, int size /* = 16 */, bool bold /* = false */, bool italic /* = false */, bool antialias /* = true */)
: d_face_name(face_name)
, d_name(name)
, d_num_imageset_used(0)
, d_current_imageset(0)
, d_current_texture(0)
, d_x_bottom(0)
, d_y_bottom(0)
, d_font(0)
, d_size(size)
, d_bold(bold)
, d_italic(italic)
, d_antialias(antialias)
{

}

ElTrueTypeFont::~ElTrueTypeFont()
{
	if (d_font)
		DeleteObject(d_font);
}

void ElTrueTypeFont::load()
{
	// create font
	d_font = CreateFont(-d_size, 0, 0, 0, (d_bold) ? FW_BOLD : FW_NORMAL,
		d_italic, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		(d_antialias) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, d_face_name.c_str());

	if (!d_font)
		return;

	// calculate font height
	HDC hDC = CreateCompatibleDC(0);
	SetMapMode(hDC, MM_TEXT);
	SetTextAlign(hDC, TA_TOP);
	SelectObject(hDC, d_font);

	TEXTMETRIC tTextMetrics;
	GetTextMetrics(hDC, &tTextMetrics);
	d_height = tTextMetrics.tmHeight;

	DeleteDC(hDC);

	_assignNewGlyphImageset();
}

void ElTrueTypeFont::rasterize(wchar_t codepoint)
{
	CodepointMap::const_iterator pos = d_cp_map.find(codepoint);
	if (pos != d_cp_map.end())
		return;

	int			i,j;
	int			nWidth, nHeight;
	float		advance;

	HDC			hBMDC;
	HBITMAP		hBM;
	BITMAPINFO	bmi;
	ABCFLOAT	abc;
	TEXTMETRIC	tTextMetrics;

	DWORD		*pPixels, *pTexData, dwPixel;

	// create and setup compatible DC
	hBMDC = CreateCompatibleDC(0);
	SetTextColor(hBMDC, RGB(255, 255, 255));
	SetBkColor(hBMDC, RGB(0, 0, 0));
	SetBkMode(hBMDC, TRANSPARENT);
	SetMapMode(hBMDC, MM_TEXT);
	SetTextAlign(hBMDC, TA_TOP);
	SelectObject(hBMDC, d_font);

	// calculate symbols metrics
	GetTextMetrics(hBMDC, &tTextMetrics);
	GetCharABCWidthsFloatW(hBMDC, codepoint, codepoint, &abc);

	// calculate symbols placement
	nWidth = abc.abcfB;
	nHeight = tTextMetrics.tmHeight;

	if (d_x_bottom + nWidth > GLYPHS_PER_PAGE)
	{
		d_x_bottom = 0;
		d_y_bottom += nHeight;
	}
	if (d_y_bottom + nHeight > GLYPHS_PER_PAGE)
	{
		_assignNewGlyphImageset();
	}

	// create DC bitmap
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biWidth = nWidth;
	bmi.bmiHeader.biHeight = -nHeight;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	hBM = CreateDIBSection(hBMDC, &bmi, DIB_RGB_COLORS, (void**)&pPixels, 0, 0);
	if (!hBM)
	{
		DeleteDC(hBMDC);
		return;
	}

	SelectObject(hBMDC, hBM);

	// draw symbols onto DC bitmap
	TextOutW(hBMDC, -abc.abcfA, 0, &codepoint, 1);
	GdiFlush();

	// transfer DC bitmap onto HGE texture with alpha channel
	D3DLOCKED_RECT TRect;
	INT TPitch;
	if (FAILED(d_current_texture->LockRect(0, &TRect, NULL, 0)))
	{
		assert(0 && "Can't lock texture");
		DeleteObject(hBM);
		DeleteDC(hBMDC);
		return;
	}
	pTexData = (DWORD*)TRect.pBits;
	pTexData += ((d_y_bottom * GLYPHS_PER_PAGE) + d_x_bottom);
	TPitch = TRect.Pitch >> 2;

	for (i = 0; i < nHeight; ++i)
	{
		for (j = 0; j < nWidth; ++j)
		{
			dwPixel = pPixels[i * nWidth + j];
			dwPixel = 0xFFFFFF | ((dwPixel & 0xFF) << 24);
			pTexData[i * TPitch + j] = dwPixel;
		}
	}
	
	d_current_texture->UnlockRect(0);

	char image_name[EL_MAX_PATH] = {0};
	sprintf(image_name, ElTrueTypeFoneImage, codepoint);
	d_current_imageset->defineImage(image_name, Point(d_x_bottom, d_y_bottom), Size(nWidth, nHeight), Point(0.0f, 0.0f));
	Image& image = d_current_imageset->getImage(image_name);
	d_cp_map[codepoint] = FontGlyph(&image, abc.abcfA, abc.abcfB, abc.abcfC);

	d_x_bottom += nWidth;

	// clean up
	DeleteObject(hBM);
	DeleteDC(hBMDC);
}

void ElTrueTypeFont::_assignNewGlyphImageset()
{
	// create a new imageset for text glyphs
	ElGUIRenderer* render = (ElGUIRenderer*)System::getSingleton().getRenderer();

	ElGUITexture* texture = (ElGUITexture*)render->createTexture();
	d_current_texture = 0;
	if (FAILED(D3DXCreateTexture(ElDeviceManager::getDevice(),
		GLYPHS_PER_PAGE, GLYPHS_PER_PAGE,
		1, 0, D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED, &d_current_texture)))
		return;
	texture->setTexture(d_current_texture);

	char imageset_name[EL_MAX_PATH] = {0};
	sprintf(imageset_name, ElTrueTypeFontImageset, d_name.c_str(), d_num_imageset_used++);
	d_current_imageset = ImagesetManager::getSingleton().createImageset(imageset_name, texture);

	d_x_bottom = 0;
	d_y_bottom = 0;
}