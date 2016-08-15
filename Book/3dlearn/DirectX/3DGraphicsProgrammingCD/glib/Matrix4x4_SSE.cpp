
#include <stdio.h>
#include <stdlib.h>

#include "Matrix4x4.h"

Matrix4x4 g_MatrixSSE_Identity = Matrix4x4SSE
(
 1.0f, 0.0f, 0.0f, 0.0f,
 0.0f, 1.0f, 0.0f, 0.0f,
 0.0f, 0.0f, 1.0f, 0.0f,
 0.0f, 0.0f, 0.0f, 1.0f
 );

#define IEEE_SP_ZERO ((unsigned int) 0)
#define IEEE_SP_SIGN ((unsigned int) ( 1 << 31 )) 

Vector4SSE g_VectorSSE_quat2mat_x0(IEEE_SP_ZERO, IEEE_SP_SIGN, IEEE_SP_SIGN, IEEE_SP_SIGN );
Vector4SSE g_VectorSSE_quat2mat_x1(IEEE_SP_SIGN, IEEE_SP_ZERO, IEEE_SP_SIGN, IEEE_SP_SIGN );
Vector4SSE g_VectorSSE_quat2mat_x2(IEEE_SP_ZERO, IEEE_SP_SIGN, IEEE_SP_SIGN, IEEE_SP_SIGN );

void Matrix4x4SSE::ConsoleOutput(void)
{
	printf("Matrix:");
	printf("\t(%f,%f,%f,%f)\n", m_Vec0.x, m_Vec0.y, m_Vec0.z, m_Vec0.w);
	printf("\t(%f,%f,%f,%f)\n", m_Vec1.x, m_Vec1.y, m_Vec1.z, m_Vec1.w);
	printf("\t(%f,%f,%f,%f)\n", m_Vec2.x, m_Vec2.y, m_Vec2.z, m_Vec2.w);
	printf("\t(%f,%f,%f,%f)\n", m_Vec3.x, m_Vec3.y, m_Vec3.z, m_Vec3.w);
}

// from intel 
Matrix4x4SSE &Matrix4x4SSE::Invert(void)
{
	float *src = &m_Vec0[0];

	__m128 minor0, minor1, minor2, minor3;
	__m128 det;

	// fool compiler only..
	__m128 tmp1 = m_Vec0.m_Vec;
	__m128 row0 = m_Vec0.m_Vec;
	__m128 row1 = m_Vec1.m_Vec;
	__m128 row2 = m_Vec2.m_Vec;
	__m128 row3 = m_Vec3.m_Vec;

	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src)), (__m64*)(src+ 4));
	row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(src+8)), (__m64*)(src+12));
	row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
	row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src+ 2)), (__m64*)(src+ 6));
	row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(src+10)), (__m64*)(src+14));
	row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
	row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);

	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row2, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_mul_ps(row1, tmp1);
	minor1 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
	minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
	minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row1, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
	minor3 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
	minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
	// -----------------------------------------------	
	tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	row2 = _mm_shuffle_ps(row2, row2, 0x4E);
	minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
	minor2 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
	minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row1);	
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
	minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
	// -----------------------------------------------
	det = _mm_mul_ps(row0, minor0);
	det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
	det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
	tmp1 = _mm_rcp_ss(det);
	det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
	det = _mm_shuffle_ps(det, det, 0x00);
	minor0 = _mm_mul_ps(det, minor0);
	_mm_storel_pi((__m64*)(src), minor0);
	_mm_storeh_pi((__m64*)(src+2), minor0);
	minor1 = _mm_mul_ps(det, minor1);
	_mm_storel_pi((__m64*)(src+4), minor1);
	_mm_storeh_pi((__m64*)(src+6), minor1);
	minor2 = _mm_mul_ps(det, minor2);
	_mm_storel_pi((__m64*)(src+ 8), minor2);
	_mm_storeh_pi((__m64*)(src+10), minor2);
	minor3 = _mm_mul_ps(det, minor3);
	_mm_storel_pi((__m64*)(src+12), minor3);
	_mm_storeh_pi((__m64*)(src+14), minor3);

	return *this;
}

void Matrix4x4Rotate(Matrix4x4SSE &matrix, Vector4SSE &v, float radian)
{
	float fSin, fCos;
	FastMath::SinCos(radian, fSin, fCos);
	float fOneMinusCos = 1.0f - fCos;

	Vector4SSE xxxx = v.GetXXXX();
	Vector4SSE yyyy = v.GetYYYY();
	Vector4SSE zzzz = v.GetZZZZ();
	Vector4SSE SinVector(fSin);
	Vector4SSE v_Mul_Sin = v * SinVector;
	Vector4SSE multipler = fOneMinusCos * v; multipler.SetW(0.0f);
	Vector4SSE add;

	Matrix4x4SSE local;
	matrix.Identity();

	add.Set(fCos, v_Mul_Sin.GetZ(), -v_Mul_Sin.GetY(), 0.0f);
	matrix.m_Vec0 = xxxx * multipler + add;

	add.Set(-v_Mul_Sin.GetZ(), fCos, v_Mul_Sin.GetX(), 0.0f);
	matrix.m_Vec1 = yyyy * multipler + add;

	add.Set(v_Mul_Sin.GetY(), -v_Mul_Sin.GetX(), fCos, 0.0f);
	matrix.m_Vec2 = zzzz * multipler + add;

	matrix.m_Vec3 = g_MatrixSSE_Identity.m_Vec3;

	/*
	local.m_00 = ( x * x ) * fOneMinusCos + fCos;
	local.m_01 = ( x * y ) * fOneMinusCos + (z * fSin);
	local.m_02 = ( x * z ) * fOneMinusCos - (y * fSin);

	local.m_10 = ( y * x ) * fOneMinusCos - (z * fSin);
	local.m_11 = ( y * y ) * fOneMinusCos + fCos;
	local.m_12 = ( y * z ) * fOneMinusCos + (x * fSin);

	local.m_20 = ( z * x ) * fOneMinusCos + (y * fSin);
	local.m_21 = ( z * y ) * fOneMinusCos - (x * fSin);
	local.m_22 = ( z * z ) * fOneMinusCos + fCos;
	*/

}

#define R_SHUFFLE_PS( x, y, z, w ) (( (w) & 3 ) << 6 | ( (z) & 3 ) << 4 | ( (y) & 3 ) << 2 | ( (x) & 3 )) 
#define R_SHUFFLE_D( x, y, z, w ) (( (w) & 3 ) << 6 | ( (z) & 3 ) << 4 | ( (y) & 3 ) << 2 | ( (x) & 3 )) 

void Matrix4x4FromQuaternion(Matrix4x4SSE &matrix, Vector4SSE &v)
{
	Vector4SSE pq = v;
	Matrix4x4SSE *pm = &matrix;

	__asm { 
		mov edi, pm	// edi = &matrix
			movaps xmm0, [pq] // xmm0 = q.x, q.y, q.z, q.w 
		//movaps xmm6, [g_VectorSSE_One] // xmm6 = t.x, t.y, t.z, t.w 
		//movaps xmm6, [g_VectorSSE_Zero] // xmm6 = 0, 0, 0, 0
		movaps xmm1, xmm0 // xmm1 = x, y, z, w 
			addps xmm1, xmm1 // xmm1 = x2, y2, z2, w2 

#ifdef _ENABLE_SSE2_
			// SSE2 version
			pshufd xmm2, xmm0, R_SHUFFLE_D( 1, 0, 0, 1 ) // xmm2 = y, x, x, y 
			pshufd xmm3, xmm1, R_SHUFFLE_D( 1, 1, 2, 2 ) // xmm3 = y2, y2, z2, z2 
#else
			// SSE Version
			movaps xmm2, xmm0
			shufps xmm2, xmm0, R_SHUFFLE_PS( 1, 0, 0, 1 ) // xmm2 = y, x, x, y 
			movaps xmm3, xmm1
			shufps xmm3, xmm1, R_SHUFFLE_PS( 1, 1, 2, 2 ) // xmm3 = y2, y2, z2, z2 
#endif // _ENABLE_SSE2_

			mulps xmm2, xmm3 // xmm2 = yy2, xy2, xz2, yz2 

#ifdef _ENABLE_SSE2_
			// SSE2 version
			pshufd xmm4, xmm0, R_SHUFFLE_D( 2, 3, 3, 3 ) // xmm4 = z, w, w, w 
			pshufd xmm5, xmm1, R_SHUFFLE_D( 2, 2, 1, 0 ) // xmm5 = z2, z2, y2, x2 
#else
			// SSE version
			movaps xmm4, xmm0 
			shufps xmm4, xmm0, R_SHUFFLE_PS( 2, 3, 3, 3 ) // xmm4 = z, w, w, w 
			movaps xmm5, xmm1
			shufps xmm5, xmm1, R_SHUFFLE_PS( 2, 2, 1, 0 ) // xmm5 = z2, z2, y2, x2 
#endif // _ENABLE_SSE2_

			mulps xmm4, xmm5 // xmm4 = zz2, wz2, wy2, wx2 
			mulss xmm0, xmm1 // xmm0 = xx2, y2, z2, w2 // calculate the last two elements of the third row 
			movss xmm7, [g_VectorSSE_One] // xmm7 = 1, 0, 0, 0 
		subss xmm7, xmm0 // xmm7 = -xx2+1, 0, 0, 0 
			subss xmm7, xmm2 // xmm7 = -xx2-yy2+1, 0, 0, 0 
			//shufps xmm7, xmm6, R_SHUFFLE_PS( 0, 1, 2, 3 ) // xmm7 = -xx2-yy2+1, 0, t.z, t.w 
			// calculate first row 
			xorps xmm2, [g_VectorSSE_quat2mat_x0] // xmm2 = yy2, -xy2, -xz2, -yz2 
		xorps xmm4, [g_VectorSSE_quat2mat_x1] // xmm4 = -zz2, wz2, -wy2, -wx2 
		addss xmm4, [g_VectorSSE_One] // xmm4 = -zz2+1, wz2, -wy2, -wx2 
		movaps xmm3, xmm4 // xmm3 = -zz2+1, wz2, -wy2, -wx2 
			subps xmm3, xmm2 // xmm3 = -yy2-zz2+1, xy2+wz2, xz2-wy2, yz2-wx2 
			//movaps [matrix.m_Vec0], xmm3 // row0 = -yy2-zz2+1, xy2+wz2, xz2-wy2, yz2-wx2 
			//mov [matrix.m_Vec0.w], 0x00000000
			movaps [edi], xmm3 // row0 = -yy2-zz2+1, xy2+wz2, xz2-wy2, yz2-wx2 
			mov dword ptr [edi + 3*4], 0x00000000 // not working, why?
			//movss [edi + 3*4], xmm6
			// calculate second row 
			movss xmm2, xmm0 // xmm2 = xx2, -xy2, -xz2, -yz2 
			xorps xmm4, [g_VectorSSE_quat2mat_x2] // xmm4 = -zz2+1, -wz2, wy2, wx2 
		subps xmm4, xmm2 // xmm4 = -xx2-zz2+1, xy2-wz2, xz2+wy2, yz2+wx2 
			//shufps xmm6, xmm6, R_SHUFFLE_PS( 1, 2, 3, 0 ) // xmm6 = t.y, t.z, t.w, t.x 
			shufps xmm4, xmm4, R_SHUFFLE_PS( 1, 0, 3, 2 ) // xmm4 = xy2-wz2, -xx2-zz2+1, yz2+wx2, xz2+wy2 
			// movaps [matrix.m_Vec1], xmm4 // row1 = xy2-wz2, -xx2-zz2+1, yz2+wx2, xz2+wy2 
			// mov [matrix.m_Vec1.w], 0x00000000
			movaps [edi + 16], xmm4 // row1 = xy2-wz2, -xx2-zz2+1, yz2+wx2, xz2+wy2 
			mov dword ptr [edi + 16 + 3*4], 0x00000000
			//movss [edi + 16 + 3*4], xmm6
			//movss [edi+1*16+3*4], xmm6 // row1 = xy2-wz2, -xx2-zz2+1, yz2+wx2, t.y 
			// calculate third row 
			movhlps xmm3, xmm4 // xmm3 = yz2+wx2, xz2+wy2, xz2-wy2, yz2-wx2 
			shufps xmm3, xmm7, R_SHUFFLE_PS( 1, 3, 0, 2 ) // xmm3 = xz2+wy2, yz2-wx2, -xx2-yy2+1, t.z 
			//movaps [matrix.m_Vec2], xmm3 // row2 = xz2+wy2, yz2-wx2, -xx2-yy2+1, t.z 
			//mov [matrix.m_Vec2.w], 0x00000000
			movaps [edi + 32], xmm3 // row2 = xz2+wy2, yz2-wx2, -xx2-yy2+1, t.z 
			mov dword ptr [edi + 32 + 3*4], 0x00000000
			//movss [edi + 32 + 3*4], xmm6
	}

	matrix[3] = g_MatrixSSE_Identity[3];
}

void Matrix4x4FromQuaternionAndTransform(Matrix4x4SSE &matrix, Vector4SSE &q, Vector4SSE &t)
{	
	Vector4SSE pq = q;
	Matrix4x4SSE *pm = &matrix;

	__asm { 
		mov edi, pm	// edi = &matrix
			movaps xmm0, [pq] // xmm0 = q.x, q.y, q.z, q.w 
		//movaps xmm6, [g_VectorSSE_One] // xmm6 = t.x, t.y, t.z, t.w 
		//movaps xmm6, [g_VectorSSE_Zero] // xmm6 = 0, 0, 0, 0
		movaps xmm1, xmm0 // xmm1 = x, y, z, w 
			addps xmm1, xmm1 // xmm1 = x2, y2, z2, w2 

#ifdef _ENABLE_SSE2_
			// SSE2 version
			pshufd xmm2, xmm0, R_SHUFFLE_D( 1, 0, 0, 1 ) // xmm2 = y, x, x, y 
			pshufd xmm3, xmm1, R_SHUFFLE_D( 1, 1, 2, 2 ) // xmm3 = y2, y2, z2, z2 
#else
			// SSE Version
			movaps xmm2, xmm0
			shufps xmm2, xmm0, R_SHUFFLE_PS( 1, 0, 0, 1 ) // xmm2 = y, x, x, y 
			movaps xmm3, xmm1
			shufps xmm3, xmm1, R_SHUFFLE_PS( 1, 1, 2, 2 ) // xmm3 = y2, y2, z2, z2 
#endif // _ENABLE_SSE2_

			mulps xmm2, xmm3 // xmm2 = yy2, xy2, xz2, yz2 

#ifdef _ENABLE_SSE2_
			// SSE2 version
			pshufd xmm4, xmm0, R_SHUFFLE_D( 2, 3, 3, 3 ) // xmm4 = z, w, w, w 
			pshufd xmm5, xmm1, R_SHUFFLE_D( 2, 2, 1, 0 ) // xmm5 = z2, z2, y2, x2 
#else
			// SSE version
			movaps xmm4, xmm0 
			shufps xmm4, xmm0, R_SHUFFLE_PS( 2, 3, 3, 3 ) // xmm4 = z, w, w, w 
			movaps xmm5, xmm1
			shufps xmm5, xmm1, R_SHUFFLE_PS( 2, 2, 1, 0 ) // xmm5 = z2, z2, y2, x2 
#endif // _ENABLE_SSE2_

			mulps xmm4, xmm5 // xmm4 = zz2, wz2, wy2, wx2 
			mulss xmm0, xmm1 // xmm0 = xx2, y2, z2, w2 // calculate the last two elements of the third row 
			movss xmm7, [g_VectorSSE_One] // xmm7 = 1, 0, 0, 0 
		subss xmm7, xmm0 // xmm7 = -xx2+1, 0, 0, 0 
			subss xmm7, xmm2 // xmm7 = -xx2-yy2+1, 0, 0, 0 
			//shufps xmm7, xmm6, R_SHUFFLE_PS( 0, 1, 2, 3 ) // xmm7 = -xx2-yy2+1, 0, t.z, t.w 
			// calculate first row 
			xorps xmm2, [g_VectorSSE_quat2mat_x0] // xmm2 = yy2, -xy2, -xz2, -yz2 
		xorps xmm4, [g_VectorSSE_quat2mat_x1] // xmm4 = -zz2, wz2, -wy2, -wx2 
		addss xmm4, [g_VectorSSE_One] // xmm4 = -zz2+1, wz2, -wy2, -wx2 
		movaps xmm3, xmm4 // xmm3 = -zz2+1, wz2, -wy2, -wx2 
			subps xmm3, xmm2 // xmm3 = -yy2-zz2+1, xy2+wz2, xz2-wy2, yz2-wx2 
			//movaps [matrix.m_Vec0], xmm3 // row0 = -yy2-zz2+1, xy2+wz2, xz2-wy2, yz2-wx2 
			//mov [matrix.m_Vec0.w], 0x00000000
			movaps [edi], xmm3 // row0 = -yy2-zz2+1, xy2+wz2, xz2-wy2, yz2-wx2 
			mov dword ptr [edi + 3*4], 0x00000000 // not working, why?
			//movss [edi + 3*4], xmm6
			// calculate second row 
			movss xmm2, xmm0 // xmm2 = xx2, -xy2, -xz2, -yz2 
			xorps xmm4, [g_VectorSSE_quat2mat_x2] // xmm4 = -zz2+1, -wz2, wy2, wx2 
		subps xmm4, xmm2 // xmm4 = -xx2-zz2+1, xy2-wz2, xz2+wy2, yz2+wx2 
			//shufps xmm6, xmm6, R_SHUFFLE_PS( 1, 2, 3, 0 ) // xmm6 = t.y, t.z, t.w, t.x 
			shufps xmm4, xmm4, R_SHUFFLE_PS( 1, 0, 3, 2 ) // xmm4 = xy2-wz2, -xx2-zz2+1, yz2+wx2, xz2+wy2 
			// movaps [matrix.m_Vec1], xmm4 // row1 = xy2-wz2, -xx2-zz2+1, yz2+wx2, xz2+wy2 
			// mov [matrix.m_Vec1.w], 0x00000000
			movaps [edi + 16], xmm4 // row1 = xy2-wz2, -xx2-zz2+1, yz2+wx2, xz2+wy2 
			mov dword ptr [edi + 16 + 3*4], 0x00000000
			//movss [edi + 16 + 3*4], xmm6
			//movss [edi+1*16+3*4], xmm6 // row1 = xy2-wz2, -xx2-zz2+1, yz2+wx2, t.y 
			// calculate third row 
			movhlps xmm3, xmm4 // xmm3 = yz2+wx2, xz2+wy2, xz2-wy2, yz2-wx2 
			shufps xmm3, xmm7, R_SHUFFLE_PS( 1, 3, 0, 2 ) // xmm3 = xz2+wy2, yz2-wx2, -xx2-yy2+1, t.z 
			//movaps [matrix.m_Vec2], xmm3 // row2 = xz2+wy2, yz2-wx2, -xx2-yy2+1, t.z 
			//mov [matrix.m_Vec2.w], 0x00000000
			movaps [edi + 32], xmm3 // row2 = xz2+wy2, yz2-wx2, -xx2-yy2+1, t.z 
			mov dword ptr [edi + 32 + 3*4], 0x00000000
			//movss [edi + 32 + 3*4], xmm6
	}

	matrix.m_Vec3 = t;
}

void QuaternionFromMatrix4x4(Matrix4x4SSE &matrix, Vector4SSE &quaternion)
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
