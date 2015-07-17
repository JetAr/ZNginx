// CText.cpp -

#include "main.h"




////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Destroy()
//// Desc:
////////////////////////////////////////////////////////////////////////////////////////////////
void CText::Destroy()
{
    if ( base )
    {
        glDeleteLists( base, 96 );
        base = 0U;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Setup()
//// Desc: setup 2D texts
////////////////////////////////////////////////////////////////////////////////////////////////
void CText2D::Setup( COpenGL &gl, __texttype tt )
{

    HFONT font;
    HFONT oldfont;
    HDC   hdc = gl.GetHDC();

    // store 96 chars
    base = glGenLists( 96 );

    font = CreateFont( -tt.size, 0, 0, 0,
                       ( tt.bold ? FW_BOLD : FW_NORMAL ),
                       ( tt.italic ? true : false ),
                       ( tt.underlined ? true : false ),
                       false,
                       ANSI_CHARSET,
                       OUT_TT_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       ANTIALIASED_QUALITY,
                       FF_DONTCARE|DEFAULT_PITCH,
                       tt.fontname.c_str()
                     );

    oldfont = (HFONT)SelectObject( hdc, font );
    // start at char 32 and build 96 chars
    wglUseFontBitmaps( hdc, 32, 96, base);
    SelectObject( hdc, oldfont );
    DeleteObject( font );
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Write()
//// Desc: write 2D text
////////////////////////////////////////////////////////////////////////////////////////////////
void CText2D::Write( string str )
{

    glPushAttrib( GL_LIST_BIT );
    glListBase( base - 32 );
    glCallLists( str.length(), GL_UNSIGNED_BYTE, str.c_str() );
    glPopAttrib();

}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Setup()
//// Desc: setup 3D texts
////////////////////////////////////////////////////////////////////////////////////////////////
void CText3D::Setup( COpenGL &gl, __texttype tt )
{

    HFONT			   font;
    HFONT		       oldfont;
    HDC			       hdc = gl.GetHDC();
    GLYPHMETRICSFLOAT  gmf[96];					// storage info for the outline chars

    // store 96 chars
    base = glGenLists( 96 );

    font = CreateFont( -tt.size, 0, 0, 0,
                       ( tt.bold ? FW_BOLD : FW_NORMAL ),
                       ( tt.italic ? true : false ),
                       ( tt.underlined ? true : false ),
                       false,
                       ANSI_CHARSET,
                       OUT_TT_PRECIS,
                       CLIP_DEFAULT_PRECIS,
                       ANTIALIASED_QUALITY,
                       FF_DONTCARE|DEFAULT_PITCH,
                       tt.fontname.c_str()
                     );

    oldfont = (HFONT)SelectObject( hdc, font );
    // start at char 32 and build 96 chars
    wglUseFontOutlines( hdc, 32, 96, base, 0.0f, tt.zv, WGL_FONT_POLYGONS, gmf );

    // calculate width
    for ( int i = 0, width = 0.0f; i < 96; i++ )
        width += gmf[i].gmfCellIncX;

    width /= 96.0f;

    SelectObject( hdc, oldfont );
    DeleteObject( font );
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Write()
//// Desc: write 3D text
////////////////////////////////////////////////////////////////////////////////////////////////
void CText3D::Write( string str )
{
    glPushAttrib( GL_LIST_BIT );
    glListBase( base - 32 );
    glCallLists( str.length(), GL_UNSIGNED_BYTE, str.c_str() );
    glPopAttrib();
}

