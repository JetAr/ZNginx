
#define IMG_OK              0x1
#define IMG_ERR_NO_FILE     0x2
#define IMG_ERR_MEM_FAIL    0x4
#define IMG_ERR_BAD_FORMAT  0x8
#define IMG_ERR_UNSUPPORTED 0x40

#include "GutImage.h"

class TGAImg : public GutImage
{
public:
	TGAImg();
	~TGAImg();
	int Load(const char* szFilename);
	int GetBPP();
	int GetWidth();
	int GetHeight();
	unsigned char* GetImg(void);       // Return a pointer to image data
	unsigned char* GetPalette(void);   // Return a pointer to VGA palette
	void FlipImg();
	bool DownSampling(void);

private:
	short int iWidth,iHeight,iBPP;
	unsigned long lImageSize;
	char bEnc;
	unsigned char *pImage, *pPalette, *pData;

	// Internal workers
	int ReadHeader();
	int LoadRawData();
	int LoadTgaRLEData();
	int LoadTgaPalette();
	void BGRtoRGB();
	unsigned char *GetPixel(int x, int y);
};
