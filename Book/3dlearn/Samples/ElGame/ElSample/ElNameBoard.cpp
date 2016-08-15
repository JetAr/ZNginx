#include "ElSamplePCH.h"
#include "ElNameBoard.h"
#include "ElDeviceManager.h"

ElNameboard::ElNameboard()
: mTexture(0)
{

}

ElNameboard::~ElNameboard()
{
	ElSafeRelease(mTexture);
}

bool ElNameboard::create(std::wstring text, std::string facename, int fontsize, unsigned int fontcolour /* = 0xFFFFFF */)
{
	int i,j;

	HDC			hBMDC;
	HBITMAP		hBM;
	BITMAPINFO	bmi;
	HFONT		hFont;
	ABCFLOAT	abc;
	TEXTMETRIC	tTextMetrics;

	DWORD		*pPixels, *pTexData, dwPixel;

	// create font
	hFont = CreateFont(-fontsize, 0, 0, 0, FW_NORMAL,
		false, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, facename.c_str());

	if (!hFont)
		return false;

	// create and setup compatible DC
	hBMDC = CreateCompatibleDC(0);
	SetTextColor(hBMDC, RGB(255, 255, 255));
	SetBkColor(hBMDC, RGB(0, 0, 0));
	SetBkMode(hBMDC, TRANSPARENT);
	SetMapMode(hBMDC, MM_TEXT);
	SetTextAlign(hBMDC, TA_LEFT | TA_TOP | TA_NOUPDATECP);
	SelectObject(hBMDC, hFont);

	// calculate symbols metrics
	RECT rt;
	rt.left = rt.top = rt.right = rt.bottom = 0;
	DrawTextW(hBMDC, text.c_str(), text.length(), &rt, DT_CENTER | DT_CALCRECT);
	
	mWidth = rt.right - rt.left;
	mHeight = rt.bottom - rt.top;

	if (!_createTexture())
	{
		DeleteObject(hFont);
		DeleteDC(hBMDC);
		return false;
	}

	// create DC bitmap
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biWidth = mWidth;
	bmi.bmiHeader.biHeight = -mHeight;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	hBM = CreateDIBSection(hBMDC, &bmi, DIB_RGB_COLORS, (void**)&pPixels, 0, 0);
	if (!hBM)
	{
		DeleteObject(hFont);
		DeleteDC(hBMDC);
		return false;
	}

	SelectObject(hBMDC, hBM);

	// draw symbols onto DC bitmap
	DrawTextW(hBMDC, text.c_str(), text.length(), &rt, DT_CENTER);
	GdiFlush();

	// transfer DC bitmap onto HGE texture with alpha channel
	D3DLOCKED_RECT TRect;
	INT TPitch;
	if (FAILED(mTexture->LockRect(0, &TRect, NULL, 0)))
	{
		assert(0 && "Can't lock texture");
		DeleteObject(hBM);
		DeleteObject(hFont);
		DeleteDC(hBMDC);
		return false;
	}
	pTexData = (DWORD*)TRect.pBits;
	TPitch = TRect.Pitch >> 2;

	for (i = 0; i < mHeight; ++i)
	{
		for (j = 0; j < mWidth; ++j)
		{
			dwPixel = pPixels[i * mWidth + j];
			dwPixel = fontcolour | ((dwPixel & 0xFF) << 24);
			pTexData[i * TPitch + j] = dwPixel;
		}
	}

	mTexture->UnlockRect(0);

	// clean up
	DeleteObject(hBM);
	DeleteObject(hFont);
	DeleteDC(hBMDC);

	return true;
}

unsigned int ElNameboard::getWidth() const
{
	return mWidth;
}

unsigned int ElNameboard::getHeight() const
{
	return mHeight;
}

IDirect3DTexture9* ElNameboard::getTexture() const
{
	return mTexture;
}

bool ElNameboard::_createTexture()
{
	if (FAILED(D3DXCreateTexture(ElDeviceManager::getDevice(),
		mWidth, mHeight,
		1, 0, D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED, &mTexture)))
		return false;

	return true;
}