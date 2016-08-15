#ifndef _VECTOR4_SSE_
#define _VECTOR4_SSE_

// SSE Vector library
// Peter Pon Aug 30 2006

#include <assert.h>
#include <stdio.h>
#include <xmmintrin.h>

#include "FastMath.h"

class Vector4SSE;

__declspec(align(16)) extern Vector4SSE g_VectorSSE_XYZ_Mask;
__declspec(align(16)) extern Vector4SSE g_VectorSSE_Zero;
__declspec(align(16)) extern Vector4SSE g_VectorSSE_One;
__declspec(align(16)) extern Vector4SSE g_VectorSSE_MinusOne;
__declspec(align(16)) extern Vector4SSE g_VectorSSE_AbsMask;
__declspec(align(16)) extern Vector4SSE g_VectorSSE_SignMask;

class Vector4SSE
{
public:
	union{
		__m128 m_Vec;
		struct 
		{
			float x,y,z,w;
		};
		float m_elem[4];
	};

public:
	inline Vector4SSE(void)
	{
		// do nothing
	}

	inline Vector4SSE(float x, float y, float z, float w = 1.0f)
	{
		m_Vec = _mm_set_ps(w,z,y,x);
	}

	inline Vector4SSE(float value)
	{
		Set(value);
	}

	inline Vector4SSE(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
	{
		m_Vec.m128_u32[0] = x;
		m_Vec.m128_u32[1] = y;
		m_Vec.m128_u32[2] = z;
		m_Vec.m128_u32[3] = w;
	}

	inline Vector4SSE(unsigned int x)
	{
		m_Vec.m128_u32[0] = x;
		m_Vec.m128_u32[1] = x;
		m_Vec.m128_u32[2] = x;
		m_Vec.m128_u32[3] = x;
	}

	void ConsoleOutput(void);

	inline void Set(float x, float y, float z, float w=0.0f)
	{
		m_Vec = _mm_set_ps(w, z, y, x);
	}

	inline void Set(float value)
	{
		m_Vec = _mm_set_ps1(value);
	}

	inline void Set(float *p)
	{
		// not aligned
		m_Vec = _mm_loadu_ps(p);
	}

	inline void SetX(float x)
	{
		this->x = x;
	}

	inline void SetY(float y)
	{
		this->y = y;
	}

	inline void SetZ(float z)
	{
		this->z = z;
	}

	inline void SetW(float w)
	{
		this->w = w;
	}

	inline void SetXYZ(float *p)
	{
		x = p[0];
		y = p[1];
		z = p[2];
	}

	inline float GetX(void)
	{
		return x;
	}

	inline Vector4SSE GetXXXX(void)
	{
		Vector4SSE result;
		result.m_Vec = _mm_shuffle_ps(m_Vec, m_Vec, _MM_SHUFFLE(0, 0, 0, 0) );
		return result;
	}

	inline float GetY(void)
	{
		return y;
	}

	inline Vector4SSE GetYYYY(void)
	{
		Vector4SSE result;
		result.m_Vec = _mm_shuffle_ps(m_Vec, m_Vec, _MM_SHUFFLE(1, 1, 1, 1) );
		return result;
	}

	inline float GetZ(void)
	{
		return z;
	}

	inline Vector4SSE GetZZZZ(void)
	{
		Vector4SSE result;
		result.m_Vec = _mm_shuffle_ps(m_Vec, m_Vec, _MM_SHUFFLE(2, 2, 2, 2) );
		return result;
	}

	inline float GetW(void)
	{
		return w;
	}

	inline Vector4SSE GetWWWW(void)
	{
		Vector4SSE result;
		result.m_Vec = _mm_shuffle_ps(m_Vec, m_Vec, _MM_SHUFFLE(3, 3, 3, 3) );
		return result;
	}

	inline void StoreAligned(float *p)
	{
		_mm_store_ps(p, m_Vec);
	}

	inline void Store(float *p)
	{
		_mm_storeu_ps(p, m_Vec);
	}

	inline void StoreXYZ(float *p)
	{
		p[0] = x;
		p[1] = y;
		p[2] = z;
	}

	inline void StoreXY(float *p)
	{
		p[0] = x;
		p[1] = y;
	}

	inline void Reciprocal(void)
	{
		m_Vec = _mm_rcp_ps(m_Vec);
	}

	inline void Sqrt(void)
	{
		m_Vec = _mm_sqrt_ps(m_Vec);
	}

	inline void ReciprocalSqrt(void)
	{
		m_Vec = _mm_rsqrt_ps(m_Vec);
	}

	inline float Length(void)
	{
		float vectorlength;

		__m128 r1 = _mm_mul_ps(m_Vec, m_Vec); // r1 = (x*x, y*y, z*z, w*w)
		__m128 r2 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1,1,1,1)); // r2 = (r1.y, r1.y, r1.y, r1.y)
		__m128 r3 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(2,2,2,2)); // r3 = (r1.z, r1.z, r1.z, r1.z)

		r2 = _mm_add_ss(r1, r2); // r2.x += r1.x = x*x + y*y;
		r2 = _mm_add_ss(r3, r2); // r2.x += r3.x = x*x + y*y * z*z;

		__m128 r4 = _mm_sqrt_ss(r2);
		_mm_store_ss(&vectorlength, r4);

		return vectorlength;
	}

	inline void Normalize(void)
	{
		__m128 r1 = _mm_mul_ps(m_Vec, m_Vec); // r1 = (x*x, y*y, z*z, w*w)
		__m128 r2 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1,1,1,1)); // r2 = (r1.y, r1.y, r1.y, r1.y)
		__m128 r3 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(2,2,2,2)); // r3 = (r1.z, r1.z, r1.z, r1.z)

		r2 = _mm_add_ss(r1, r2); // r2.x += r1.x = x*x + y*y;
		r2 = _mm_add_ss(r3, r2); // r2.x += r3.x = x*x + y*y * z*z;
		
		__m128 r4 = _mm_rsqrt_ss(r2);
		r4 = _mm_shuffle_ps(r4, r4, _MM_SHUFFLE(0, 0, 0, 0));
		m_Vec = _mm_mul_ps(m_Vec, r4);
	}

	inline float NormalizeAndGetLength(void)
	{
		float vectorlength;

		__m128 r1 = _mm_mul_ps(m_Vec, m_Vec); // r1 = (x*x, y*y, z*z, w*w)
		__m128 r2 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1,1,1,1)); // r2 = (r1.y, r1.y, r1.y, r1.y)
		__m128 r3 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(2,2,2,2)); // r3 = (r1.z, r1.z, r1.z, r1.z)

		r2 = _mm_add_ss(r1, r2); // r2.x += r1.x = x*x + y*y;
		r2 = _mm_add_ss(r3, r2); // r2.x += r3.x = x*x + y*y * z*z;

		__m128 r4 = _mm_sqrt_ss(r2);
		_mm_store_ss(&vectorlength, r4);

		r4 = _mm_shuffle_ps(r4, r4, _MM_SHUFFLE(0, 0, 0, 0));
		m_Vec = _mm_div_ps(m_Vec, r4);

		return vectorlength;
	}

	inline Vector4SSE &Abs(void)
	{
		m_Vec = _mm_and_ps(m_Vec, g_VectorSSE_AbsMask.m_Vec);
		return *this;
	}

	inline Vector4SSE &InvertSign(void)
	{
		m_Vec = _mm_xor_ps(m_Vec, g_VectorSSE_SignMask.m_Vec);
		return *this;
	}

	inline Vector4SSE &Saturate(void)
	{
		m_Vec = _mm_min_ps(m_Vec, g_VectorSSE_One.m_Vec);
		m_Vec = _mm_max_ps(m_Vec, g_VectorSSE_Zero.m_Vec);
		return *this;
	}

	inline Vector4SSE &Clamp_to_0(void)
	{
		m_Vec = _mm_max_ps(m_Vec, g_VectorSSE_Zero.m_Vec);
		return *this;
	}

	inline Vector4SSE &Clamp_to_1(void)
	{
		m_Vec = _mm_min_ps(m_Vec, g_VectorSSE_One.m_Vec);
		return *this;
	}

	inline Vector4SSE &Floor(void)
	{
		// this is not optimjized with SSE instructions
		x = floor(x);
		y = floor(y);
		z = floor(z);
		w = floor(w);
		return *this;
	}

	inline static Vector4SSE &GetZero(void)
	{
		return g_VectorSSE_Zero;
	}

	inline static Vector4SSE &GetOne(void)
	{
		return g_VectorSSE_One;
	}

	// operators
	inline Vector4SSE &operator=(Vector4SSE &rhs)
	{
		m_Vec = rhs.m_Vec;
		return *this;
	}

	inline Vector4SSE &operator=(float value)
	{
		m_Vec = _mm_set_ps1(value);
		return *this;
	}

	inline Vector4SSE &operator+=(Vector4SSE &rhs)
	{
		m_Vec = _mm_add_ps(m_Vec, rhs.m_Vec);
		return *this;
	}

	inline Vector4SSE &operator+=(float value)
	{
		__m128 r = _mm_set_ps1(value);
		m_Vec = _mm_add_ps(m_Vec, r);
		return *this;
	}

	inline Vector4SSE &operator-=(Vector4SSE &rhs)
	{
		m_Vec = _mm_sub_ps(m_Vec, rhs.m_Vec);
		return *this;
	}

	inline Vector4SSE &operator-=(float value)
	{
		__m128 r = _mm_set_ps1(value);
		m_Vec = _mm_sub_ps(m_Vec, r);
		return *this;
	}

	inline Vector4SSE &operator*=(Vector4SSE &rhs)
	{
		m_Vec = _mm_mul_ps(m_Vec, rhs.m_Vec);
		return *this;
	}

	inline Vector4SSE &operator*=(float value)
	{
		__m128 r = _mm_set_ps1(value);
		m_Vec = _mm_mul_ps(m_Vec, r);
		return *this;
	}

	inline Vector4SSE &operator/=(Vector4SSE &rhs)
	{
		m_Vec = _mm_div_ps(m_Vec, rhs.m_Vec);
		return *this;
	}

	inline Vector4SSE &operator/=(float value)
	{
		__m128 r = _mm_set_ps1(value);
		m_Vec = _mm_div_ps(m_Vec, r);
		return *this;
	}

	inline float &operator[](int index)
	{
		assert(index>=0 && index<4);
		return m_elem[index];
	}

	static const char *InstructionType(void)
	{
		return "Intel_SSE";
	}
};

inline Vector4SSE operator-(Vector4SSE &rhs)
{
	Vector4SSE result;
	result.m_Vec = _mm_xor_ps(rhs.m_Vec, g_VectorSSE_SignMask.m_Vec);
	return result;
}

inline bool operator==(Vector4SSE &a, Vector4SSE &b)
{
	__m128 r = _mm_cmpeq_ps(a.m_Vec, b.m_Vec);
	int sign_mask = _mm_movemask_ps(r);
	return sign_mask==0x0f;
}

inline bool operator!=(Vector4SSE &a, Vector4SSE &b)
{
	__m128 r = _mm_cmpeq_ps(a.m_Vec, b.m_Vec);
	int sign_mask = _mm_movemask_ps(r);
	return sign_mask!=0xff;
}

inline Vector4SSE operator+(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE c;
	c.m_Vec = _mm_add_ps(a.m_Vec, b.m_Vec);
	return c;
}

inline Vector4SSE operator-(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE c;
	c.m_Vec = _mm_sub_ps(a.m_Vec, b.m_Vec);
	return c;
}

inline Vector4SSE operator*(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE c;
	c.m_Vec = _mm_mul_ps(a.m_Vec, b.m_Vec);
	return c;
}

inline Vector4SSE operator*(float f, Vector4SSE &v)
{
	Vector4SSE result;

	__m128 ffff = _mm_set_ps1(f);
	result.m_Vec = _mm_mul_ps(v.m_Vec, ffff);

	return result;
}

inline Vector4SSE operator*(Vector4SSE &v, float f)
{
	Vector4SSE result;

	__m128 ffff = _mm_set_ps1(f);
	result.m_Vec = _mm_mul_ps(v.m_Vec, ffff);

	return result;
}

inline Vector4SSE operator/(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE c;
	c.m_Vec = _mm_div_ps(a.m_Vec, b.m_Vec);
	return c;
}

inline Vector4SSE operator/(Vector4SSE &a, float f)
{
	Vector4SSE result;

	__m128 ffff = _mm_set_ps1(f);
	result.m_Vec = _mm_div_ps(a.m_Vec, ffff);

	return result;
}

inline Vector4SSE operator/(float f, Vector4SSE &a)
{
	Vector4SSE result;

	__m128 ffff = _mm_set_ps1(f);
	result.m_Vec = _mm_div_ps(ffff, a.m_Vec);

	return result;
}

inline float Vector3Dotf(Vector4SSE &a, Vector4SSE &b)
{
	float result;

	__m128 r1 = _mm_mul_ps(a.m_Vec, b.m_Vec); // r1 = (x*x, y*y, z*z, w*w)
	__m128 r2 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1,1,1,1)); // r2 = (r1.y, r1.y, r1.y, r1.y)
	__m128 r3 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(2,2,2,2)); // r3 = (r1.z, r1.z, r1.z, r1.z)

	r2 = _mm_add_ss(r1, r2); // r2.x += r1.x = x*x + y*y;
	r2 = _mm_add_ss(r3, r2); // r2.x += r3.x = x*x + y*y * z*z;

	_mm_store_ss(&result, r2);

	return result;
}

inline float VectorDotf(Vector4SSE &a, Vector4SSE &b)
{
	float result;

	__m128 r1 = _mm_mul_ps(a.m_Vec, b.m_Vec);
	__m128 r2 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1,1,1,1));
	__m128 r3 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(2,2,2,2));
	__m128 r4 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(3,3,3,3));

	r2 = _mm_add_ss(r1, r2); // r2.x = x*x + y*y;
	r2 = _mm_add_ss(r3, r2); // r2.x = x*x + y*y + z*z;
	r2 = _mm_add_ss(r4, r2); // r2.x = x*x + y*y + z*z + w*w;

	_mm_store_ss(&result, r2);

	return result;
}

inline Vector4SSE Vector3Dot(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE c = Vector3Dotf(a, b);
	return c;
}

inline Vector4SSE VectorDot(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE c = VectorDotf(a,b);
	return c;
}

inline Vector4SSE Vector3CrossProduct(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE result;

	Vector4SSE r0, r1, r2, r3;

	// Shuffles
	//					  w  y   z  x
	// w | x | z | y  == 11 00  10 01 = 0xC9 
	// w | y | x | z  == 11 01  00 10 = 0xD2

	r0.m_Vec = _mm_shuffle_ps(a.m_Vec, a.m_Vec, 0xc9);
	r1.m_Vec = _mm_shuffle_ps(b.m_Vec, b.m_Vec, 0xd2);
	r2.m_Vec = _mm_shuffle_ps(a.m_Vec, a.m_Vec, 0xd2);
	r3.m_Vec = _mm_shuffle_ps(b.m_Vec, b.m_Vec, 0xc9);

	r1.m_Vec = _mm_mul_ps(r0.m_Vec, r1.m_Vec);
	r3.m_Vec = _mm_mul_ps(r2.m_Vec, r3.m_Vec);
	r2.m_Vec = _mm_sub_ps(r1.m_Vec, r3.m_Vec);

	result = r2;

	return result;
}

inline Vector4SSE Vector3Cross_Verify(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE c;

	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;
	c.w = 0.0f;

	return c;
}

inline Vector4SSE VectorLerp(Vector4SSE &a, Vector4SSE &b, float t)
{
	Vector4SSE c;
	
	__m128 tvec = _mm_set_ps1(t);
	__m128 diff = _mm_sub_ps(b.m_Vec, a.m_Vec);
	__m128 diffvec = _mm_mul_ps(diff, tvec);

	c.m_Vec = _mm_add_ps(a.m_Vec, diffvec);

	return c;
}

inline Vector4SSE VectorMax(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE c;

	c.m_Vec = _mm_max_ps(a.m_Vec, b.m_Vec);

	return c;
}

inline Vector4SSE VectorGetMax(Vector4SSE &a, Vector4SSE &b)
{
	return VectorMax(a, b);
}

inline Vector4SSE VectorMin(Vector4SSE &a, Vector4SSE &b)
{
	Vector4SSE c;

	c.m_Vec = _mm_min_ps(a.m_Vec, b.m_Vec);

	return c;
}

inline Vector4SSE VectorGetMin(Vector4SSE &a, Vector4SSE &b)
{
	return VectorMin(a, b);
}

inline Vector4SSE VectorReciprocal(Vector4SSE &a)
{
	Vector4SSE result;

	result.m_Vec = _mm_rcp_ps(a.m_Vec);

	return result;
}

inline Vector4SSE VectorSqrt(Vector4SSE &a)
{
	Vector4SSE result;

	result.m_Vec = _mm_sqrt_ps(a.m_Vec);

	return result;
}

inline Vector4SSE VectorReciprocalSqrt(Vector4SSE &a)
{
	Vector4SSE result;

	result.m_Vec = _mm_rsqrt_ps(a.m_Vec);

	return result;
}

inline Vector4SSE VectorNormalize(Vector4SSE &a)
{
	Vector4SSE result;

	__m128 r1 = _mm_mul_ps(a.m_Vec, a.m_Vec); // r1 = (x*x, y*y, z*z, w*w)
	__m128 r2 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1,1,1,1)); // r2 = (r1.y, r1.y, r1.y, r1.y)
	__m128 r3 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(2,2,2,2)); // r3 = (r1.z, r1.z, r1.z, r1.z)

	r2 = _mm_add_ss(r1, r2); // r2.x += r1.x = x*x + y*y;
	r2 = _mm_add_ss(r3, r2); // r2.x += r3.x = x*x + y*y * z*z;

	__m128 r4 = _mm_rsqrt_ss(r2);
	r4 = _mm_shuffle_ps(r4, r4, _MM_SHUFFLE(0, 0, 0, 0));
	result.m_Vec = _mm_mul_ps(a.m_Vec, r4);

	return result;
}

inline float VectorNormalizeAndGetLength(Vector4SSE &a, Vector4SSE &result)
{
	float vectorlength;

	__m128 r1 = _mm_mul_ps(a.m_Vec, a.m_Vec); // r1 = (x*x, y*y, z*z, w*w)
	__m128 r2 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(1,1,1,1)); // r2 = (r1.y, r1.y, r1.y, r1.y)
	__m128 r3 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(2,2,2,2)); // r3 = (r1.z, r1.z, r1.z, r1.z)

	r2 = _mm_add_ss(r1, r2); // r2.x += r1.x = x*x + y*y;
	r2 = _mm_add_ss(r3, r2); // r2.x += r3.x = x*x + y*y * z*z;

	__m128 r4 = _mm_sqrt_ss(r2);
	_mm_store_ss(&vectorlength, r4);

	r4 = _mm_shuffle_ps(r4, r4, _MM_SHUFFLE(0, 0, 0, 0));
	result.m_Vec = _mm_div_ps(a.m_Vec, r4);

	return vectorlength;
}

inline Vector4SSE VectorAbs(Vector4SSE &a)
{
	Vector4SSE result;

	result.m_Vec = _mm_and_ps(a.m_Vec, g_VectorSSE_AbsMask.m_Vec);

	return result;
}

inline Vector4SSE VectorInvertSign(Vector4SSE &a)
{
	Vector4SSE result;

	result.m_Vec = _mm_xor_ps(a.m_Vec, g_VectorSSE_SignMask.m_Vec);
	
	return result;
}

inline Vector4SSE VectorSaturate(Vector4SSE &a)
{
	Vector4SSE result;

	result.m_Vec = _mm_min_ps(a.m_Vec, g_VectorSSE_One.m_Vec);
	result.m_Vec = _mm_max_ps(result.m_Vec, g_VectorSSE_Zero.m_Vec);

	return result;
}

inline Vector4SSE VectorQuaternionToRotation(Vector4SSE &v)
{
	Vector4SSE result;
	
	float cosvalue = FastMath::ACos(v.w);
	float s = FastMath::ASin(cosvalue);

	if ( s==0.0f )
	{
		result.Set(1.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		result = v / s;
		result.w = cosvalue * 2.0f;
	}

	return result;
}

inline Vector4SSE VectorRotationToQuaternion(Vector4SSE &v)
{
	Vector4SSE result;

	float radius_half = v.w * 0.5f;
	float sinvalue, cosvalue;
	FastMath::SinCos(radius_half, sinvalue, cosvalue);

	result = v * sinvalue;
	result.w = cosvalue;

	return result;
}

// plane向量記錄平面表式示中a*x+b*y+c*z+d=0中的(a,b,c,d)值
inline Vector4SSE PointToPlaneDistance(Vector4SSE &point, Vector4SSE &plane)
{
	Vector4SSE p = point;
	p[3] = 1.0f;
	return VectorDot(p, plane);
}

// 計算point對plane平面鏡射後的3D位置
inline Vector4SSE MirrorPoint(Vector4SSE &point, Vector4SSE &plane)
{
	Vector4SSE mirrored_point;
	
	float d = PointToPlaneDistance(point, plane)[0];
	mirrored_point = point - (2.0f * d ) * plane;
	mirrored_point[3] = 1.0f;

	return mirrored_point;
}

// 計算vector對plane平面鏡射後的3D方向
inline Vector4SSE MirrorVector(Vector4SSE &vector, Vector4SSE &plane)
{
	Vector4SSE mirrored_a, mirrored_b, mirrored_vector;
	Vector4SSE vZero = Vector4SSE::GetZero();

	float d = PointToPlaneDistance(vector, plane)[0];
	mirrored_a = vector - (2.0f * d ) * plane;

	d = PointToPlaneDistance(vZero, plane)[0];
	mirrored_b = vZero - (2.0f * d ) * plane;
	
	mirrored_vector = mirrored_a - mirrored_b;
	mirrored_vector[3] = 1.0f;

	return mirrored_vector;
}

#endif // _VECTOR4_SSE_