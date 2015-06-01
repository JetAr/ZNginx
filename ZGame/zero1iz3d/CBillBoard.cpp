// CBillBoard.cpp -



#include "main.h"



CBillBoardFragment::CBillBoardFragment()
{
	pos = CVector( 0.0f, 0.0f, 0.0f );
	clr = rgba( 1.0f, 1.0f, 1.0f, 1.0f );
	width = height = 0;
}


CBillBoardFragment::CBillBoardFragment( const CBillBoardFragment &bb )
{
	pos = bb.pos;
	clr = bb.clr;
	width = bb.width;
	height = bb.height;
}


void CBillBoardFragment::Rotate( CVector axis_angles )
{
	glRotatef( axis_angles.x, 1.0f, 0.0f, 0.0f );
	glRotatef( axis_angles.y, 0.0f, 1.0f, 0.0f );
	glRotatef( axis_angles.z, 0.0f, 0.0f, 1.0f );
}



int CBillBoard::setTexture( string filename )
{
	if ( tex.isLoaded() )
		tex.Free();

	if ( tex.LoadTextureFromTGA( filename.c_str() ) )
		return 1;

	return 0;
};


void CBillBoard::setTexture( CTexture &texture )
{
	if ( tex.isLoaded() )
		tex.Free();

	tex = texture;
}



void CBillBoard::Reneder( RENDERER device )
{
	// check for billboards in list
	if ( bills.empty() )
		return;

	billtype::iterator p = bills.begin();
	CVector			   v;
	CMatrix			   vm, id;
	rgba			   clr;
	float			   w, h;	

	// get view and projection matrices
	device.getViewMat( vm );

	id.Identity();
	device.setViewMat( id );
	
	// set states
	device.setState( gl_TEXTURE_2D, true );

	// set texture and blending
	tex.Activate();
	device.setState( gl_ALPHA_TEST,  true );
	glAlphaFunc( GL_GREATER, 0.1f );
	
	glBegin( GL_QUADS );
	//glBegin( GL_TRIANGLES );

	for ( ; p != bills.end(); p++ )
	{
		// vzemi goleminata na billboarda
		//float wd, hd;
		p->getSize( w, h );
		w *= 0.5f;
		h *= 0.5f;

		// translirai vector na posiciqta kym view matricata
		// osnovno poluchavame vector usporeden na view-vectora
		v = p->getPosition();
		v = vm * v;

		// sloji cveta
		clr = p->getColor();
		glColor4f( clr.r, clr.g, clr.b, clr.a );

		// rendirai
	    glTexCoord2d(1,1); glVertex3f(v.x+w,v.y+h,v.z); // Top Right
		glTexCoord2d(0,1); glVertex3f(v.x-w,v.y+h,v.z); // Top Left
		glTexCoord2d(0,0); glVertex3f(v.x-w,v.y-h,v.z); // Bottom Left
		glTexCoord2d(1,0); glVertex3f(v.x+w,v.y-h,v.z); // Bottom Right
		// left side
		/*glTexCoord2d( 0, 0 ); glVertex3f( v.x -w, v.y - h, v.z );  
		glTexCoord2d( 0, 1 ); glVertex3f( v.x - w, v.y + h, v.z );
		glTexCoord2d( 1, 0 ); glVertex3f( v.x + w, v.y -h, v.z  );
		// right-3-angle
		glTexCoord2d( 0, 1 ); glVertex3f( v.x - w, v.y + h, v.z );
		glTexCoord2d( 0, 0 ); glVertex3f( v.x - w, v.y - h, v.z );
		glTexCoord2d( 0, 1 ); glVertex3f( v.x - w, v.y + h, v.z );  */
	}

	glEnd();

	// restore states
	device.restoreState( gl_TEXTURE_2D );
	device.restoreState( gl_ALPHA_TEST );

	// restore view matrix
	device.setViewMat( vm );

}


void CBillBoard::Destroy()
{
	tex.Free();
	Clear();
}

