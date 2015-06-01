// CTexture.h - 


#ifndef __CTEXTURE_INCLUDED
#define __CTEXTURE_INCLUDED


// Texture Class
class CTexture {

private:
	
	typedef struct {
		BYTE  header[6];
		UINT  bytesPerPixel;// byte-ve za pixel ( 3 ili 4 )
		UINT  imgsize;		// pamet neobhodima za image-to
		UINT  type;			// tip RGB ili RGBA
		UINT  width;
		UINT  height;
		UINT  bpp;			// bitsperpixel 24 ili 32
	} struct_tga;

	
    typedef struct struct_texture {
		BYTE  *imgdata;
		UINT  bpp;
		UINT  width;
		UINT  height;
		UINT  type;			    // image data GL_RGB ili GL_RGBA
	};
	
	GLuint m_textureID;			// OpenGL texture ID
	bool   m_textureloaded;		    // zaredena li e texturata

	int  LoadTGAUncompressed( const char *filename, FILE *fpTGA );
	int  LoadTGACompressed( const char *filename, FILE *fpTGA );
	void GenerateTexture( struct_texture *texture );

public:
	CTexture();
	~CTexture();

	int LoadTextureFromBitmap( const char *filename );
	int LoadTextureFromTGA( const char *filename );
	
	int Activate();
	void Free();
	bool isLoaded() { return m_textureloaded; };

};

#endif
