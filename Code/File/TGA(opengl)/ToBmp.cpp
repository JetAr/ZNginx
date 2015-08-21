typedef boost::shared_ptr<Gdiplus::Image> ImagePtr;

ImagePtr CMainApp::OpenTgaImage(const CString & strImg)
{
    Texture texTga;
    if(!LoadTGA(&texTga, strImg))
        return ImagePtr();

    boost::shared_ptr<Gdiplus::Bitmap> bmp;
    Gdiplus::BitmapData bd;
    switch(texTga.bpp)
    {
    case 32:
        bmp.reset(new Gdiplus::Bitmap(texTga.width, texTga.height, PixelFormat32bppARGB));
        if(Gdiplus::Ok == bmp->LockBits(NULL, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bd))
        {
            for(UINT y = 0; y < texTga.height; y++)
            {
                for(UINT x = 0; x < texTga.width; x++)
                {
                    BYTE * pSrc = texTga.imageData + ((texTga.height - y - 1) * texTga.width + x) * 4;
                    BYTE * pDst = (BYTE *)bd.Scan0 + (y * bd.Stride + x * 4);
                    pDst[0] = pSrc[2];
                    pDst[1] = pSrc[1];
                    pDst[2] = pSrc[0];
                    pDst[3] = pSrc[3];
                }
            }
            bmp->UnlockBits(&bd);
        }
        break;

    case 24:
        bmp.reset(new Gdiplus::Bitmap(texTga.width, texTga.height, PixelFormat24bppRGB));
        if(Gdiplus::Ok == bmp->LockBits(NULL, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bd))
        {
            for(UINT y = 0; y < texTga.height; y++)
            {
                for(UINT x = 0; x < texTga.width; x++)
                {
                    BYTE * pSrc = texTga.imageData + ((texTga.height - y - 1) * texTga.width + x) * 3;
                    BYTE * pDst = (BYTE *)bd.Scan0 + (y * bd.Stride + x * 3);
                    pDst[0] = pSrc[2];
                    pDst[1] = pSrc[1];
                    pDst[2] = pSrc[0];
                }
            }
            bmp->UnlockBits(&bd);
        }
        break;

    case 16:
        ASSERT(false);
        break;
    }
    free(texTga.imageData);
    return bmp;
}