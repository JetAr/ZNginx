// CCamera.h -


#ifndef __CCAMERA_INCLUDED
#define __CCAMERA_INCLUDED


#include <math.h>
#include "Geometry.h"

#define CX_RATIO  0.00125f  // (1 / 800)
#define CY_RATIO  0.00166f  // (1 / 600)

/*
#define UP              0.1               // Forward speed.
#define DOWN            -0.1              // Backward speed.
#define LEFT            0.03              // Left speed.
#define RIGHT           -0.03             // Right speed.
#define STRAFE_LEFT     -0.03             // Left straft speed.
#define STRAFE_RIGHT    0.03              // Right straft speed.
*/


class CCamera 
{

public:

	CVector  mv_pos;			// camera position
	CVector  mv_view;			// view 
	CVector  mv_up;				// up vector
	CVector  mv_strafe;			// strafe vector


	CCamera();

	void Reset();
	void Translate( float, float, float );
	void setPosition( float x, float y, float z,  float vx, float vy, float vz,  float ux, float uy, float uz );
	void setPosition2D( float, float );
	void LookAt();
	void GetDirection( CVector &v );
	void Move( float speed );
	void Rotate( float angle, CVector v );
	void Rotate( float angle, float , float , float );
	void Strafe( float speed );
	

};



#endif
