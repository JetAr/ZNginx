// CVector.h -


#ifndef __CGEOMETRY_INCLUDED
#define __CGEOMETRY_INCLUDED


#include <cmath>
#include <memory.h>

#define PI		   3.1415926f
#define PI2		   6.2831853f
#define PI_2	   1.5707963f



//////////////////
class CVector;
class CVector2D;
class CMatrix;
class CBoundBox;
//////////////////


// 3D vector
class CVector
{

public:
    float x,y,z;				// components
    //float nx, ny, nz;			// normals

    CVector()
    {
        x = y = z = 0.0f;
    };
    CVector( float X, float Y, float Z );
    CVector( const CVector &v );
    CVector( CVector v1, CVector v2 );

    void operator=( CVector v );
    CVector operator+( CVector v );
    CVector operator-( CVector v );
    CVector operator*( float );
    CVector operator/( float );

    void operator+=( CVector v );
    void operator-=( CVector v );
    void operator*=( float );
    void operator/=( float );

    bool operator==( CVector v );
    bool operator!=( CVector v );

    void ToArray( float* );
    void NormalsToArray( float* );
    void Product( CVector v1, CVector v2 );
    float GetMagnitude();
    //void SetNormals();
    void Normalize();
    void NormalizeTriangle( CVector triangle[] );

    void glNormal();
    void glVector()
    {
        glVertex3f( x, y, z );
    };


};


// 2D vector ( stores UV coordinates )
class CVector2D
{

public:
    float x,y;				// components

    CVector2D()
    {
        x = y = 0.0f;
    };
    CVector2D( float X, float Y );
    CVector2D( const CVector2D &v );
    CVector2D( CVector2D v1, CVector2D v2 );

    void operator=( CVector2D v );
    CVector2D operator+( CVector2D v );
    CVector2D operator-( CVector2D v );
    CVector2D operator*( float );
    CVector2D operator/( float );

    void operator+=( CVector2D v );
    void operator-=( CVector2D v );
    void operator*=( float );
    void operator/=( float );

    bool operator==( CVector2D v );
    bool operator!=( CVector2D v );

    void ToArray( float* );
    float GetMagnitude();
    void Normalize();
};


// Bounding box - class
class CBoundBox
{

private:
    CVector pos;			// position in 3D space
    CVector dims;			// w,h,l - dimensions
    CVector edge;			// pos + dims

public:

    CBoundBox();
    CBoundBox( CVector position, CVector dimensions );

    // check for vertex collision
    bool Collide( const CVector &v );
    // check if it collide with another box
    bool Collide( CBoundBox &bb );

    void setPosition( CVector position )
    {
        pos = position;
        edge = pos + dims;
    };
    void setDimensions( CVector dimensions )
    {
        dims = dimensions;
        edge = pos + dims;
    };
    CVector getPosition()
    {
        return pos;
    };
    CVector getDimensions()
    {
        return dims;
    };
    CVector getEdge()
    {
        return edge;
    };

};

/*

  // identity matrix
  1 0 0 0
  0 1 0 0
  0 0 1 0
  0 0 0 1


  */


// Matrices {!} - To be completed...
class CMatrix
{

public:
    float mat[16];

    CMatrix()
    {
        Identity();
    };
    CMatrix( const CMatrix &m )
    {
        memcpy( mat, m.mat, sizeof(mat) );
    };
    CMatrix( float *m )
    {
        memcpy( mat, m, sizeof(mat) );
    };

    void Identity()
    {
        Clear();
        mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
    };
    void Clear()
    {
        memset( mat, 0, sizeof(mat) ); /*for( int i = 0; i < 16; i++ ) mat[i] = 0.0f;*/
    };

    CMatrix operator* ( CMatrix &m );
    CMatrix operator*= ( CMatrix &m );
    CMatrix operator+ ( CMatrix &m );
    CMatrix operator+= ( CMatrix &m );

    CVector operator *( CVector &v );

    void Transpose();
    void Translate( float x, float y, float z );
    void Scale( float sx, float sy, float sz );
    void RotateX( int angle );
    void RotateY( int angle );
    void RotateZ( int angle );
    void Rotate( int xa, int ya, int za );
    void FlipX();

};


#endif
