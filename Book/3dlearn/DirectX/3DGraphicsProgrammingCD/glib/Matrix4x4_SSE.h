#ifndef _MATRIX4x4_SSE_
#define _MATRIX4x4_SSE_

#include "Vector4_SSE.h"

class Matrix4x4SSE;

__declspec(align(16)) extern Matrix4x4SSE g_MatrixSSE_Identity;
__declspec(align(16)) extern Vector4SSE g_VectorSSE_quat2mat_x0;
__declspec(align(16)) extern Vector4SSE g_VectorSSE_quat2mat_x1;
__declspec(align(16)) extern Vector4SSE g_VectorSSE_quat2mat_x2;

inline Vector4SSE operator*(Vector4SSE &v, Matrix4x4SSE &matrix);
inline Matrix4x4SSE operator*(Matrix4x4SSE &a, Matrix4x4SSE &b);

class Matrix4x4SSE
{
public:
	union
	{
		struct
		{
			float m_00, m_01, m_02, m_03;
			float m_10, m_11, m_12, m_13;
			float m_20, m_21, m_22, m_23;
			float m_30, m_31, m_32, m_33;
		};

		struct
		{
			Vector4SSE m_Vec0, m_Vec1, m_Vec2, m_Vec3;
		};
	};

public:
	inline Matrix4x4SSE()
	{

	}

	inline Matrix4x4SSE(
		float f00, float f01, float f02, float f03,
		float f10, float f11, float f12, float f13,
		float f20, float f21, float f22, float f23,
		float f30, float f31, float f32, float f33
		)	
	{
		m_00 = f00; m_01 = f01; m_02 = f02; m_03 = f03;
		m_10 = f10; m_11 = f11; m_12 = f12; m_13 = f13;
		m_20 = f20; m_21 = f21; m_22 = f22; m_23 = f23;
		m_30 = f30; m_31 = f31; m_32 = f32; m_33 = f33;
	}

	inline void Identity(void)
	{
		/*
		*this = g_MatrixSSE_Identity;
		*/
		m_Vec0.Set(1.0f, 0.0f, 0.0f, 0.0f);
		m_Vec1.Set(0.0f, 1.0f, 0.0f, 0.0f);
		m_Vec2.Set(0.0f, 0.0f, 1.0f, 0.0f);
		m_Vec3.Set(0.0f, 0.0f, 0.0f, 1.0f);
	}

	static Matrix4x4SSE IdentityMatrix(void)
	{
		return g_MatrixSSE_Identity;
	}

	inline void SetRow(int row, Vector4SSE &vec)
	{
		assert(row>=0 && row<4);
		Vector4SSE *rows = &m_Vec0;
		rows[row] = vec;
	}

	inline void SetColumn(int column, Vector4SSE &vec)
	{
		assert(column>=0 && column<4);
		float *p = &m_00 + column;
		p[0] = vec.GetX(); 
		p[4] = vec.GetY(); 
		p[8] = vec.GetZ(); 
		p[12] = vec.GetW(); 
	}

	inline Vector4SSE &GetRow(int row)
	{
		assert(row>=0 && row<4);
		Vector4SSE *rows = &m_Vec0;
		return rows[row];
	}

	inline Vector4SSE GetColumn(int column)
	{
		assert(column>=0 && column<4);
		Vector4SSE vec;
		float *p = &m_00 + column;

		vec.SetX(p[0]);
		vec.SetY(p[4]);
		vec.SetZ(p[8]);
		vec.SetW(p[12]);

		return vec;
	}

	// assume input vec.w equals 1
	// result.w is garbage
	inline Vector4SSE TransformVec3(Vector4SSE &vec)
	{
		Vector4SSE result;

		__m128 vxxxx = _mm_shuffle_ps(vec.m_Vec, vec.m_Vec, _MM_SHUFFLE(0, 0, 0, 0) );
		__m128 vyyyy = _mm_shuffle_ps(vec.m_Vec, vec.m_Vec, _MM_SHUFFLE(1, 1, 1, 1) );
		__m128 vzzzz = _mm_shuffle_ps(vec.m_Vec, vec.m_Vec, _MM_SHUFFLE(2, 2, 2, 2) );

		__m128 temp;

		result.m_Vec = _mm_mul_ps(vxxxx, m_Vec0.m_Vec);	

		temp = _mm_mul_ps(vyyyy, m_Vec1.m_Vec);
		result.m_Vec = _mm_add_ps(result.m_Vec, temp);

		temp = _mm_mul_ps(vzzzz, m_Vec2.m_Vec);
		result.m_Vec = _mm_add_ps(result.m_Vec, temp);

		result.m_Vec = _mm_add_ps(result.m_Vec, m_Vec3.m_Vec);

		return result;
	}

	// this = R * this
	void Rotate( Vector4SSE &v, float radian)
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
		local.Identity();

		add.Set(fCos, v_Mul_Sin.GetZ(), -v_Mul_Sin.GetY(), 0.0f);
		local.m_Vec0 = xxxx * multipler + add;

		add.Set(-v_Mul_Sin.GetZ(), fCos, v_Mul_Sin.GetX(), 0.0f);
		local.m_Vec1 = yyyy * multipler + add;

		add.Set(v_Mul_Sin.GetY(), -v_Mul_Sin.GetX(), fCos, 0.0f);
		local.m_Vec2 = zzzz * multipler + add;

		*this = local * (*this);
	}

	// replace current matrix
	void Rotate_Replace( Vector4CPU &v, float radian)
	{
		float x = v.x;
		float y = v.y;
		float z = v.z;
		float w = v.w;

		float fSin, fCos;
		FastMath::SinCos(radian, fSin, fCos);

		m_00 = ( x * x ) * ( 1.0f - fCos ) + fCos;
		m_01 = ( x * y ) * ( 1.0f - fCos ) + (z * fSin);
		m_02 = ( x * z ) * ( 1.0f - fCos ) - (y * fSin);
		m_03 = 0.0f;

		m_10 = ( y * x ) * ( 1.0f - fCos ) - (z * fSin);
		m_11 = ( y * y ) * ( 1.0f - fCos ) + fCos ;
		m_12 = ( y * z ) * ( 1.0f - fCos ) + (x * fSin);
		m_13 = 0.0f;

		m_20 = ( z * x ) * ( 1.0f - fCos ) + (y * fSin);
		m_21 = ( z * y ) * ( 1.0f - fCos ) - (x * fSin);
		m_22 = ( z * z ) * ( 1.0f - fCos ) + fCos;
		m_23 = 0.0f;

		m_30 = 0.0f;
		m_31 = 0.0f;
		m_32 = 0.0f;
		m_33 = 1.0f;
	}

	// this = Rx * this
	void RotateX( const float radian )
	{
		float Sin, Cos;
		FastMath::SinCos(radian, Sin, Cos);

		Vector4SSE SinVector(Sin);
		Vector4SSE CosVector(Cos);

		Vector4SSE t0 = m_Vec1 * CosVector + m_Vec2 * SinVector;
		Vector4SSE t1 = m_Vec2 * CosVector - m_Vec1 * SinVector;

		m_Vec1 = t0;
		m_Vec2 = t1;
	}

	// replace current matrix
	void RotateX_Replace( const float radian )
	{
		float fSin, fCos;
		FastMath::SinCos(radian, fSin, fCos);

		m_Vec0.Set(1.0f,  0.0f,  0.0f, 0.0f);
		m_Vec1.Set(0.0f,  fCos,  fSin, 0.0f);
		m_Vec2.Set(0.0f, -fSin,  fCos, 0.0f);
		m_Vec3.Set(0.0f,  0.0f,  0.0f, 1.0f);
	}

	// this = Ry * this
	void RotateY( const float radian )
	{
		float Sin, Cos;
		FastMath::SinCos(radian, Sin, Cos);

		Vector4SSE SinVector(Sin);
		Vector4SSE CosVector(Cos);

		Vector4SSE t0 = m_Vec0 * CosVector - m_Vec2 * SinVector;
		Vector4SSE t1 = m_Vec0 * SinVector + m_Vec2 * CosVector;

		m_Vec0 = t0;
		m_Vec2 = t1;
	}

	// replace current matrix
	void RotateY_Replace( const float radian )
	{
		float fSin, fCos;
		FastMath::SinCos(radian, fSin, fCos);

		m_Vec0.Set( fCos,  0.0f,-fSin, 0.0f);
		m_Vec1.Set( 0.0f,  1.0f, 0.0f, 0.0f);
		m_Vec2.Set( fSin,  0.0f, fCos, 0.0f);
		m_Vec3.Set( 0.0f,  0.0f, 0.0f, 1.0f);
	}

	// this = Rz * this
	void RotateZ( const float radian )
	{
		float Sin, Cos;
		FastMath::SinCos(radian, Sin, Cos);

		Vector4SSE SinVector(Sin);
		Vector4SSE CosVector(Cos);

		Vector4SSE t0 = m_Vec0 * CosVector + m_Vec1 * SinVector;
		Vector4SSE t1 = m_Vec1 * CosVector - m_Vec0 * SinVector;

		m_Vec0 = t0;
		m_Vec1 = t1;
	}

	// replace current matrix
	void RotateZ_Replace( const float radian )
	{
		float fSin, fCos;
		FastMath::SinCos(radian, fSin, fCos);

		m_Vec0.Set( fCos,  fSin, 0.0f, 0.0f);
		m_Vec1.Set(-fSin,  fCos, 0.0f, 0.0f);
		m_Vec2.Set( 0.0f,  0.0f, 1.0f, 0.0f);
		m_Vec3.Set( 0.0f,  0.0f, 0.0f, 1.0f);
	}

	void Scale(Vector4SSE &scale)
	{
		m_Vec0 *= scale;
		m_Vec1 *= scale;
		m_Vec2 *= scale;
		m_Vec3 *= scale;
	}

	void Scale(float x, float y, float z)
	{
		Vector4 vScale(x, y, z, 1.0f);
		Scale(vScale);
		//Matrix4x4 scale_replace(x, y, z);
		//*this = *this * scale;
	}

	void Scale_Replace(Vector4SSE &scale)
	{
		Scale_Replace(scale[0], scale[1], scale[2]);
	}

	void Scale_Replace(float x, float y, float z)
	{
		m_Vec0.Set(x, 0, 0, 0);
		m_Vec1.Set(0, y, 0, 0);
		m_Vec2.Set(0, 0, z, 0);
		m_Vec3.Set(0, 0, 0, 1);
	}

	inline Matrix4x4SSE &FastInvert(void)
	{
		Vector4SSE pos = VectorInvertSign(m_Vec3);
		m_Vec3 = g_MatrixSSE_Identity.m_Vec3;

		Transpose();
		Translate(pos);

		return *this;
	}

	Matrix4x4SSE &Invert(void);

	inline Matrix4x4SSE &Transpose(void)
	{
		_MM_TRANSPOSE4_PS(m_Vec0.m_Vec, m_Vec1.m_Vec, m_Vec2.m_Vec, m_Vec3.m_Vec);

		return *this;
	}

	// this = T * this
	void Translate(float x, float y, float z)
	{
		Vector4SSE xxxx(x);
		Vector4SSE yyyy(y);
		Vector4SSE zzzz(z);

		m_Vec3 += m_Vec0 * xxxx + m_Vec1 * yyyy + m_Vec2 * zzzz;
	}

	// this = T * this
	void Translate(Vector4SSE &v)
	{
		Vector4SSE xxxx = v.GetXXXX();
		Vector4SSE yyyy = v.GetYYYY();
		Vector4SSE zzzz = v.GetZZZZ();

		m_Vec3 += m_Vec0 * xxxx + m_Vec1 * yyyy + m_Vec2 * zzzz;
	}

	void Translate_Replace(float x, float y, float z)
	{
		Identity();

		m_Vec3[0] = x;
		m_Vec3[1] = y;
		m_Vec3[2] = z;
	}

	void Translate_Replace(Vector4SSE &v)
	{
		Identity();

		m_Vec3 = v;
	}

	void TranslateX(float d)
	{
		Vector4SSE dddd(d);
		m_Vec3 += m_Vec0 * dddd;
	}

	void TranslateY(float d)
	{
		Vector4SSE dddd(d);
		m_Vec3 += m_Vec1 * dddd;
	}

	void TranslateZ(float d)
	{
		Vector4SSE dddd(d);
		m_Vec3 += m_Vec2 * dddd;
	}

	Vector4SSE RotateVector(Vector4SSE &vec)
	{
		Vector4SSE result;

		result  = vec.GetXXXX() * m_Vec0;
		result += vec.GetYYYY() * m_Vec1;
		result += vec.GetZZZZ() * m_Vec2;

		return result;
	}

	void NoTranslate(void)
	{
		m_Vec3 = IdentityMatrix()[3];
	}

	void NoRotate(void)
	{
		m_Vec0 = IdentityMatrix()[0];
		m_Vec1 = IdentityMatrix()[1];
		m_Vec2 = IdentityMatrix()[2];
	}

	void SetMatrix3x4(Matrix4x4SSE &mat)
	{
		m_Vec0 = mat[0];
		m_Vec1 = mat[1];
		m_Vec2 = mat[2];
	}

	void ConsoleOutput(void);

	// operator
	inline float &operator() (int i, int j)	
	{ 
		assert(i>=0 && i<4);
		assert(j>=0 && j<4);
		return GetRow(i)[j];
	}

	inline Vector4SSE &operator[](int row)
	{
		return GetRow(row);
	}

	inline Matrix4x4SSE &operator=(Matrix4x4SSE &rhs)
	{
		m_Vec0 = rhs.m_Vec0;
		m_Vec1 = rhs.m_Vec1;
		m_Vec2 = rhs.m_Vec2;
		m_Vec3 = rhs.m_Vec3;

		return *this;
	}

	inline Matrix4x4SSE &operator=(float f)
	{
		__m128 ffff = _mm_set_ps1(f);

		m_Vec0.m_Vec = ffff;
		m_Vec1.m_Vec = ffff;
		m_Vec2.m_Vec = ffff;
		m_Vec3.m_Vec = ffff;

		return *this;
	}

	inline Matrix4x4SSE &operator+=(float f)
	{
		__m128 ffff = _mm_set_ps1(f);

		m_Vec0.m_Vec = _mm_add_ps(m_Vec0.m_Vec, ffff);
		m_Vec1.m_Vec = _mm_add_ps(m_Vec1.m_Vec, ffff);
		m_Vec2.m_Vec = _mm_add_ps(m_Vec2.m_Vec, ffff);
		m_Vec3.m_Vec = _mm_add_ps(m_Vec3.m_Vec, ffff);

		return *this;
	}

	inline Matrix4x4SSE &operator+=(Matrix4x4SSE &rhs)
	{
		m_Vec0.m_Vec = _mm_add_ps(m_Vec0.m_Vec, rhs.m_Vec0.m_Vec);
		m_Vec1.m_Vec = _mm_add_ps(m_Vec1.m_Vec, rhs.m_Vec1.m_Vec);
		m_Vec2.m_Vec = _mm_add_ps(m_Vec2.m_Vec, rhs.m_Vec2.m_Vec);
		m_Vec3.m_Vec = _mm_add_ps(m_Vec3.m_Vec, rhs.m_Vec3.m_Vec);

		return *this;
	}

	inline Matrix4x4SSE &operator-=(float f)
	{
		__m128 ffff = _mm_set_ps1(f);

		m_Vec0.m_Vec = _mm_sub_ps(m_Vec0.m_Vec, ffff);
		m_Vec1.m_Vec = _mm_sub_ps(m_Vec1.m_Vec, ffff);
		m_Vec2.m_Vec = _mm_sub_ps(m_Vec2.m_Vec, ffff);
		m_Vec3.m_Vec = _mm_sub_ps(m_Vec3.m_Vec, ffff);

		return *this;
	}

	inline Matrix4x4SSE &operator-=(Matrix4x4SSE &rhs)
	{
		m_Vec0.m_Vec = _mm_sub_ps(m_Vec0.m_Vec, rhs.m_Vec0.m_Vec);
		m_Vec1.m_Vec = _mm_sub_ps(m_Vec1.m_Vec, rhs.m_Vec1.m_Vec);
		m_Vec2.m_Vec = _mm_sub_ps(m_Vec2.m_Vec, rhs.m_Vec2.m_Vec);
		m_Vec3.m_Vec = _mm_sub_ps(m_Vec3.m_Vec, rhs.m_Vec3.m_Vec);

		return *this;
	}

	inline Matrix4x4SSE &operator*=(float f)
	{
		__m128 ffff = _mm_set_ps1(f);

		m_Vec0.m_Vec = _mm_mul_ps(m_Vec0.m_Vec, ffff);
		m_Vec1.m_Vec = _mm_mul_ps(m_Vec1.m_Vec, ffff);
		m_Vec2.m_Vec = _mm_mul_ps(m_Vec2.m_Vec, ffff);
		m_Vec3.m_Vec = _mm_mul_ps(m_Vec3.m_Vec, ffff);

		return *this;
	}

	inline Matrix4x4SSE &operator*=(Matrix4x4SSE &rhs)
	{
		m_Vec0 = m_Vec0 * rhs;
		m_Vec1 = m_Vec1 * rhs;
		m_Vec2 = m_Vec2 * rhs;
		m_Vec3 = m_Vec3 * rhs;

		return *this;
	}

	inline Matrix4x4SSE &operator/=(float f)
	{
		__m128 ffff = _mm_set_ps1(f);

		m_Vec0.m_Vec = _mm_div_ps(m_Vec0.m_Vec, ffff);
		m_Vec1.m_Vec = _mm_div_ps(m_Vec1.m_Vec, ffff);
		m_Vec2.m_Vec = _mm_div_ps(m_Vec2.m_Vec, ffff);
		m_Vec3.m_Vec = _mm_div_ps(m_Vec3.m_Vec, ffff);

		return *this;
	}

};

inline bool operator==(Matrix4x4SSE &a, Matrix4x4SSE &b)
{
	// true if all vectors equal to each other
	bool result = a.m_Vec0==b.m_Vec0 && a.m_Vec1==b.m_Vec1 && a.m_Vec2==b.m_Vec2 && a.m_Vec3==b.m_Vec3;
	return result;
}

inline bool operator!=(Matrix4x4SSE &a, Matrix4x4SSE &b)
{
	// true if any one vector not-equal
	bool result = a.m_Vec0!=b.m_Vec0 || a.m_Vec1!=b.m_Vec1 || a.m_Vec2!=b.m_Vec2 || a.m_Vec3!=b.m_Vec3;
	return result;
}

inline Matrix4x4SSE operator+(Matrix4x4SSE &a, float f)
{
	Matrix4x4SSE result = a;
	result += f;
	return result;
}

inline Matrix4x4SSE operator+(float f, Matrix4x4SSE &a)
{
	Matrix4x4SSE result = a;
	result += f;
	return result;
}

inline Matrix4x4SSE operator+(Matrix4x4SSE &a, Matrix4x4SSE &b)
{
	Matrix4x4SSE result;

	result.m_Vec0 = a.m_Vec0 + b.m_Vec0;
	result.m_Vec1 = a.m_Vec1 + b.m_Vec1;
	result.m_Vec2 = a.m_Vec2 + b.m_Vec2;
	result.m_Vec3 = a.m_Vec3 + b.m_Vec3;

	return result;
}

inline Matrix4x4SSE operator-(Matrix4x4SSE &a, float f)
{
	Matrix4x4SSE result = a;
	result -= f;
	return result;
}

inline Matrix4x4SSE operator-(float f, Matrix4x4SSE &a)
{
	Matrix4x4SSE result = a;
	result -= f;
	return result;
}

inline Matrix4x4SSE operator-(Matrix4x4SSE &a, Matrix4x4SSE &b)
{
	Matrix4x4SSE result;

	result.m_Vec0 = a.m_Vec0 - b.m_Vec0;
	result.m_Vec1 = a.m_Vec1 - b.m_Vec1;
	result.m_Vec2 = a.m_Vec2 - b.m_Vec2;
	result.m_Vec3 = a.m_Vec3 - b.m_Vec3;

	return result;
}


inline Vector4SSE operator*(Vector4SSE &vec, Matrix4x4SSE &matrix)
{
	/*
	Vector4SSE result;

	__m128 vxxxx = _mm_shuffle_ps(vec.m_Vec, vec.m_Vec, _MM_SHUFFLE(0, 0, 0, 0) );
	__m128 vyyyy = _mm_shuffle_ps(vec.m_Vec, vec.m_Vec, _MM_SHUFFLE(1, 1, 1, 1) );
	__m128 vzzzz = _mm_shuffle_ps(vec.m_Vec, vec.m_Vec, _MM_SHUFFLE(2, 2, 2, 2) );
	__m128 vwwww = _mm_shuffle_ps(vec.m_Vec, vec.m_Vec, _MM_SHUFFLE(3, 3, 3, 3) );

	__m128 temp;

	result.m_Vec = _mm_mul_ps(vxxxx, matrix.m_Vec0.m_Vec);	

	temp = _mm_mul_ps(vyyyy, matrix.m_Vec1.m_Vec);
	result.m_Vec = _mm_add_ps(result.m_Vec, temp);

	temp = _mm_mul_ps(vzzzz, matrix.m_Vec2.m_Vec);
	result.m_Vec = _mm_add_ps(result.m_Vec, temp);

	temp = _mm_mul_ps(vwwww, matrix.m_Vec3.m_Vec);
	result.m_Vec = _mm_add_ps(result.m_Vec, temp);
	*/

	Vector4SSE result;

	result  = vec.GetXXXX() * matrix.m_Vec0;
	result += vec.GetYYYY() * matrix.m_Vec1;
	result += vec.GetZZZZ() * matrix.m_Vec2;
	result += vec.GetWWWW() * matrix.m_Vec3;

	return result;
}

inline Vector4SSE operator*(Matrix4x4SSE &matrix, Vector4SSE &vec)
{

	Vector4SSE result;

	result[0] = VectorDot(vec, matrix[0]).GetX();
	result[1] = VectorDot(vec, matrix[1]).GetX();
	result[2] = VectorDot(vec, matrix[2]).GetX();
	result[3] = VectorDot(vec, matrix[3]).GetX();

	return result;

}

inline Matrix4x4SSE operator*(Matrix4x4SSE &a, Matrix4x4SSE &b)
{
	Matrix4x4SSE result;

	/*
	register __m128 v0 = b.m_Vec0.m_Vec;
	register __m128 v1 = b.m_Vec1.m_Vec;
	register __m128 v2 = b.m_Vec2.m_Vec;
	register __m128 v3 = b.m_Vec3.m_Vec;

	register __m128 v;
	register __m128 broadcast;
	register __m128 vsum;
	register __m128 vtemp;

	v = a.m_Vec0.m_Vec;
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0,0,0,0)); // xxxx
	vsum = _mm_mul_ps(broadcast, v0);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1,1,1,1)); // yyyy
	vtemp = _mm_mul_ps(broadcast, v1);
	vsum = _mm_add_ps(vtemp, vsum);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,2,2,2)); // zzzz
	vtemp = _mm_mul_ps(broadcast, v2);
	vsum = _mm_add_ps(vtemp, vsum);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3,3,3,3)); // wwww
	vtemp = _mm_mul_ps(broadcast, v3);
	result.m_Vec0.m_Vec = _mm_add_ps(vtemp, vsum);

	v = a.m_Vec1.m_Vec;
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0,0,0,0)); // xxxx
	vsum = _mm_mul_ps(broadcast, v0);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1,1,1,1)); // yyyy
	vtemp = _mm_mul_ps(broadcast, v1);
	vsum = _mm_add_ps(vtemp, vsum);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,2,2,2)); // zzzz
	vtemp = _mm_mul_ps(broadcast, v2);
	vsum = _mm_add_ps(vtemp, vsum);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3,3,3,3)); // wwww
	vtemp = _mm_mul_ps(broadcast, v3);
	result.m_Vec1.m_Vec = _mm_add_ps(vtemp, vsum);

	v = a.m_Vec2.m_Vec;
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0,0,0,0)); // xxxx
	vsum = _mm_mul_ps(broadcast, v0);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1,1,1,1)); // yyyy
	vtemp = _mm_mul_ps(broadcast, v1);
	vsum = _mm_add_ps(vtemp, vsum);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,2,2,2)); // zzzz
	vtemp = _mm_mul_ps(broadcast, v2);
	vsum = _mm_add_ps(vtemp, vsum);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3,3,3,3)); // wwww
	vtemp = _mm_mul_ps(broadcast, v3);
	result.m_Vec2.m_Vec = _mm_add_ps(vtemp, vsum);

	v = a.m_Vec3.m_Vec;
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0,0,0,0)); // xxxx
	vsum = _mm_mul_ps(broadcast, v0);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1,1,1,1)); // yyyy
	vtemp = _mm_mul_ps(broadcast, v1);
	vsum = _mm_add_ps(vtemp, vsum);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2,2,2,2)); // zzzz
	vtemp = _mm_mul_ps(broadcast, v3);
	vsum = _mm_add_ps(vtemp, vsum);
	broadcast = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3,3,3,3)); // wwww
	vtemp = _mm_mul_ps(broadcast, v3);
	result.m_Vec3.m_Vec = _mm_add_ps(vtemp, vsum);
	*/

	// turned out the simplified, non-assembly version is faster... Peter Pon
	result.m_Vec0 = 
		a.m_Vec0.GetXXXX() * b.m_Vec0 +
		a.m_Vec0.GetYYYY() * b.m_Vec1 +
		a.m_Vec0.GetZZZZ() * b.m_Vec2 +
		a.m_Vec0.GetWWWW() * b.m_Vec3;

	result.m_Vec1 = 
		a.m_Vec1.GetXXXX() * b.m_Vec0 +
		a.m_Vec1.GetYYYY() * b.m_Vec1 +
		a.m_Vec1.GetZZZZ() * b.m_Vec2 +
		a.m_Vec1.GetWWWW() * b.m_Vec3;

	result.m_Vec2 = 
		a.m_Vec2.GetXXXX() * b.m_Vec0 +
		a.m_Vec2.GetYYYY() * b.m_Vec1 +
		a.m_Vec2.GetZZZZ() * b.m_Vec2 +
		a.m_Vec2.GetWWWW() * b.m_Vec3;

	result.m_Vec3 = 
		a.m_Vec3.GetXXXX() * b.m_Vec0 +
		a.m_Vec3.GetYYYY() * b.m_Vec1 +
		a.m_Vec3.GetZZZZ() * b.m_Vec2 +
		a.m_Vec3.GetWWWW() * b.m_Vec3;

	return result;
}

inline Matrix4x4SSE operator*(Matrix4x4SSE &a, float f)
{
	Matrix4x4SSE result;

	__m128 ffff = _mm_set_ps1(f);

	result.m_Vec0.m_Vec = _mm_mul_ps(a.m_Vec0.m_Vec, ffff);
	result.m_Vec1.m_Vec = _mm_mul_ps(a.m_Vec1.m_Vec, ffff);
	result.m_Vec2.m_Vec = _mm_mul_ps(a.m_Vec2.m_Vec, ffff);
	result.m_Vec3.m_Vec = _mm_mul_ps(a.m_Vec3.m_Vec, ffff);

	return result;
}

inline Matrix4x4SSE operator*(float f, Matrix4x4SSE &a)
{
	Matrix4x4SSE result;
	__m128 ffff = _mm_set_ps1(f);

	result.m_Vec0.m_Vec = _mm_mul_ps(a.m_Vec0.m_Vec, ffff);
	result.m_Vec1.m_Vec = _mm_mul_ps(a.m_Vec1.m_Vec, ffff);
	result.m_Vec2.m_Vec = _mm_mul_ps(a.m_Vec2.m_Vec, ffff);
	result.m_Vec3.m_Vec = _mm_mul_ps(a.m_Vec3.m_Vec, ffff);

	return result;
}

inline Matrix4x4SSE operator/(Matrix4x4SSE &matrix, float f)
{
	Matrix4x4SSE result;
	__m128 ffff = _mm_set_ps1(f);

	result.m_Vec0.m_Vec = _mm_div_ps(matrix.m_Vec0.m_Vec, ffff);
	result.m_Vec1.m_Vec = _mm_div_ps(matrix.m_Vec1.m_Vec, ffff);
	result.m_Vec2.m_Vec = _mm_div_ps(matrix.m_Vec2.m_Vec, ffff);
	result.m_Vec3.m_Vec = _mm_div_ps(matrix.m_Vec3.m_Vec, ffff);

	return result;
}

inline Matrix4x4SSE Matrix4x4Transpose(Matrix4x4SSE &matrix)
{
	Matrix4x4SSE result = matrix;
	result.Transpose();
	return result;
}

inline Vector4SSE Matrix4x4TransformVector3(Vector4SSE &v, Matrix4x4SSE &matrix)
{
	Vector4SSE result;

	result  = v.GetXXXX() * matrix.m_Vec0;
	result += v.GetYYYY() * matrix.m_Vec1;
	result += v.GetZZZZ() * matrix.m_Vec2;
	result += matrix.m_Vec3;

	return result;
}

inline Matrix4x4SSE Matrix4x4FastInvert(Matrix4x4SSE &matrix)
{
	Matrix4x4SSE invmatrix = matrix;
	invmatrix.FastInvert();
	return invmatrix;
}

void Matrix4x4Rotate(Matrix4x4SSE &matrix, Vector4SSE &v, float radian);
void Matrix4x4FromQuaternion(Matrix4x4SSE &matrix, Vector4SSE &v);
void Matrix4x4FromQuaternionAndTransform(Matrix4x4SSE &matrix, Vector4SSE &q, Vector4SSE &t = g_MatrixSSE_Identity.m_Vec3 );
void QuaternionFromMatrix4x4(Matrix4x4SSE &matrix, Vector4SSE &quaternion);

#endif // _MATRIX4x4_SSE_
