// CVector.cpp -


#include "main.h"


// precalcilirani Cos i Sin functions, consuming ~2.8kb memory
float g_cos[] =
{
    1.0000f,0.9998f,0.9994f,0.9986f,0.9976f,0.9962f,0.9945f,0.9925f,
    0.9903f,0.9877f,0.9848f,0.9816f,0.9781f,0.9744f,0.9703f,0.9659f,
    0.9613f,0.9563f,0.9511f,0.9455f,0.9397f,0.9336f,0.9272f,0.9205f,
    0.9135f,0.9063f,0.8988f,0.8910f,0.8829f,0.8746f,0.8660f,0.8572f,
    0.8480f,0.8387f,0.8290f,0.8192f,0.8090f,0.7986f,0.7880f,0.7771f,
    0.7660f,0.7547f,0.7431f,0.7314f,0.7193f,0.7071f,0.6947f,0.6820f,
    0.6691f,0.6561f,0.6428f,0.6293f,0.6157f,0.6018f,0.5878f,0.5736f,
    0.5592f,0.5446f,0.5299f,0.5150f,0.5000f,0.4848f,0.4695f,0.4540f,
    0.4384f,0.4226f,0.4067f,0.3907f,0.3746f,0.3584f,0.3420f,0.3256f,
    0.3090f,0.2924f,0.2756f,0.2588f,0.2419f,0.2250f,0.2079f,0.1908f,
    0.1736f,0.1564f,0.1392f,0.1219f,0.1045f,0.0872f,0.0698f,0.0523f,
    0.0349f,0.0175f,-0.0000f,-0.0175f,-0.0349f,-0.0523f,-0.0698f,-0.0872f,
    -0.1045f,-0.1219f,-0.1392f,-0.1564f,-0.1736f,-0.1908f,-0.2079f,-0.2250f,
    -0.2419f,-0.2588f,-0.2756f,-0.2924f,-0.3090f,-0.3256f,-0.3420f,-0.3584f,
    -0.3746f,-0.3907f,-0.4067f,-0.4226f,-0.4384f,-0.4540f,-0.4695f,-0.4848f,
    -0.5000f,-0.5150f,-0.5299f,-0.5446f,-0.5592f,-0.5736f,-0.5878f,-0.6018f,
    -0.6157f,-0.6293f,-0.6428f,-0.6561f,-0.6691f,-0.6820f,-0.6947f,-0.7071f,
    -0.7193f,-0.7314f,-0.7431f,-0.7547f,-0.7660f,-0.7771f,-0.7880f,-0.7986f,
    -0.8090f,-0.8192f,-0.8290f,-0.8387f,-0.8480f,-0.8572f,-0.8660f,-0.8746f,
    -0.8829f,-0.8910f,-0.8988f,-0.9063f,-0.9135f,-0.9205f,-0.9272f,-0.9336f,
    -0.9397f,-0.9455f,-0.9511f,-0.9563f,-0.9613f,-0.9659f,-0.9703f,-0.9744f,
    -0.9781f,-0.9816f,-0.9848f,-0.9877f,-0.9903f,-0.9925f,-0.9945f,-0.9962f,
    -0.9976f,-0.9986f,-0.9994f,-0.9998f,-1.0000f,-0.9998f,-0.9994f,-0.9986f,
    -0.9976f,-0.9962f,-0.9945f,-0.9925f,-0.9903f,-0.9877f,-0.9848f,-0.9816f,
    -0.9781f,-0.9744f,-0.9703f,-0.9659f,-0.9613f,-0.9563f,-0.9511f,-0.9455f,
    -0.9397f,-0.9336f,-0.9272f,-0.9205f,-0.9135f,-0.9063f,-0.8988f,-0.8910f,
    -0.8829f,-0.8746f,-0.8660f,-0.8572f,-0.8480f,-0.8387f,-0.8290f,-0.8192f,
    -0.8090f,-0.7986f,-0.7880f,-0.7771f,-0.7660f,-0.7547f,-0.7431f,-0.7314f,
    -0.7193f,-0.7071f,-0.6947f,-0.6820f,-0.6691f,-0.6561f,-0.6428f,-0.6293f,
    -0.6157f,-0.6018f,-0.5878f,-0.5736f,-0.5592f,-0.5446f,-0.5299f,-0.5150f,
    -0.5000f,-0.4848f,-0.4695f,-0.4540f,-0.4384f,-0.4226f,-0.4067f,-0.3907f,
    -0.3746f,-0.3584f,-0.3420f,-0.3256f,-0.3090f,-0.2924f,-0.2756f,-0.2588f,
    -0.2419f,-0.2250f,-0.2079f,-0.1908f,-0.1736f,-0.1564f,-0.1392f,-0.1219f,
    -0.1045f,-0.0872f,-0.0698f,-0.0523f,-0.0349f,-0.0175f,0.0000f,0.0175f,
    0.0349f,0.0523f,0.0698f,0.0872f,0.1045f,0.1219f,0.1392f,0.1564f,
    0.1736f,0.1908f,0.2079f,0.2250f,0.2419f,0.2588f,0.2756f,0.2924f,
    0.3090f,0.3256f,0.3420f,0.3584f,0.3746f,0.3907f,0.4067f,0.4226f,
    0.4384f,0.4540f,0.4695f,0.4848f,0.5000f,0.5150f,0.5299f,0.5446f,
    0.5592f,0.5736f,0.5878f,0.6018f,0.6157f,0.6293f,0.6428f,0.6561f,
    0.6691f,0.6820f,0.6947f,0.7071f,0.7193f,0.7314f,0.7431f,0.7547f,
    0.7660f,0.7771f,0.7880f,0.7986f,0.8090f,0.8192f,0.8290f,0.8387f,
    0.8480f,0.8572f,0.8660f,0.8746f,0.8829f,0.8910f,0.8988f,0.9063f,
    0.9135f,0.9205f,0.9272f,0.9336f,0.9397f,0.9455f,0.9511f,0.9563f,
    0.9613f,0.9659f,0.9703f,0.9744f,0.9781f,0.9816f,0.9848f,0.9877f,
    0.9903f,0.9925f,0.9945f,0.9962f,0.9976f,0.9986f,0.9994f,0.9998f
};


float g_sin[] =
{
    0.0000f,0.0175f,0.0349f,0.0523f,0.0698f,0.0872f,0.1045f,0.1219f,
    0.1392f,0.1564f,0.1736f,0.1908f,0.2079f,0.2250f,0.2419f,0.2588f,
    0.2756f,0.2924f,0.3090f,0.3256f,0.3420f,0.3584f,0.3746f,0.3907f,
    0.4067f,0.4226f,0.4384f,0.4540f,0.4695f,0.4848f,0.5000f,0.5150f,
    0.5299f,0.5446f,0.5592f,0.5736f,0.5878f,0.6018f,0.6157f,0.6293f,
    0.6428f,0.6561f,0.6691f,0.6820f,0.6947f,0.7071f,0.7193f,0.7314f,
    0.7431f,0.7547f,0.7660f,0.7771f,0.7880f,0.7986f,0.8090f,0.8192f,
    0.8290f,0.8387f,0.8480f,0.8572f,0.8660f,0.8746f,0.8829f,0.8910f,
    0.8988f,0.9063f,0.9135f,0.9205f,0.9272f,0.9336f,0.9397f,0.9455f,
    0.9511f,0.9563f,0.9613f,0.9659f,0.9703f,0.9744f,0.9781f,0.9816f,
    0.9848f,0.9877f,0.9903f,0.9925f,0.9945f,0.9962f,0.9976f,0.9986f,
    0.9994f,0.9998f,1.0000f,0.9998f,0.9994f,0.9986f,0.9976f,0.9962f,
    0.9945f,0.9925f,0.9903f,0.9877f,0.9848f,0.9816f,0.9781f,0.9744f,
    0.9703f,0.9659f,0.9613f,0.9563f,0.9511f,0.9455f,0.9397f,0.9336f,
    0.9272f,0.9205f,0.9135f,0.9063f,0.8988f,0.8910f,0.8829f,0.8746f,
    0.8660f,0.8572f,0.8480f,0.8387f,0.8290f,0.8192f,0.8090f,0.7986f,
    0.7880f,0.7771f,0.7660f,0.7547f,0.7431f,0.7314f,0.7193f,0.7071f,
    0.6947f,0.6820f,0.6691f,0.6561f,0.6428f,0.6293f,0.6157f,0.6018f,
    0.5878f,0.5736f,0.5592f,0.5446f,0.5299f,0.5150f,0.5000f,0.4848f,
    0.4695f,0.4540f,0.4384f,0.4226f,0.4067f,0.3907f,0.3746f,0.3584f,
    0.3420f,0.3256f,0.3090f,0.2924f,0.2756f,0.2588f,0.2419f,0.2250f,
    0.2079f,0.1908f,0.1736f,0.1564f,0.1392f,0.1219f,0.1045f,0.0872f,
    0.0698f,0.0523f,0.0349f,0.0175f,-0.0000f,-0.0175f,-0.0349f,-0.0523f,
    -0.0698f,-0.0872f,-0.1045f,-0.1219f,-0.1392f,-0.1564f,-0.1736f,-0.1908f,
    -0.2079f,-0.2250f,-0.2419f,-0.2588f,-0.2756f,-0.2924f,-0.3090f,-0.3256f,
    -0.3420f,-0.3584f,-0.3746f,-0.3907f,-0.4067f,-0.4226f,-0.4384f,-0.4540f,
    -0.4695f,-0.4848f,-0.5000f,-0.5150f,-0.5299f,-0.5446f,-0.5592f,-0.5736f,
    -0.5878f,-0.6018f,-0.6157f,-0.6293f,-0.6428f,-0.6561f,-0.6691f,-0.6820f,
    -0.6947f,-0.7071f,-0.7193f,-0.7314f,-0.7431f,-0.7547f,-0.7660f,-0.7771f,
    -0.7880f,-0.7986f,-0.8090f,-0.8192f,-0.8290f,-0.8387f,-0.8480f,-0.8572f,
    -0.8660f,-0.8746f,-0.8829f,-0.8910f,-0.8988f,-0.9063f,-0.9135f,-0.9205f,
    -0.9272f,-0.9336f,-0.9397f,-0.9455f,-0.9511f,-0.9563f,-0.9613f,-0.9659f,
    -0.9703f,-0.9744f,-0.9781f,-0.9816f,-0.9848f,-0.9877f,-0.9903f,-0.9925f,
    -0.9945f,-0.9962f,-0.9976f,-0.9986f,-0.9994f,-0.9998f,-1.0000f,-0.9998f,
    -0.9994f,-0.9986f,-0.9976f,-0.9962f,-0.9945f,-0.9925f,-0.9903f,-0.9877f,
    -0.9848f,-0.9816f,-0.9781f,-0.9744f,-0.9703f,-0.9659f,-0.9613f,-0.9563f,
    -0.9511f,-0.9455f,-0.9397f,-0.9336f,-0.9272f,-0.9205f,-0.9135f,-0.9063f,
    -0.8988f,-0.8910f,-0.8829f,-0.8746f,-0.8660f,-0.8572f,-0.8480f,-0.8387f,
    -0.8290f,-0.8192f,-0.8090f,-0.7986f,-0.7880f,-0.7771f,-0.7660f,-0.7547f,
    -0.7431f,-0.7314f,-0.7193f,-0.7071f,-0.6947f,-0.6820f,-0.6691f,-0.6561f,
    -0.6428f,-0.6293f,-0.6157f,-0.6018f,-0.5878f,-0.5736f,-0.5592f,-0.5446f,
    -0.5299f,-0.5150f,-0.5000f,-0.4848f,-0.4695f,-0.4540f,-0.4384f,-0.4226f,
    -0.4067f,-0.3907f,-0.3746f,-0.3584f,-0.3420f,-0.3256f,-0.3090f,-0.2924f,
    -0.2756f,-0.2588f,-0.2419f,-0.2250f,-0.2079f,-0.1908f,-0.1736f,-0.1564f,
    -0.1392f,-0.1219f,-0.1045f,-0.0872f,-0.0698f,-0.0523f,-0.0349f,-0.0175f
};






/******************* CLASS CVECTOR 3D ********************/

CVector::CVector( float X, float Y, float Z )
{
    x = X;
    y = Y;
    z = Z;
}


CVector::CVector( const CVector &v )
{
    x = v.x;
    y = v.y;
    z = v.z;
}


CVector::CVector( CVector v1, CVector v2 )
{
    x = v1.x - v2.x;
    y = v1.y - v2.y;
    z = v1.z - v2.z;
}


void CVector::operator =( CVector v )
{
    x = v.x;
    y = v.y;
    z = v.z;
}


CVector CVector::operator +( CVector v )
{
    return CVector( x + v.x , y + v.y, z + v.z );
}


CVector CVector::operator -( CVector v )
{
    return CVector( x - v.x , y - v.y, z - v.z );
}


CVector CVector::operator *( float n )
{
    return CVector( x * n, y * n, z * n );
}


CVector CVector::operator /( float n )
{
    n /= 1;

    return CVector( x * n, y * n, z * n );
}



void CVector::operator +=( CVector v )
{
    x += v.x;
    y += v.y;
    z += v.z;
}


void CVector::operator -=( CVector v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
}


void CVector::operator *=( float n )
{
    x *= n;
    y *= n;
    z *= n;
}


void CVector::operator /=( float n )
{
    n /= 1;
    x *= n;
    y *= n;
    z *= n;
}


bool CVector::operator ==( CVector v )
{
    if ( x == v.x && y == v.y && z == v.z )
        return true;

    return false;
}


bool CVector::operator !=( CVector v )
{
    if ( x == v.x && y == v.y && z == v.y )
        return false;

    return true;
}


void CVector::ToArray( float *v )
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}


void CVector::NormalsToArray( float *v )
{
    //v[0] = nx;
//	v[1] = ny;
    //v[2] = nz;
}


void CVector::Product( CVector v1, CVector v2 )
{
    // a X b = c( y1*z2 - z1*y2, x2*z1 - x1*z2, x1*y2 - x2*y1 )   // vectorno proivezdenie
    x = ( v1.y * v2.z ) - ( v1.z * v2.y );
    y = ( v2.x * v1.z ) - ( v1.x * v2.z );
    z = ( v1.x * v2.y ) - ( v2.x * v1.y );
}

float CVector::GetMagnitude()
{
    return (float)sqrt( x*x + y*y + z*z );
}


/*void CVector::SetNormals()]
{
	float len = GetMagnitude();

	// prevent division by zero
	if ( len == 0.0f )
		len = 1.0f;

	nx = x /= len;
	ny = y /= len;
	nz = z /= len;
}*/

void CVector::glNormal()
{
    float len = GetMagnitude();
    float nx, ny, nz;

    // prevent division by zero
    if ( len == 0.0f )
        len = 1.0f;

    nx = x / len;
    ny = y / len;
    nz = z / len;
    glNormal3f( nx, ny, nz );

}


void CVector::Normalize()
{
    float len = GetMagnitude();

    // prevent division by zero
    if ( len == 0.0f )
        len = 1.0f;

    x = x / len;
    y = y / len;
    z = z / len;
}


void CVector::NormalizeTriangle( CVector triangle[] )
{
    CVector v1, v2;

    // get vectors from triangle vertices
    v1.x = triangle[0].x - triangle[1].x;
    v1.y = triangle[0].y - triangle[1].y;
    v1.z = triangle[0].z - triangle[1].z;

    v2.x = triangle[1].x - triangle[2].x;
    v2.y = triangle[1].y - triangle[2].y;
    v2.z = triangle[1].z - triangle[2].z;

    // get vectors product
    Product( v1, v2 );
    // normalize this product vector
    Normalize();
}



/******************* CLASS CVECTOR2D ********************/




CVector2D::CVector2D( float X, float Y )
{
    x = X;
    y = Y;
}


CVector2D::CVector2D( const CVector2D &v )
{
    x = v.x;
    y = v.y;
}


CVector2D::CVector2D( CVector2D v1, CVector2D v2 )
{
    x = v1.x - v2.x;
    y = v1.y - v2.y;
}


void CVector2D::operator =( CVector2D v )
{
    x = v.x;
    y = v.y;
}


CVector2D CVector2D::operator +( CVector2D v )
{
    return CVector2D( x + v.x , y + v.y );
}


CVector2D CVector2D::operator -( CVector2D v )
{
    return CVector2D( x - v.x , y - v.y );
}


CVector2D CVector2D::operator *( float n )
{
    return CVector2D( x * n, y * n );
}


CVector2D CVector2D::operator /( float n )
{
    n /= 1;

    return CVector2D( x * n, y * n );
}



void CVector2D::operator +=( CVector2D v )
{
    x += v.x;
    y += v.y;
}


void CVector2D::operator -=( CVector2D v )
{
    x -= v.x;
    y -= v.y;
}


void CVector2D::operator *=( float n )
{
    x *= n;
    y *= n;
}


void CVector2D::operator /=( float n )
{
    n /= 1;
    x *= n;
    y *= n;
}


bool CVector2D::operator ==( CVector2D v )
{
    if ( x == v.x && y == v.y )
        return true;

    return false;
}


bool CVector2D::operator !=( CVector2D v )
{
    if ( x == v.x && y == v.y )
        return false;

    return true;
}


void CVector2D::ToArray( float *v )
{
    v[0] = x;
    v[1] = y;
}

float CVector2D::GetMagnitude()
{
    return (float)sqrt( x*x + y*y );
}


void CVector2D::Normalize()
{
    float len = GetMagnitude();

    // prevent division by zero
    if ( len == 0.0f )
        len = 1.0f;

    x = x / len;
    y = y / len;
}


/******************* CLASS CMATRIX ********************/


// umnojenie na matrici ( ah, bai Geniooo, tvoita ma.. :-]]] )
CMatrix CMatrix::operator * ( CMatrix &m )
{
    CMatrix rm;

    // mresult(j+i*4)=m1->row(i)*m2->column(j)

    for( int i = 0; i < 4; i++ )
    {
        int y = 4*i;
        float v1 = mat[0+y], v2 = mat[1+y], v3 = mat[2+y], v4 = mat[3+y];

        for ( int j = 0; j < 4; j++ )
            rm.mat[j+i*4] = ( v1 * m.mat[j+0] ) + ( v2 * m.mat[j+4] ) + \
                            ( v3 * m.mat[j+8] ) + ( v4 * m.mat[j+12] );
    }

    return rm;
}


CMatrix CMatrix::operator *= ( CMatrix &m )
{
    CMatrix rm;

    for( int i = 0; i < 4; i++ )
    {
        int y = 4*i;
        float v1 = mat[0+y], v2 = mat[1+y], v3 = mat[2+y], v4 = mat[3+y];

        for ( int j = 0; j < 4; j++ )
            rm.mat[j+i*4] = ( v1 * m.mat[j+0] ) + ( v2 * m.mat[j+4] ) + \
                            ( v3 * m.mat[j+8] ) + ( v4 * m.mat[j+12] );
    }

    // copy to local matrix
    memcpy( mat, rm.mat, sizeof(mat) );

    return *this;
}

// sabirane na matrici
CMatrix CMatrix::operator + ( CMatrix &m )
{
    CMatrix rm;

    for ( int i = 0; i < 16; i++ )
        rm.mat[i] = mat[i] + m.mat[i];

    return rm;
}


CMatrix CMatrix::operator += ( CMatrix &m )
{
    CMatrix rm;

    for ( int i = 0; i < 16; i++ )
        rm.mat[i] = mat[i] + m.mat[i];

    memcpy( mat, rm.mat, sizeof(mat) );

    return *this;
}

// transponirane na matrica
void CMatrix::Transpose()
{
    float tm[16];

    for ( int i = 0; i < 4; i++ )
        for ( int j = 0; j < 4; j++ )
            tm[i*4+j] = mat[i+j*4];

    memcpy( mat, tm, sizeof(mat) );
}

// translaciq na vector po matrica ( dadena koordinatna sistema )
CVector CMatrix::operator * ( CVector &v )
{
    CVector tv;

    tv.x = v.x * mat[0] + v.y * mat[4] + v.z * mat[8] + mat[12];
    tv.y = v.x * mat[1] + v.y * mat[5] + v.z * mat[9] +  mat[13];
    tv.z = v.x * mat[2] + v.y * mat[6] + v.z * mat[10] + mat[14];
    //tv.w = v.x * mat[12] + v.y * mat[13] + v.z * mat[14] + v.w * mat[15];

    return tv;
}


// translaciq po kooridnati
void CMatrix::Translate( float x, float y, float z )
{
    Identity();
    mat[12] = x;
    mat[13] = y;
    mat[14] = z;
}

// scaling
void CMatrix::Scale( float sx, float sy, float sz )
{
    Identity();
    mat[0] = sx;
    mat[5] = sy;
    mat[10] = sz;
    mat[15] = 1;
}

// rotaciq okolo X-axis, vsystnost vartim osite YZ -> Pitch
void CMatrix::RotateX( int angle )
{
    float cosx = g_cos[angle];
    float sinx = g_sin[angle];

    Identity();
    mat[5] = cosx;
    mat[6] = -sinx;
    mat[9] = sinx;
    mat[10] = cosx;
}

// rotaciq okolo Y-axis, vartim osite XZ -> Yaw
void CMatrix::RotateY( int angle )
{
    float cosx = g_cos[angle];
    float sinx = g_sin[angle];

    Identity();
    mat[0] = cosx;
    mat[2] = -sinx;
    mat[8] = sinx;
    mat[10] = cosx;
}


// rotaciq okolo Z-axis, vartim osite XY  -> Roll
void CMatrix::RotateZ( int angle )
{
    float cosx = g_cos[angle];
    float sinx = g_sin[angle];

    Identity();
    mat[0] = cosx;
    mat[1] = -sinx;
    mat[4] = sinx;
    mat[5] = cosx;
}

// Rotate
void CMatrix::Rotate( int xa, int ya, int za )
{
    float c1 = g_cos[xa];
    float s1 = g_sin[xa];
    float c2 = g_cos[ya];
    float s2 = g_sin[ya];
    float c3 = g_cos[za];
    float s3 = g_sin[za];

    Identity();

    mat[0] = c1*c3 + s1*s2*s3;
    mat[1] = -c1*s3 + c3*s1*s2;
    mat[2] = c2 * s1;

    mat[4] = c2 * s3;
    mat[5] = c2 * c3;
    mat[6] = -s2;

    mat[8] = -c3*s1 + c1*s2*s3;
    mat[9] = s1*s3 + c1*c3*s2;
    mat[10] = c1*c2;
}

// Left-Hand correction, razmeniq X tai che +X she otiva nadqsno
void CMatrix::FlipX()
{
    Identity();
    mat[0] = -1.0f;
}

/******************* CLASS CBOUNDBOX ********************/


CBoundBox::CBoundBox()
{

    //...

}

CBoundBox::CBoundBox( CVector position, CVector dimensions )
{
    pos = position;
    dims = dimensions;
    edge = pos + dims;
}



bool CBoundBox::Collide( const CVector &v )
{

    float x1 = pos.x, y1 = pos.y, z1 = pos.z;
    float ex1 = edge.x, ey1 = edge.y, ez1 = edge.z;

    if ( v.x < x1 )				// test x-axis
        return false;

    if ( v.x > ex1 )
        return false;

    if ( v.y < y1 )				// test y-axis
        return false;

    if ( v.y > ey1 )
        return false;

    if ( v.z < z1 )				// test z-axis
        return false;

    if ( v.z > ez1 )
        return false;

    return true;
}


bool CBoundBox::Collide( CBoundBox &bb )
{
    float x1 = pos.x, y1 = pos.y, z1 = pos.z;
    float ex1 = edge.x, ey1 = edge.y, ez1 = edge.z;

    CVector v_bbe = bb.getPosition();
    float x2 = v_bbe.x, y2 = v_bbe.y, z2 = v_bbe.z;

    v_bbe = bb.getEdge();
    float ex2 = v_bbe.x, ey2 = v_bbe.y, ez2 = v_bbe.z;

    /*
    	x1 = ( x1 >= x2 && x1 <= ex2 ) || ( ex1 >= x2 && ex1 <= ex2 );
    	y1 = ( y1 >= y2 && y1 <= ey2 ) || ( ey1 >= y2 && ey1 <= ey2 );
    	z1 = ( z1 >= z2 && z1 <= ez2 ) || ( ez1 >= z2 && ez1 <= ez2 );
    	x2 = ( x2 >= x1 && x2 <= ex1 ) || ( ex2 >= x1 && ex2 <= ex1 );
    	y2 = ( y2 >= y1 && y2 <= ey1 ) || ( ey2 >= y1 && ey2 <= ey1 );
    	z2 = ( z2 >= z1 && z2 <= ez1 ) || ( ez2 >= z1 && ez2 <= ez1 );

    */
    if ( ( x1 >= x2 && x1 <= ex2 ) || ( ex1 >= x2 && ex1 <= ex2 ) )
        if ( ( y1 >= y2 && y1 <= ey2 ) || ( ey1 >= y2 && ey1 <= ey2 ) )
            if ( ( z1 >= z2 && z1 <= ez2 ) || ( ez1 >= z2 && ez1 <= ez2 ) )
            {
                return true;
            }


    if ( ( x2 >= x1 && x2 <= ex1 ) || ( ex2 >= x1 && ex2 <= ex1 ) )
        if ( ( y2 >= y1 && y2 <= ey1 ) || ( ey2 >= y1 && ey2 <= ey1 ) )
            if ( ( z2 >= z1 && z2 <= ez1 ) || ( ez2 >= z1 && ez2 <= ez1 ) )
            {
                return true;
            }



    return false;
}



