#ifndef _MATRIX4x4_GENERAL_
#define _MATRIX4x4_GENERAL_

#if !defined(_ENABLE_GENERALCPU_)
#define _ENABLE_GENERALCPU_
#endif 

#include "Vector4.h"

class Matrix4x4CPU;

extern Matrix4x4CPU g_MatrixCPU_Identity;

inline Vector4CPU operator*(Vector4CPU &v, Matrix4x4CPU &matrix);
inline Matrix4x4CPU operator*(Matrix4x4CPU &a, Matrix4x4CPU &b);

class Matrix4x4CPU
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
			Vector4CPU m_Vec0, m_Vec1, m_Vec2, m_Vec3;
		};
	};

public:
	inline Matrix4x4CPU()
	{

	}

	inline Matrix4x4CPU(
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
		m_00 = 1.0f; m_01 = 0.0f; m_02 = 0.0f; m_03 = 0.0f;
		m_10 = 0.0f; m_11 = 1.0f; m_12 = 0.0f; m_13 = 0.0f;
		m_20 = 0.0f; m_21 = 0.0f; m_22 = 1.0f; m_23 = 0.0f;
		m_30 = 0.0f; m_31 = 0.0f; m_32 = 0.0f; m_33 = 1.0f;
		*/
		*this = g_MatrixCPU_Identity;
	}

	static Matrix4x4CPU &IdentityMatrix(void)
	{
		return g_MatrixCPU_Identity;
	}

	inline void SetRow(int row, Vector4CPU &vec)
	{
		assert(row>=0 && row<4);
		Vector4CPU *rows = &m_Vec0;
		rows[row] = vec;
	}

	inline void SetColumn(int column, Vector4CPU &vec)
	{
		assert(column>=0 && column<4);
		float *p = &m_00 + column;
		p[0] = vec.GetX(); 
		p[4] = vec.GetY(); 
		p[8] = vec.GetZ(); 
		p[12] = vec.GetW(); 
	}

	inline Vector4CPU &GetRow(int row)
	{
		assert(row>=0 && row<4);
		Vector4CPU *rows = &m_Vec0;
		return rows[row];
	}

	inline Vector4CPU GetColumn(int column)
	{
		assert(column>=0 && column<4);
		Vector4CPU vec;
		float *p = &m_00 + column;

		vec.SetX(p[0]);
		vec.SetY(p[4]);
		vec.SetZ(p[8]);
		vec.SetW(p[12]);

		return vec;
	}

	inline Matrix4x4CPU &Transpose(void)
	{
		FastMath::Swap(m_01,m_10);
		FastMath::Swap(m_02,m_20);
		FastMath::Swap(m_03,m_30);
		FastMath::Swap(m_12,m_21);
		FastMath::Swap(m_13,m_31);
		FastMath::Swap(m_23,m_32);

		return *this;
	}

	// assume input vec.w equals 1
	// result.w is garbage
	inline Vector4CPU TransformVec3(Vector4CPU &vec)
	{
		Vector4CPU result = vec * (*this);
		return result;
	}

	void Rotate( Vector4CPU &v, float radian)
	{
		float x = v.x;
		float y = v.y;
		float z = v.z;
		float w = v.w;

		float fSin, fCos;
		FastMath::SinCos(radian, fSin, fCos);

		Matrix4x4CPU  local, final;

		local.m_00 = ( x * x ) * ( 1.0f - fCos ) + fCos;
		local.m_01 = ( x * y ) * ( 1.0f - fCos ) + (z * fSin);
		local.m_02 = ( x * z ) * ( 1.0f - fCos ) - (y * fSin);
		local.m_03 = 0.0f;

		local.m_10 = ( y * x ) * ( 1.0f - fCos ) - (z * fSin);
		local.m_11 = ( y * y ) * ( 1.0f - fCos ) + fCos ;
		local.m_12 = ( y * z ) * ( 1.0f - fCos ) + (x * fSin);
		local.m_13 = 0.0f;

		local.m_20 = ( z * x ) * ( 1.0f - fCos ) + (y * fSin);
		local.m_21 = ( z * y ) * ( 1.0f - fCos ) - (x * fSin);
		local.m_22 = ( z * z ) * ( 1.0f - fCos ) + fCos;
		local.m_23 = 0.0f;

		local.m_30 = 0.0f;
		local.m_31 = 0.0f;
		local.m_32 = 0.0f;
		local.m_33 = 1.0f;

		final = local * (*this);
		*this = final;
	}

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

	void Rotate( float x, float y, float z, float radian )
	{
		Vector4CPU v(x, y, z, 0.0f);
		Rotate(v, radian);
	}

	void Rotate_Replace(float x, float y, float z, float radian)
	{
		Vector4CPU v(x, y, z, 0.0f);
		Rotate_Replace(v, radian);
	}

	// this = Rx * this
	void RotateX( const float radian )
	{
		float Sin, Cos;
		FastMath::SinCos(radian, Sin, Cos);

		float Temp10, Temp11, Temp12, Temp13;
		float Temp20, Temp21, Temp22, Temp23;

		Temp10 = m_10 * Cos + m_20 * Sin;
		Temp11 = m_11 * Cos + m_21 * Sin;
		Temp12 = m_12 * Cos + m_22 * Sin;
		Temp13 = m_13 * Cos + m_23 * Sin;

		Temp20 = m_10 *-Sin + m_20 * Cos;
		Temp21 = m_11 *-Sin + m_21 * Cos;
		Temp22 = m_12 *-Sin + m_22 * Cos;
		Temp23 = m_13 *-Sin + m_23 * Cos;

		m_10 = Temp10;
		m_11 = Temp11;
		m_12 = Temp12;
		m_13 = Temp13;
		m_20 = Temp20;
		m_21 = Temp21;
		m_22 = Temp22;
		m_23 = Temp23;
	}

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
		float Temp00, Temp01, Temp02, Temp03;
		float Temp20, Temp21, Temp22, Temp23;
		float Sin, Cos;
		FastMath::SinCos(radian, Sin, Cos);

		Temp00 = m_00 * Cos - m_20 * Sin;
		Temp01 = m_01 * Cos - m_21 * Sin;
		Temp02 = m_02 * Cos - m_22 * Sin;
		Temp03 = m_03 * Cos - m_23 * Sin;

		Temp20 = m_00 * Sin + m_20 * Cos;
		Temp21 = m_01 * Sin + m_21 * Cos;
		Temp22 = m_02 * Sin + m_22 * Cos;
		Temp23 = m_03 * Sin + m_23 * Cos;

		m_00 = Temp00;
		m_01 = Temp01;
		m_02 = Temp02;
		m_03 = Temp03;
		m_20 = Temp20;
		m_21 = Temp21;
		m_22 = Temp22;
		m_23 = Temp23;
	}

	void RotateY_Replace( const float radian )
	{
		float fSin, fCos;
		FastMath::SinCos(radian, fSin, fCos);

		m_Vec0.Set( fCos,  0.0f, -fSin, 0.0f);
		m_Vec1.Set( 0.0f,  1.0f,  0.0f, 0.0f);
		m_Vec2.Set(-fSin,  0.0f,  fCos, 0.0f);
		m_Vec3.Set( 0.0f,  0.0f,  0.0f, 1.0f);
	}

	// this = Rx * this
	void RotateZ( const float radian )
	{
		float Temp00, Temp01, Temp02, Temp03;
		float Temp10, Temp11, Temp12, Temp13;
		float Sin, Cos;
		FastMath::SinCos(radian, Sin, Cos);

		Temp00 = m_00 * Cos + m_10 * Sin;
		Temp01 = m_01 * Cos + m_11 * Sin;
		Temp02 = m_02 * Cos + m_12 * Sin;
		Temp03 = m_03 * Cos + m_13 * Sin;

		Temp10 = m_00 *-Sin + m_10 * Cos;
		Temp11 = m_01 *-Sin + m_11 * Cos;
		Temp12 = m_02 *-Sin + m_12 * Cos;
		Temp13 = m_03 *-Sin + m_13 * Cos;

		m_00 = Temp00;
		m_01 = Temp01;
		m_02 = Temp02;
		m_03 = Temp03;
		m_10 = Temp10;
		m_11 = Temp11;
		m_12 = Temp12;
		m_13 = Temp13;
	}

	void RotateZ_Replace( const float radian )
	{
		float fSin, fCos;
		FastMath::SinCos(radian, fSin, fCos);

		m_Vec0.Set( fCos,  fSin, 0.0f, 0.0f);
		m_Vec1.Set(-fSin,  fCos, 0.0f, 0.0f);
		m_Vec2.Set( 0.0f,  0.0f, 1.0f, 0.0f);
		m_Vec3.Set( 0.0f,  0.0f, 0.0f, 1.0f);
	}

	void Scale(Vector4CPU &scale)
	{
		Scale(scale[0], scale[1], scale[2]);
	}

	void Scale(float x, float y, float z)
	{
		m_Vec0 *= x;
		m_Vec1 *= y;
		m_Vec2 *= z;
	}

	void Scale_Replace(Vector4CPU &scale)
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

	inline Matrix4x4CPU &FastInvert(void)
	{
		Vector4CPU pos = VectorInvertSign(m_Vec3);
		m_Vec3 = g_MatrixCPU_Identity.m_Vec3;

		Transpose();
		Translate(pos);

		return *this;
	}

	Matrix4x4CPU &Invert(void);

	// this = T * this
	void Translate(float x, float y, float z)
	{
		m_Vec3 += m_Vec0 * x + m_Vec1 * y + m_Vec2 * z;
	}

	void Translate(Vector4CPU &v)
	{
		m_Vec3 += m_Vec0 * v.GetX() + m_Vec1 * v.GetY() + m_Vec2 * v.GetZ();
	}

	void Translate_Replace(float x, float y, float z)
	{
		Identity();
		m_Vec3[0] = x;
		m_Vec3[1] = y;
		m_Vec3[2] = z;
	}

	void Translate_Replace(Vector4CPU &v)
	{
		Identity();
		m_Vec3 = v;
	}

	void TranslateX(float d)
	{
		Vector4CPU dddd(d);
		m_Vec3 += m_Vec0 * dddd;
	}

	void TrsnalteY(float d)
	{
		Vector4CPU dddd(d);
		m_Vec3 += m_Vec1 * dddd;
	}

	void TranslateZ(float d)
	{
		Vector4CPU dddd(d);
		m_Vec3 += m_Vec2 * dddd;
	}

	Vector4CPU RotateVector(Vector4CPU &v)
	{
		Vector4CPU result;

		result.x = v.x*m_00 + v.y*m_10 + v.z*m_20;
		result.y = v.x*m_01 + v.y*m_11 + v.z*m_21;
		result.z = v.x*m_02 + v.y*m_12 + v.z*m_22;
		result.w = 1.0f;

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

	void SetMatrix3x4(Matrix4x4CPU &mat)
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

	inline Vector4CPU &operator[](int row)
	{
		return GetRow(row);
	}

	inline Matrix4x4CPU &operator=(Matrix4x4CPU &rhs)
	{
		m_Vec0 = rhs.m_Vec0;
		m_Vec1 = rhs.m_Vec1;
		m_Vec2 = rhs.m_Vec2;
		m_Vec3 = rhs.m_Vec3;

		return *this;
	}

	inline Matrix4x4CPU &operator=(float f)
	{
		Vector4CPU ffff(f);

		m_Vec0 = ffff;
		m_Vec1 = ffff;
		m_Vec2 = ffff;
		m_Vec3 = ffff;

		return *this;
	}

	inline Matrix4x4CPU &operator+=(float f)
	{
		Vector4CPU ffff(f);

		m_Vec0 += ffff;
		m_Vec1 += ffff;
		m_Vec2 += ffff;
		m_Vec3 += ffff;

		return *this;
	}

	inline Matrix4x4CPU &operator+=(Matrix4x4CPU &rhs)
	{
		m_Vec0 += rhs.m_Vec0;
		m_Vec1 += rhs.m_Vec1;
		m_Vec2 += rhs.m_Vec2;
		m_Vec3 += rhs.m_Vec3;

		return *this;
	}

	inline Matrix4x4CPU &operator-=(float f)
	{
		Vector4CPU ffff(f);

		m_Vec0 -= f;
		m_Vec1 -= f;
		m_Vec2 -= f;
		m_Vec3 -= f;

		return *this;
	}

	inline Matrix4x4CPU &operator-=(Matrix4x4CPU &rhs)
	{
		m_Vec0 -= rhs.m_Vec0;
		m_Vec1 -= rhs.m_Vec1;
		m_Vec2 -= rhs.m_Vec2;
		m_Vec3 -= rhs.m_Vec3;

		return *this;
	}

	inline Matrix4x4CPU &operator*=(float f)
	{
		m_Vec0 *= f;
		m_Vec1 *= f;
		m_Vec2 *= f;
		m_Vec3 *= f;

		return *this;
	}

	inline Matrix4x4CPU &operator*=(Matrix4x4CPU &rhs)
	{
		m_Vec0 = m_Vec0 * rhs;
		m_Vec1 = m_Vec1 * rhs;
		m_Vec2 = m_Vec2 * rhs;
		m_Vec3 = m_Vec3 * rhs;

		return *this;
	}

	inline Matrix4x4CPU &operator/=(float f)
	{
		m_Vec0 /= f;
		m_Vec1 /= f;
		m_Vec2 /= f;
		m_Vec3 /= f;

		return *this;
	}

};

inline bool operator==(Matrix4x4CPU &a, Matrix4x4CPU &b)
{
	// true if all vectors equal to each other
	bool result = a.m_Vec0==b.m_Vec0 && a.m_Vec1==b.m_Vec1 && a.m_Vec2==b.m_Vec2 && a.m_Vec3==b.m_Vec3;
	return result;
}

inline bool operator!=(Matrix4x4CPU &a, Matrix4x4CPU &b)
{
	// true if any one vector not-equal
	bool result = a.m_Vec0!=b.m_Vec0 || a.m_Vec1!=b.m_Vec1 || a.m_Vec2!=b.m_Vec2 || a.m_Vec3!=b.m_Vec3;
	return result;
}

inline Matrix4x4CPU operator+(Matrix4x4CPU &a, float f)
{
	Matrix4x4CPU result = a;
	result += f;
	return result;
}

inline Matrix4x4CPU operator+(float f, Matrix4x4CPU &a)
{
	Matrix4x4CPU result = a;
	result += f;
	return result;
}

inline Matrix4x4CPU operator+(Matrix4x4CPU &a, Matrix4x4CPU &b)
{
	Matrix4x4CPU result;

	result.m_Vec0 = a.m_Vec0 + b.m_Vec0;
	result.m_Vec1 = a.m_Vec1 + b.m_Vec1;
	result.m_Vec2 = a.m_Vec2 + b.m_Vec2;
	result.m_Vec3 = a.m_Vec3 + b.m_Vec3;

	return result;
}

inline Matrix4x4CPU operator-(Matrix4x4CPU &a, float f)
{
	Matrix4x4CPU result = a;
	result -= f;
	return result;
}

inline Matrix4x4CPU operator-(float f, Matrix4x4CPU &a)
{
	Matrix4x4CPU result = a;
	result -= f;
	return result;
}

inline Matrix4x4CPU operator-(Matrix4x4CPU &a, Matrix4x4CPU &b)
{
	Matrix4x4CPU result;

	result.m_Vec0 = a.m_Vec0 - b.m_Vec0;
	result.m_Vec1 = a.m_Vec1 - b.m_Vec1;
	result.m_Vec2 = a.m_Vec2 - b.m_Vec2;
	result.m_Vec3 = a.m_Vec3 - b.m_Vec3;

	return result;
}

inline Vector4CPU operator*(Vector4CPU &v, Matrix4x4CPU &m)
{
	Vector4CPU result;

	result.x = v.x*m.m_00 + v.y*m.m_10 + v.z*m.m_20 + v.w*m.m_30;
	result.y = v.x*m.m_01 + v.y*m.m_11 + v.z*m.m_21 + v.w*m.m_31;
	result.z = v.x*m.m_02 + v.y*m.m_12 + v.z*m.m_22 + v.w*m.m_32;
	result.w = v.x*m.m_03 + v.y*m.m_13 + v.z*m.m_23 + v.w*m.m_33;

	return result;
}

inline Vector4CPU operator*(Matrix4x4CPU &matrix, Vector4CPU &vec)
{
	Vector4CPU result;

	result[0] = VectorDot(vec, matrix[0]).GetX();
	result[1] = VectorDot(vec, matrix[1]).GetX();
	result[2] = VectorDot(vec, matrix[2]).GetX();
	result[3] = VectorDot(vec, matrix[3]).GetX();

	return result;
}

inline Matrix4x4CPU operator*(Matrix4x4CPU &a, Matrix4x4CPU &b)
{
	Matrix4x4CPU result;

	result.m_Vec0 = a.m_Vec0 * b;
	result.m_Vec1 = a.m_Vec1 * b;
	result.m_Vec2 = a.m_Vec2 * b;
	result.m_Vec3 = a.m_Vec3 * b;

	return result;
}

inline Matrix4x4CPU operator*(Matrix4x4CPU &a, float f)
{
	Matrix4x4CPU result;

	result.m_Vec0 = a.m_Vec0 * f;
	result.m_Vec1 = a.m_Vec1 * f;
	result.m_Vec2 = a.m_Vec2 * f;
	result.m_Vec3 = a.m_Vec3 * f;

	return result;
}

inline Matrix4x4CPU operator*(float f, Matrix4x4CPU &a)
{
	Matrix4x4CPU result;

	result.m_Vec0 = a.m_Vec0 * f;
	result.m_Vec1 = a.m_Vec1 * f;
	result.m_Vec2 = a.m_Vec2 * f;
	result.m_Vec3 = a.m_Vec3 * f;

	return result;
}

inline Matrix4x4CPU operator/(Matrix4x4CPU &a, float f)
{
	Matrix4x4CPU result;

	result.m_Vec0 = a.m_Vec0 / f;
	result.m_Vec1 = a.m_Vec1 / f;
	result.m_Vec2 = a.m_Vec2 / f;
	result.m_Vec3 = a.m_Vec3 / f;

	return result;

	return result;
}

inline Matrix4x4CPU Matrix4x4Transpose(Matrix4x4CPU &matrix)
{
	Matrix4x4CPU result = matrix;
	result.Transpose();
	return result;
}

void Matrix4x4FromQuaternion(Matrix4x4CPU &matrix, Vector4CPU &v);
void Matrix4x4FromQuaternionAndTransform(Matrix4x4CPU &matrix, Vector4CPU &q, Vector4CPU &t = g_MatrixCPU_Identity.m_Vec3 );
void QuaternionFromMatrix4x4(Matrix4x4CPU &matrix, Vector4CPU &quaternion);

#endif // _MATRIX4x4_GENERAL_
