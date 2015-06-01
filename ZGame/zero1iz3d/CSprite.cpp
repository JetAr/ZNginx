// CSprite.cpp -



#include "main.h"



////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: CSprite()
//// Desc:
////////////////////////////////////////////////////////////////////////////////////////////////
CSprite::CSprite()
{
	clr = rgba( 1.0f, 1.0f, 1.0f, 1.0f );
	num_frames = 0;
	cur_frame = 0;
	bAnimated = bRotate = false;
	AlphaBlend = ALPHABLEND_NONE;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: ~CSprite()
//// Desc:
////////////////////////////////////////////////////////////////////////////////////////////////
CSprite::~CSprite()
{
	frames.clear();
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Load()
//// Desc: load sprite and init frames 
////////////////////////////////////////////////////////////////////////////////////////////////
int CSprite::Load( string filename )
{

	__frame tmpfr;

	tmpfr.texture.LoadTextureFromTGA( filename.c_str() ); //"data/tree2.tga" );
	tmpfr.width = 2.0f;
	tmpfr.height = 2.0f;

	frames.push_back( tmpfr );
	num_frames++;

	return 1;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: setSize()
//// Desc: set sprite frames dimensions
////////////////////////////////////////////////////////////////////////////////////////////////
void CSprite::setSize( float w, float h )
{

	if ( num_frames > 1 )
	{
		vector<__frame>::iterator iter;
		
		for( iter = frames.begin(); iter != frames.end(); iter++ )
		{
			iter->width = w;
			iter->height = h;
		}
	}
	else if ( num_frames == 1 )
	{
		frames[0].width = w;
		frames[0].height = h;
	}
	
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: CSprite2D()
//// Desc:
////////////////////////////////////////////////////////////////////////////////////////////////
CSprite2D::CSprite2D()
{
	rot_angle = rot_speed = 0.0f;
	pos = CVector2D( 0.0f, 0.0f );
}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Render()
//// Desc: render 2D sprite
////////////////////////////////////////////////////////////////////////////////////////////////
void CSprite2D::Render( RENDERER device )
{

	float lwidth, lheight;

	glPushMatrix();
	glTranslatef( pos.x, pos.y, 0.0f );
	glRotatef( rot_angle, 0.0f, 0.0f, 1.0f );
	
	if ( bRotate ) // {!} HRT
		rot_angle += rot_speed;

	// set alphablending
	if ( AlphaBlend == ALPHABLEND_NORMAL ) 
	{
		device.setState( gl_ALPHA_TEST, true );
		glAlphaFunc( GL_GREATER, 0.1f );
		glColor4f( clr.r, clr.g, clr.b, clr.a );
	}
	else if ( AlphaBlend == ALPHABLEND_ADDITIVE )
	{
		device.setState( gl_BLEND, true );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		glColor4f( clr.r, clr.g, clr.b, clr.a );
	}
	else
		glColor3f( clr.r, clr.g, clr.b );

	// calculate center of sprite
	lwidth = (frames[cur_frame].width) * 0.5f;
	lheight = (frames[cur_frame].height)  * 0.5f;

	// set texture
	device.setState( gl_TEXTURE_2D, true );
	frames[cur_frame].texture.Activate();

	// render
	glBegin( GL_TRIANGLE_STRIP );
	glTexCoord2d( 0, 1 ); glVertex2f( -lwidth, lheight );  
	glTexCoord2d( 0, 0 ); glVertex2f( -lwidth, -lheight );
	glTexCoord2d( 1, 0 ); glVertex2f( lwidth, -lheight );
	// right-3-angle
	glTexCoord2d( 0, 1 ); glVertex2f( -lwidth, lheight );  
	glTexCoord2d( 1, 1 ); glVertex2f( lwidth, lheight );
	glTexCoord2d( 1, 0 ); glVertex2f( lwidth, -lheight );
	glEnd();

	// disable blending
	if ( AlphaBlend == ALPHABLEND_ADDITIVE ) 
		device.restoreState( gl_BLEND );
	else if ( AlphaBlend == ALPHABLEND_NORMAL ) 
		device.restoreState( gl_ALPHA_TEST );

	device.restoreState( gl_TEXTURE_2D );

	glPopMatrix();

	// advance animation frames
	if ( bAnimated )
	{
		cur_frame += 1; // HRT 
		if ( cur_frame > num_frames )
			cur_frame = 0;
	}

}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: CSprite3D()
//// Desc:
////////////////////////////////////////////////////////////////////////////////////////////////
CSprite3D::CSprite3D()
{
	pos = CVector( 0.0f, 0.0f, 0.0f );
	rot_speed = CVector( 0.0f, 0.0f, 0.0f );
	rot_angle = CVector( 0.0f, 0.0f, 0.0f );

}


////////////////////////////////////////////////////////////////////////////////////////////////
//// Name: Render()
//// Desc: render 3D sprite
////////////////////////////////////////////////////////////////////////////////////////////////
void CSprite3D::Render( RENDERER device )
{

	float lwidth, lheight;

	glPushMatrix();
	glTranslatef( pos.x, pos.y, pos.z );
	glRotatef( rot_angle.x, 1.0f, 0.0f, 0.0f );
	glRotatef( rot_angle.y, 0.0f, 1.0f, 0.0f );
	glRotatef( rot_angle.z, 0.0f, 0.0f, 1.0f );
	
	if ( bRotate ) // {!} HRT
		rot_angle += rot_speed;
	
	// set alphablending
	if ( AlphaBlend == ALPHABLEND_NORMAL ) 
	{
		//glEnable( GL_BLEND );
		//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		device.setState( gl_ALPHA_TEST, true );
		glAlphaFunc( GL_GREATER, 0.1f );
		glColor4f( clr.r, clr.g, clr.b, clr.a );

	}
	else if ( AlphaBlend == ALPHABLEND_ADDITIVE )
	{
		device.setState( gl_BLEND, true );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		glColor4f( clr.r, clr.g, clr.b, clr.a );
	}
	else
		glColor3f( clr.r, clr.g, clr.b );

	// calculate sprite center
	lwidth = (frames[cur_frame].width) * 0.5f;
	lheight = (frames[cur_frame].height)  * 0.5f;

	// set texture
	device.setState( gl_TEXTURE_2D, true );
	frames[cur_frame].texture.Activate();


	// render
	glBegin( GL_TRIANGLE_STRIP );
	glTexCoord2d( 0, 1 ); glVertex2f( -lwidth, lheight );  
	glTexCoord2d( 0, 0 ); glVertex2f( -lwidth, -lheight );
	glTexCoord2d( 1, 0 ); glVertex2f( lwidth, -lheight );
	// right-3-angle
	glTexCoord2d( 0, 1 ); glVertex2f( -lwidth, lheight );  
	glTexCoord2d( 1, 1 ); glVertex2f( lwidth, lheight );
	glTexCoord2d( 1, 0 ); glVertex2f( lwidth, -lheight );
	glEnd();

	// disable alpha blend
	if ( AlphaBlend == ALPHABLEND_ADDITIVE ) 
		device.restoreState( gl_BLEND );
	else if ( AlphaBlend == ALPHABLEND_NORMAL ) 
		device.restoreState( gl_ALPHA_TEST );

	device.restoreState( gl_TEXTURE_2D );

	glPopMatrix();

	// advance animation frames
	if ( bAnimated )
	{
		cur_frame += 1; // HRT 
		if ( cur_frame > num_frames )
			cur_frame = 0;
	}
}

/*


  	glBegin( GL_TRIANGLE_STRIP );

	v = CVector( -lwidth, lheight, 1.0f ); 	
	v.glNormal();

	glTexCoord2d( 0, 1 ); glVertex2f( -lwidth, lheight );  

	v = CVector( -lwidth, -lheight, 1.0f ); 
	v.glNormal();

	glTexCoord2d( 0, 0 ); glVertex2f( -lwidth, -lheight );

	v = CVector( lwidth, -lheight, 1.0f ); 
	v.glNormal();
	
	glTexCoord2d( 1, 0 ); glVertex2f( lwidth, -lheight );
	// right-3-angle
	v = CVector( -lwidth, lheight, 1.0f ); 
	v.glNormal();

	glTexCoord2d( 0, 1 ); glVertex2f( -lwidth, lheight );  

	v = CVector( lwidth, lheight, 1.0f ); 
	v.glNormal();

	glTexCoord2d( 1, 1 ); glVertex2f( lwidth, lheight );

	v = CVector( lwidth, -lheight, 1.0f ); 
	v.glNormal();
	
	glTexCoord2d( 1, 0 ); glVertex2f( lwidth, -lheight );
	glEnd();

*/


