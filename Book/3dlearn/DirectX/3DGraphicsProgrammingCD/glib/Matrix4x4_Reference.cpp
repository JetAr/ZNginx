#include <stdio.h>
#include <stdlib.h>

#include "Matrix4x4.h"

Matrix4x4CPU g_MatrixCPU_Identity = Matrix4x4CPU
(
 1.0f, 0.0f, 0.0f, 0.0f,
 0.0f, 1.0f, 0.0f, 0.0f,
 0.0f, 0.0f, 1.0f, 0.0f,
 0.0f, 0.0f, 0.0f, 1.0f
 );

#define IEEE_SP_ZERO ((unsigned int) 0)
#define IEEE_SP_SIGN ((unsigned int) ( 1 << 31 )) 

void Matrix4x4CPU::ConsoleOutput(void)
{
	printf("Matrix:");
	printf("\t(%5.2f,%5.2f,%5.2f,%5.2f)\n", m_Vec0.x, m_Vec0.y, m_Vec0.z, m_Vec0.w);
	printf("\t(%5.2f,%5.2f,%5.2f,%5.2f)\n", m_Vec1.x, m_Vec1.y, m_Vec1.z, m_Vec1.w);
	printf("\t(%5.2f,%5.2f,%5.2f,%5.2f)\n", m_Vec2.x, m_Vec2.y, m_Vec2.z, m_Vec2.w);
	printf("\t(%5.2f,%5.2f,%5.2f,%5.2f)\n", m_Vec3.x, m_Vec3.y, m_Vec3.z, m_Vec3.w);
}

inline float MINOR(Matrix4x4CPU &m, const int r0, const int r1, const int r2, const int c0, const int c1, const int c2)
{
	return 
		m(r0,c0) * (m(r1,c1) * m(r2,c2) - m(r2,c1) * m(r1,c2)) -
		m(r0,c1) * (m(r1,c0) * m(r2,c2) - m(r2,c0) * m(r1,c2)) +
		m(r0,c2) * (m(r1,c0) * m(r2,c1) - m(r2,c0) * m(r1,c1));
}


Matrix4x4CPU Matrix4x4_Adjoint(Matrix4x4CPU &m)
{
	Matrix4x4CPU ma
		( 
		MINOR(m, 1, 2, 3, 1, 2, 3),
		-MINOR(m, 0, 2, 3, 1, 2, 3),
		MINOR(m, 0, 1, 3, 1, 2, 3),
		-MINOR(m, 0, 1, 2, 1, 2, 3),

		-MINOR(m, 1, 2, 3, 0, 2, 3),
		MINOR(m, 0, 2, 3, 0, 2, 3),
		-MINOR(m, 0, 1, 3, 0, 2, 3),
		MINOR(m, 0, 1, 2, 0, 2, 3),

		MINOR(m, 1, 2, 3, 0, 1, 3),
		-MINOR(m, 0, 2, 3, 0, 1, 3),
		MINOR(m, 0, 1, 3, 0, 1, 3),
		-MINOR(m, 0, 1, 2, 0, 1, 3),

		-MINOR(m, 1, 2, 3, 0, 1, 2),
		MINOR(m, 0, 2, 3, 0, 1, 2),
		-MINOR(m, 0, 1, 3, 0, 1, 2),
		MINOR(m, 0, 1, 2, 0, 1, 2)
		);

	return ma;
}

float Matrix4x4_Determinant(Matrix4x4CPU &m) 
{
	return 
		m.m_00 * MINOR(m, 1, 2, 3, 1, 2, 3) -
		m.m_01 * MINOR(m, 1, 2, 3, 0, 2, 3) +
		m.m_02 * MINOR(m, 1, 2, 3, 0, 1, 3) -
		m.m_03 * MINOR(m, 1, 2, 3, 0, 1, 2);
}

Matrix4x4CPU &Matrix4x4CPU::Invert()
{
	Matrix4x4CPU m = (1.0f / Matrix4x4_Determinant(*this)) * Matrix4x4_Adjoint(*this);
	*this = m;
	return *this;
}

void Matrix4x4Rotate(Matrix4x4CPU &matrix, Vector4CPU &v, float radian)
{
	float x = v.x;
	float y = v.y;
	float z = v.z;

	float fSin, fCos;
	FastMath::SinCos(radian, fSin, fCos);
	float fOneMinusCos = 1.0f - fCos;

	matrix.m_00 = ( x * x ) * fOneMinusCos + fCos;
	matrix.m_01 = ( x * y ) * fOneMinusCos + (z * fSin);
	matrix.m_02 = ( x * z ) * fOneMinusCos - (y * fSin);
	matrix.m_03 = 0.0f;

	matrix.m_10 = ( y * x ) * fOneMinusCos - (z * fSin);
	matrix.m_11 = ( y * y ) * fOneMinusCos + fCos;
	matrix.m_12 = ( y * z ) * fOneMinusCos + (x * fSin);
	matrix.m_13 = 0.0f;

	matrix.m_20 = ( z * x ) * fOneMinusCos + (y * fSin);
	matrix.m_21 = ( z * y ) * fOneMinusCos - (x * fSin);
	matrix.m_22 = ( z * z ) * fOneMinusCos + fCos;
	matrix.m_23 = 0.0f;

	matrix.m_Vec3 = g_MatrixCPU_Identity.m_Vec3;
}

void Matrix4x4FromQuaternionAndTransform(Matrix4x4CPU &mat, Vector4CPU &v, Vector4CPU &t)
{	
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = v.x + v.x;
	y2 = v.y + v.y;
	z2 = v.z + v.z;
	xx = v.x * x2;
	xy = v.x * y2;
	xz = v.x * z2;
	yy = v.y * y2;
	yz = v.y * z2;
	zz = v.z * z2;
	wx = v.w * x2;
	wy = v.w * y2;
	wz = v.w * z2;

	mat.m_00 = 1.0f - (yy + zz);
	mat.m_10 = xy - wz;
	mat.m_20 = xz + wy;
	mat.m_30 = 0.0f;

	mat.m_01 = xy + wz;
	mat.m_11 = 1.0f - (xx + zz);
	mat.m_21 = yz - wx;
	mat.m_31 = 0.0f;

	mat.m_02 = xz - wy;
	mat.m_12 = yz + wx;
	mat.m_22 = 1.0f - (xx + yy);
	mat.m_32 = 0.0f;

	mat.m_Vec3 = t;
}

void QuaternionFromMatrix4x4(Matrix4x4CPU &matrix, Vector4CPU &quaternion)
{
	// not optimized yet
	float *q = &quaternion.x; 
	const float *m = &matrix.m_00;

	if ( m[0 * 4 + 0] + m[1 * 4 + 1] + m[2 * 4 + 2] > 0.0f ) 
	{ 
		float t = + m[0 * 4 + 0] + m[1 * 4 + 1] + m[2 * 4 + 2] + 1.0f; 
		float s = FastMath::ReciprocalSqrt( t ) * 0.5f; 
		q[3] = s * t; 
		q[2] = ( m[0 * 4 + 1] - m[1 * 4 + 0] ) * s; 
		q[1] = ( m[2 * 4 + 0] - m[0 * 4 + 2] ) * s; 
		q[0] = ( m[1 * 4 + 2] - m[2 * 4 + 1] ) * s; 
	} 
	else if ( m[0 * 4 + 0] > m[1 * 4 + 1] && m[0 * 4 + 0] > m[2 * 4 + 2] ) 
	{ 
		float t = + m[0 * 4 + 0] - m[1 * 4 + 1] - m[2 * 4 + 2] + 1.0f; 
		float s = FastMath::ReciprocalSqrt( t ) * 0.5f; 
		q[0] = s * t; 
		q[1] = ( m[0 * 4 + 1] + m[1 * 4 + 0] ) * s; 

		q[2] = ( m[2 * 4 + 0] + m[0 * 4 + 2] ) * s; 
		q[3] = ( m[1 * 4 + 2] - m[2 * 4 + 1] ) * s; 
	} 
	else if ( m[1 * 4 + 1] > m[2 * 4 + 2] ) 
	{ 
		float t = - m[0 * 4 + 0] + m[1 * 4 + 1] - m[2 * 4 + 2] + 1.0f; 
		float s = FastMath::ReciprocalSqrt( t ) * 0.5f; 
		q[1] = s * t; 
		q[0] = ( m[0 * 4 + 1] + m[1 * 4 + 0] ) * s; 
		q[3] = ( m[2 * 4 + 0] - m[0 * 4 + 2] ) * s; 
		q[2] = ( m[1 * 4 + 2] + m[2 * 4 + 1] ) * s; 
	} 
	else 
	{ 
		float t = - m[0 * 4 + 0] - m[1 * 4 + 1] + m[2 * 4 + 2] + 1.0f; 
		float s = FastMath::ReciprocalSqrt( t ) * 0.5f; 
		q[2] = s * t; 
		q[3] = ( m[0 * 4 + 1] - m[1 * 4 + 0] ) * s; 
		q[0] = ( m[2 * 4 + 0] + m[0 * 4 + 2] ) * s; 
		q[1] = ( m[1 * 4 + 2] + m[2 * 4 + 1] ) * s; 
	} 
}

void Matrix4x4FromQuaternion(Matrix4x4CPU &mat, Vector4CPU &v)
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = v.x + v.x;
	y2 = v.y + v.y;
	z2 = v.z + v.z;
	xx = v.x * x2;
	xy = v.x * y2;
	xz = v.x * z2;
	yy = v.y * y2;
	yz = v.y * z2;
	zz = v.z * z2;
	wx = v.w * x2;
	wy = v.w * y2;
	wz = v.w * z2;

	mat.m_00 = 1.0f - (yy + zz);
	mat.m_10 = xy - wz;
	mat.m_20 = xz + wy;
	mat.m_30 = 0.0f;

	mat.m_01 = xy + wz;
	mat.m_11 = 1.0f - (xx + zz);
	mat.m_21 = yz - wx;
	mat.m_31 = 0.0f;

	mat.m_02 = xz - wy;
	mat.m_12 = yz + wx;
	mat.m_22 = 1.0f - (xx + yy);
	mat.m_32 = 0.0f;

	mat.m_03 = 0.0f;
	mat.m_13 = 0.0f;
	mat.m_23 = 0.0f;
	mat.m_33 = 1.0f;
}
