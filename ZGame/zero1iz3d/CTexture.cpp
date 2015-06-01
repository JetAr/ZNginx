// CTexture.cpp -


#include "main.h"



/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: CTexture()
//// Desc: class constructor
/////////////////////////////////////////////////////////////////////////////////////////////////
CTexture::CTexture()
{
	m_textureloaded = FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: ~CTexture()
//// Desc: class destructor
/////////////////////////////////////////////////////////////////////////////////////////////////
CTexture::~CTexture()
{
  //Free();
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Free()
//// Desc: osvobodi pametta
/////////////////////////////////////////////////////////////////////////////////////////////////
void CTexture::Free()
{
	glDeleteTextures( 1, &m_textureID );				// iztrii indexa na texturata ot steck-a
	m_textureloaded = FALSE;

}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Activate()
//// Desc: activirai texturata za rendirane
/////////////////////////////////////////////////////////////////////////////////////////////////
int CTexture::Activate()
{
	if ( !m_textureloaded ) return FALSE;

	glBindTexture( GL_TEXTURE_2D, m_textureID );

 return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: LoadTextureFromTGA()
//// Desc: opredeli kompresiqta na TGA texturata i q zaredi
/////////////////////////////////////////////////////////////////////////////////////////////////
int CTexture::LoadTextureFromTGA( const char *filename )
{
	FILE *fp;
	BYTE uTGA_header[12] = {0,0,2,0,0,0,0,0,0,0,0,0};				// nekompresirani TGA header
	BYTE cTGA_header[12] = {0,0,10,0,0,0,0,0,0,0,0,0};				// kompresirani TGA header
	BYTE szHeader[12];

	memset( szHeader, NULL, sizeof(szHeader) );

	if ( (fp=fopen( filename, "rb" )) == NULL )
	{
		// appendtolog
		return FALSE;
	}

	// read header
	if ( fread( szHeader, sizeof(szHeader), 1, fp ) == 0 ) 
	{
		// appendtolog 
		return FALSE;
	}
		
	// compare headers and determine compression
	if ( memcmp( uTGA_header, szHeader, sizeof(szHeader) ) == 0 )
	{
	  return LoadTGAUncompressed( filename, fp );
	}
	else if ( memcmp( cTGA_header, szHeader, sizeof(szHeader) ) == 0 )
	{
	  return LoadTGACompressed( filename, fp );
	}
	

	// appendtolog - invalid file
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: LoadTGAUncompressed()
//// Desc: Zarejda nekompresirana textura
/////////////////////////////////////////////////////////////////////////////////////////////////
int CTexture::LoadTGAUncompressed( const char *filename, FILE *fpTGA )
{

 struct_tga tga;
 struct_texture tex;

 // read 6 chars headers
 if ( fread( &tga.header, sizeof(tga.header), 1, fpTGA ) == 0 )
 {
	 // appendtolog
	 return FALSE;
 }

 // calculate TGA image properites
 tex.width = tga.header[1] * 256 + tga.header[0];
 tex.height = tga.header[3] * 256 + tga.header[2];
 tex.bpp = tga.header[4];
 tga.width = tex.width;
 tga.height = tex.height;
 tga.bpp = tex.bpp;

 // check for valid info
 if ( (tex.width <= 0) || (tex.height <= 0) || (tex.bpp != 24 && tex.bpp != 32) )
 {
	 // appendtolog - invalid image properites
	 return FALSE;
 }

 // set image type
 if ( tex.bpp == 24 )
 {
	 tex.type = GL_RGB;
 }
 else 
 {
	 tex.type = GL_RGBA;
 }

 // caluclate bytes per pixel and total image data
 tga.bytesPerPixel = tga.bpp / 8;
 tga.imgsize = (tga.bytesPerPixel * tga.width * tga.height);

 // rezervirai memory for the image data
 //tex.imgdata = (BYTE *)malloc(tga.imgsize);
 try {
 	 tex.imgdata = new BYTE[tga.imgsize];
 }
 catch( std::bad_alloc ) {

	 // appendtolog - notenough memory to load texture
	 return FALSE;
 }


 // read image data
 if ( fread( tex.imgdata, 1, tga.imgsize, fpTGA ) != tga.imgsize ) return FALSE;
 

 // razmeni TGA BGR format s GL readable RGB format
 for ( UINT i = 0; i < (UINT)tga.imgsize; i += tga.bytesPerPixel )
 {
	 // shte izpolzwame XOR za da ramenim componentite ( 10x2 Steve Thomas )
	 tex.imgdata[i] ^= tex.imgdata[i+2] ^= tex.imgdata[i] ^= tex.imgdata[i+2];
 }

 fclose( fpTGA );

 // generirai textura sys informaciqta ot TGA-to
 GenerateTexture( &tex );

 // osvobodi pametta na image-to
 delete[] tex.imgdata; 

 return m_textureloaded = TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: LoadTGACompressed()
//// Desc: zarejda kompresirana (RLE) textura
/////////////////////////////////////////////////////////////////////////////////////////////////
int CTexture::LoadTGACompressed( const char *filename, FILE *fpTGA  )
{

 struct_tga		 tga;
 struct_texture  tex;
 UINT			 pixelcount;							// wsichko pixeli w image-to
 UINT			 curpixel;								// pixel ot koito chetem w momenta
 UINT			 curbyte;								// byte koito shte zapiswame w image-data
 BYTE			 *clrbuf;								// color buffer

 // read 6 chars headers
 if ( fread( &tga.header, sizeof(tga.header), 1, fpTGA ) == 0 )
 {
	 // appendtolog
	 return FALSE;
 }

 // calculate TGA image properites
 tex.width  = tga.header[1] * 256 + tga.header[0];
 tex.height = tga.header[3] * 256 + tga.header[2];
 tex.bpp	= tga.header[4];
 tga.width  = tex.width;
 tga.height = tex.height;
 tga.bpp    = tex.bpp;

 // check for valid info
 if ( (tex.width <= 0) || (tex.height <= 0) || (tex.bpp != 24 && tex.bpp != 32) )
 {
	 // appendtolog - invalid image properites
	 return FALSE;
 }

 // set image type
 if ( tex.bpp == 24 )
 {
	 tex.type = GL_RGB;
 }
 else 
 {
	 tex.type = GL_RGBA;
 }

 // caluclate bytes per pixel and total image data
 tga.bytesPerPixel = tga.bpp / 8;
 tga.imgsize	   = (tga.bytesPerPixel * tga.width * tga.height);
 
 tex.imgdata = new BYTE[tga.imgsize];
 try {
 	 tex.imgdata = new BYTE[tga.imgsize];
 }
 catch( std::bad_alloc ) {

	 // appendtolog - notenough memory to load texture
	 return FALSE;
 }

 pixelcount = curpixel = curbyte = 0;
 pixelcount = tga.width * tga.height;
 clrbuf = new BYTE[tga.bytesPerPixel];

 do
 {
	 BYTE chunkheader = 0;
	 fread( &chunkheader, sizeof(BYTE), 1, fpTGA );

	 // proveri za RAW header, ako e takyw, togawa trqbwa da +1 za da namerim
	 // obshtiq broi na pixelite sled header-a
	 if ( chunkheader < 128 )
	 {
		 chunkheader++;
		 // procheti pixelite v colorbufer-a
		 for ( short i = 0; i < chunkheader; i++ )
		 {
			 fread( clrbuf, 1, tga.bytesPerPixel, fpTGA );
		 
			// sega shte vzemem pixelite ot colorbufera i shte gi zapishem w image-data kato
			// ednovremenno razmenqme formata BGR(A) s RGB(A) 
			tex.imgdata[curbyte]     = clrbuf[2];     // write R
			tex.imgdata[curbyte + 1] = clrbuf[1];     // write G
			tex.imgdata[curbyte + 2] = clrbuf[0];     // write B
			if ( tga.bytesPerPixel == 4 ) 
				tex.imgdata[curbyte + 3] = clrbuf[3]; // write A
		 
			curbyte += tga.bytesPerPixel;			   // sledwashtite 3/4 pixela
			curpixel++;							   // uwelichi broqcha na pixelite
		 }
	 }
	 else
	 {
		 // RLE header, headeryt ima stoinost = 127 taka che 
		 // shte izvadim 127 za da namerim broq na pixelite
		 chunkheader -= 127;
		 fread( clrbuf, 1, tga.bytesPerPixel, fpTGA );

		 // sledwashtiqt cikyl shte zapechatame w pametta tolkova pyti
		 // kolkoto e razultata ot header-a
		 for ( short j = 0; j < chunkheader; j++ )
		 {
			tex.imgdata[curbyte]     = clrbuf[2];     // write R
			tex.imgdata[curbyte + 1] = clrbuf[1];     // write G
			tex.imgdata[curbyte + 2] = clrbuf[0];     // write B
			if ( tga.bytesPerPixel == 4 ) 
				tex.imgdata[curbyte + 3] = clrbuf[3]; // write A
		 
			 curbyte += tga.bytesPerPixel;			   // sledwashtite 3/4 pixela
			 curpixel++;							   // uwelichi broqcha na pixelite
		 }
	 }
 } while ( curpixel < pixelcount );

 fclose( fpTGA );

 // generirai textura sys informaciqta ot TGA-to
 GenerateTexture( &tex );

 // osvobodi pametta na image-to
 delete[] tex.imgdata; 

 return m_textureloaded = TRUE;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: LoadTextureFromBitmap()
//// Desc: Zarejda textura ot bitmap chrez AUX funkciq
/////////////////////////////////////////////////////////////////////////////////////////////////
int CTexture::LoadTextureFromBitmap( const char *filename )
{

 AUX_RGBImageRec *buffer_image = NULL;
 struct_texture  tex;
 FILE *fp = NULL;
 
 // proveri parametyra (hah!)
 if ( !filename ) return FALSE;
 
 // proveri dali file-a syshtestvuva i go zaredi ako e taka!
 if ( (fp = fopen( filename, "r" ) ))
 {
   fclose(fp);
   if ( (buffer_image = auxDIBImageLoad(filename)) == NULL )
	 return FALSE;

   // setupni localnata stuctura
   tex.width = buffer_image->sizeX;
   tex.height = buffer_image->sizeY;
   tex.type = GL_RGB;
   tex.imgdata = buffer_image->data;
   tex.bpp = 24;
   GenerateTexture( &tex );
   	
   // osvobodi zaetata pamet
   if ( buffer_image )
   {
     if ( buffer_image->data ) free(buffer_image->data);
     free(buffer_image);
   }
 
   return m_textureloaded = TRUE;
 }
 
 return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: GenerateTexture()
//// Desc: generira textura
/////////////////////////////////////////////////////////////////////////////////////////////////
void CTexture::GenerateTexture( struct_texture *texture )
{

   // kolko texturi da se generirat
   glGenTextures( 1, &m_textureID );
   glBindTexture( GL_TEXTURE_2D, m_textureID );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

   // postroi bez mipmaps
//   glTexImage2D( GL_TEXTURE_2D, 0, texture->bpp == 24 ? 3 : 4 , texture->width, texture->height, 0, texture->type, 
//				   GL_UNSIGNED_BYTE, texture->imgdata );
   
   // postroi s mipmaps
   gluBuild2DMipmaps( GL_TEXTURE_2D, texture->bpp == 24 ? 3 : 4, \
					  texture->width, texture->height, \
					  texture->type, GL_UNSIGNED_BYTE, \
					  texture->imgdata );

}
