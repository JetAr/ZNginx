// CCamera.cpp -


#include "main.h"




//////////////////////////////////////////////////////////////////////////////////////
/// Name: CCamera()
/// Desc: constructor
//////////////////////////////////////////////////////////////////////////////////////
CCamera::CCamera()
{
	CVector v1 = CVector( 0.0, 0.0, 0.0 );		// position vector
	CVector v2 = CVector( 0.0, 1.0, 0.5 );		// view vector ( view 'up' nad 'out' the screen )
	CVector v3 = CVector( 0.0, 0.0, 1.0 );		// up vector

	mv_pos = v1;
	mv_view = v2;
	mv_up = v3;
}


//////////////////////////////////////////////////////////////////////////////////////
/// Name: Reset()
/// Desc: reset view
//////////////////////////////////////////////////////////////////////////////////////
void CCamera::Reset()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//////////////////////////////////////////////////////////////////////////////////////
/// Name: Translate()
/// Desc: translate by coords
//////////////////////////////////////////////////////////////////////////////////////
void Translate( float x, float y, float z )
{
	glTranslatef( x, y, z );
}


//////////////////////////////////////////////////////////////////////////////////////
/// Name: SetPosition()
/// Desc: set camera vectors
//////////////////////////////////////////////////////////////////////////////////////
void CCamera::setPosition( float x, float y, float z,
						   float vx, float vy, float vz,
						   float ux, float uy, float uz )
{
	mv_pos = CVector( x, y, z );
	mv_view = CVector( vx, vy, vz );
	mv_up = CVector( ux, uy, uz );
}


//////////////////////////////////////////////////////////////////////////////////////
/// Name: SetPosition2D()
/// Desc: 
//////////////////////////////////////////////////////////////////////////////////////
void CCamera::setPosition2D( float x, float y )
{

	// convert coords
	//x = CX_RATIO * x - 0.5f;
	//y = CY_RATIO * y;
	//x = ( 1 / ( 800 / x ) ) - 0.5f;
	//y = ( 1 / ( 600 / y ) );

	//glRasterPos2i( x, y );
	glRasterPos2f(x, y);
}


//////////////////////////////////////////////////////////////////////////////////////
/// Name: LookAt()
/// Desc:
//////////////////////////////////////////////////////////////////////////////////////
void CCamera::LookAt() 
{
/*	glLookAt( mv_pos.x,  mv_pos.y,  mv_pos.z,
			  mv_view.x, mv_view.y, mv_view.z,
			  mv_up.x,   mv_up.y,   mv_up.z );*/
}

//////////////////////////////////////////////////////////////////////////////////////
/// Name:
/// Desc:
//////////////////////////////////////////////////////////////////////////////////////
void CCamera::GetDirection( CVector &v )
{
	// our view minus our position
	v = mv_view - mv_pos;
}

//////////////////////////////////////////////////////////////////////////////////////
/// Name: Move()
/// Desc: move camera
//////////////////////////////////////////////////////////////////////////////////////
void CCamera::Move( float speed )
{
	CVector v_dir;
	float   ax, az;

	// get the direction we are looking at
	GetDirection( v_dir );

	ax = v_dir.x * speed;
	az = v_dir.z * speed;

	// move position
	mv_pos.x += ax;
	mv_pos.z += az;

	// move the view
	mv_view.x += ax;
	mv_view.z += az;
}


//////////////////////////////////////////////////////////////////////////////////////
/// Name: Rotate()
/// Desc: rotate camera
//////////////////////////////////////////////////////////////////////////////////////
void CCamera::Rotate( float angle, CVector v )
{

	CVector vnew_dir;
	float	sin_th, cos_th;

	// get our currect facing
	GetDirection( mv_view );

	sin_th = (float)sin(angle);
	cos_th = (float)cos(angle);

	// here's the nightmare -] find the new position vertices
	vnew_dir.x  = ( cos_th + ( 1 - cos_th ) * v.x ) * mv_view.x;
	vnew_dir.x += ( ( 1 - cos_th ) * v.x * v.y - v.z * sin_th ) * mv_view.y;
	vnew_dir.x += ( ( 1 - cos_th ) * v.x * v.z + v.y * sin_th ) * mv_view.z;

	vnew_dir.y  = ( ( 1 - cos_th ) * v.x * v.y + v.z * sin_th ) * mv_view.x;
	vnew_dir.y += ( cos_th + ( 1 - cos_th ) * v.y ) * mv_view.y;
	vnew_dir.y += ( ( 1 - cos_th ) * v.y * v.z - v.x * sin_th ) * mv_view.z;

	vnew_dir.z  = ( ( 1 - cos_th ) * v.x * v.z - v.y * sin_th ) * mv_view.x;
	vnew_dir.z += ( ( 1 - cos_th ) * v.y * v.z + v.x * sin_th ) * mv_view.y;
	vnew_dir.z += ( cos_th + ( 1 - cos_th ) * v.z ) * mv_view.z;
		
	// add the rotated vector to our position to set the new view vector
	mv_view = mv_pos + vnew_dir;

}


//////////////////////////////////////////////////////////////////////////////////////
/// Name: Rotate()
/// Desc:
//////////////////////////////////////////////////////////////////////////////////////
void CCamera::Rotate( float angle, float x, float y, float z)
{
	CVector vs = CVector( x, y, z );

	Rotate( angle, vs );
}



//////////////////////////////////////////////////////////////////////////////////////
/// Name: Strafe()
/// Desc: strafe camera
//////////////////////////////////////////////////////////////////////////////////////
void CCamera::Strafe( float speed )
{
	CVector v_dir;
	CVector v_pro;
	float   magnitude = 0.0f;

	// get direction
	GetDirection( v_dir );

	// nameri komplanarniq vektor na up i view
	v_pro.Product( v_dir, mv_up );
	
	// normalize product vector
	v_pro.Normalize();

	// save strafe values
	mv_strafe = v_pro;

	float ax = mv_strafe.x * speed;
	float az = mv_strafe.z * speed;

	// move position and view
	mv_pos.x += ax;
	mv_pos.z += az;
	mv_view.x += ax;
	mv_view.z += az;
}


