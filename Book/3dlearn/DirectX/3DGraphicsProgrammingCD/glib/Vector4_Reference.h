#ifndef _VECTOR4_GENERAL_
#define _VECTOR4_GENERAL_

#if !defined(_ENABLE_GENERALCPU_)
	#define _ENABLE_GENERALCPU_
#endif 

// Vector library
// Peter Pon Aug 30 2006

#include <assert.h>
#include <stdio.h>

#include "FastMath.h"

class Vector4CPU;

extern Vector4CPU g_VectorCPU_Zero;
extern Vector4CPU g_VectorCPU_One;

class Vector4CPU
{
public:
	union{
		struct 
		{
			float x,y,z,w;
		};
		float m_elem[4];
		unsigned int m_ielem[4];
	};

public:
	inline Vector4CPU(void)
	{
		// do nothing
	}

	inline Vector4CPU(float x, float y, float z, float w=1.0f)
	{
		Set(x, y, z, w);
	}

	inline Vector4CPU(float value)
	{
		Set(value);
	}

	inline Vector4CPU(unsigned int x, unsigned int y, unsigned int z, unsigned int w)
	{
		m_ielem[0] = x;
		m_ielem[0] = y;
		m_ielem[0] = z;
		m_ielem[0] = w;
	}

	inline Vector4CPU(unsigned int x)
	{
		m_ielem[0] = x;
		m_ielem[0] = x;
		m_ielem[0] = x;
		m_ielem[0] = x;
	}

	void ConsoleOutput(void);

	inline void Set(float x, float y, float z, float w=0.0f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	inline void Set(float value)
	{
		this->x = value;
		this->y = value;
		this->z = value;
		this->w = value;
	}

	inline void Set(float *p)
	{
		this->x = p[0];
		this->y = p[1];
		this->z = p[2];
		this->w = p[3];
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

	inline Vector4CPU GetXXXX(void)
	{
		Vector4CPU result(x);
		return result;
	}

	inline float GetY(void)
	{
		return y;
	}

	inline Vector4CPU GetYYYY(void)
	{
		Vector4CPU result(y);
		return result;
	}

	inline float GetZ(void)
	{
		return z;
	}

	inline Vector4CPU GetZZZZ(void)
	{
		Vector4CPU result(z);
		return result;
	}

	inline float GetW(void)
	{
		return w;
	}

	inline Vector4CPU GetWWWW(void)
	{
		Vector4CPU result(w);
		return result;
	}

	inline float StoreAligned(float *p)
	{
		p[0] = x;
		p[1] = y;
		p[2] = z;
		p[3] = w;
	}

	inline void Store(float *p)
	{
		p[0] = x;
		p[1] = y;
		p[2] = z;
		p[3] = w;
	}

	inline void StoreXYZ(float *p)
	{
		p[0] = x;
		p[1] = y;
		p[2] = z;
	}

	inline void Reciprocal(void)
	{
		x = 1.0f/x;
		y = 1.0f/y;
		z = 1.0f/z;
		w = 1.0f/w;
	}

	inline void Sqrt(void)
	{
		x = FastMath::Sqrt(x);
		y = FastMath::Sqrt(y);
		z = FastMath::Sqrt(z);
		w = FastMath::Sqrt(w);
	}

	inline void ReciprocalSqrt(void)
	{
		x = FastMath::ReciprocalSqrt(x);
		y = FastMath::ReciprocalSqrt(y);
		z = FastMath::ReciprocalSqrt(z);
		w = FastMath::ReciprocalSqrt(w);
	}

	inline float Length(void)
	{
		float vectorlength;
		
		float sum = x*x + y*y + z*z;
		vectorlength = FastMath::Sqrt(sum);

		return vectorlength;
	}

	inline void Normalize(void)
	{
		float len = Length();
		x/=len;
		y/=len;
		z/=len;
	}

	inline float NormalizeAndGetLength(void)
	{
		float len = Length();
		x/=len;
		y/=len;
		z/=len;
		return len;
	}

	inline Vector4CPU &Abs(void)
	{
		x = FastMath::Abs(x);
		y = FastMath::Abs(y);
		z = FastMath::Abs(z);
		w = FastMath::Abs(w);

		return *this;
	}

	inline Vector4CPU &InvertSign(void)
	{
		x = -x;
		y = -y;
		z = -z;
		w = -w;

		return *this;
	}

	inline Vector4CPU &Saturate(void)
	{
		if ( x > 1.0f ) x = 1.0f;
		if ( y > 1.0f ) y = 1.0f;
		if ( z > 1.0f ) z = 1.0f;
		if ( w > 1.0f ) w = 1.0f;

		if ( x < 0.0f ) x = 0.0f;
		if ( y < 0.0f ) y = 0.0f;
		if ( z < 0.0f ) z = 0.0f;
		if ( w < 0.0f ) w = 0.0f;
	
		return *this;
	}

	inline Vector4CPU &Clamp_to_0(void)
	{
		if ( x < 0.0f ) x = 0.0f;
		if ( y < 0.0f ) y = 0.0f;
		if ( z < 0.0f ) z = 0.0f;
		if ( w < 0.0f ) w = 0.0f;
	
		return *this;
	}

	inline Vector4CPU &Clamp_to_1(void)
	{
		if ( x > 1.0f ) x = 1.0f;
		if ( y > 1.0f ) y = 1.0f;
		if ( z > 1.0f ) z = 1.0f;
		if ( w > 1.0f ) w = 1.0f;

		return *this;
	}

	inline Vector4CPU &Floor(void)
	{
		x = floor(x);
		y = floor(y);
		z = floor(z);
		w = floor(w);
		return *this;
	}

	inline static Vector4CPU &GetZero(void)
	{
		return g_VectorCPU_Zero;
	}

	inline static Vector4CPU &GetOne(void)
	{
		return g_VectorCPU_One;
	}

	// operators
	inline Vector4CPU &operator=(Vector4CPU &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
		return *this;
	}

	inline Vector4CPU &operator=(float value)
	{
		Set(value);
		return *this;
	}

	inline Vector4CPU &operator+=(Vector4CPU &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}

	inline Vector4CPU &operator+=(float value)
	{
		x += value;
		y += value;
		z += value;
		w += value;
		return *this;
	}

	inline Vector4CPU &operator-=(Vector4CPU &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	inline Vector4CPU &operator-=(float value)
	{
		x -= value;
		y -= value;
		z -= value;
		w -= value;
		return *this;
	}

	inline Vector4CPU &operator*=(Vector4CPU &rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		w *= rhs.w;
		return *this;
	}

	inline Vector4CPU &operator*=(float value)
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
		return *this;
	}

	inline Vector4CPU &operator/=(Vector4CPU &rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		w /= rhs.w;
		return *this;
	}

	inline Vector4CPU &operator/=(float value)
	{
		x /= value;
		y /= value;
		z /= value;
		w /= value;
		return *this;
	}

	inline float &operator[](int index)
	{
		assert(index>=0 && index<4);
		return m_elem[index];
	}

	static const char *InstructionType(void)
	{
		return "General_CPU";
	}
};

inline bool operator==(Vector4CPU &a, Vector4CPU &b)
{
	bool result = (a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w);
	return result;
}

inline bool operator!=(Vector4CPU &a, Vector4CPU &b)
{
	bool result = (a.x==b.x && a.y==b.y && a.z==b.z && a.w==b.w);
	return !result;
}

inline Vector4CPU operator+(Vector4CPU &a, Vector4CPU &b)
{
	Vector4CPU c;

	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	c.w = a.w + b.w;

	return c;
}

inline Vector4CPU operator-(Vector4CPU &a, Vector4CPU &b)
{
	Vector4CPU c;

	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	c.w = a.w - b.w;

	return c;
}

inline Vector4CPU operator*(Vector4CPU &a, Vector4CPU &b)
{
	Vector4CPU c;

	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
	c.w = a.w * b.w;

	return c;
}

inline Vector4CPU operator*(float f, Vector4CPU &v)
{
	Vector4CPU result;

	result.x = f * v.x;
	result.y = f * v.y;
	result.z = f * v.z;
	result.w = f * v.w;

	return result;
}

inline Vector4CPU operator*(Vector4CPU &v, float f)
{
	Vector4CPU result;

	result.x = f * v.x;
	result.y = f * v.y;
	result.z = f * v.z;
	result.w = f * v.w;

	return result;
}

inline Vector4CPU operator/(Vector4CPU &a, Vector4CPU &b)
{
	Vector4CPU c;

	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
	c.w = a.w / b.w;

	return c;
}

inline Vector4CPU operator/(Vector4CPU &a, float f)
{
	Vector4CPU c;

	c.x = a.x / f;
	c.y = a.y / f;
	c.z = a.z / f;
	c.w = a.w / f;

	return c;
}

inline Vector4CPU operator/(float f, Vector4CPU &a)
{
	Vector4CPU c;

	c.x = f / a.x;
	c.y = f / a.y;
	c.z = f / a.z;
	c.w = f / a.w;

	return c;
}

inline float Vector3Dotf(Vector4CPU &a, Vector4CPU &b)
{
	float result = a.x * b.x + a.y * b.y + a.z * b.z;
	return result;
}

inline float VectorDotf(Vector4CPU &a, Vector4CPU &b)
{
	float result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	return result;
}

inline Vector4CPU Vector3Dot(Vector4CPU &a, Vector4CPU &b)
{
	Vector4CPU c = Vector3Dotf(a, b);
	return c;
}

inline Vector4CPU VectorDot(Vector4CPU &a, Vector4CPU &b)
{
	Vector4CPU c = VectorDotf(a,b);
	return c;
}

inline Vector4CPU Vector3CrossProduct(Vector4CPU &a, Vector4CPU &b)
{
	Vector4CPU c;

	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;
	c.w = 0.0f;

	return c;
}

inline Vector4CPU VectorLerp(Vector4CPU &a, Vector4CPU &b, float t)
{
	Vector4CPU c = a + (b - a) * t;
	return c;
}

inline Vector4CPU VectorGetMax(Vector4CPU &a, Vector4CPU &b)
{
	Vector4CPU c;

	c.x = a.x > b.x ? a.x : b.x;
	c.y = a.y > b.y ? a.y : b.y;
	c.z = a.z > b.z ? a.z : b.z;
	c.w = a.w > b.w ? a.w : b.w;

	return c;
}

inline Vector4CPU VectorGetMin(Vector4CPU &a, Vector4CPU &b)
{
	Vector4CPU c;

	c.x = a.x < b.x ? a.x : b.x;
	c.y = a.y < b.y ? a.y : b.y;
	c.z = a.z < b.z ? a.z : b.z;
	c.w = a.w < b.w ? a.w : b.w;

	return c;
}

inline Vector4CPU VectorReciprocal(Vector4CPU &a)
{
	Vector4CPU result = a;
	result.Reciprocal();
	return result;
}

inline Vector4CPU VectorSqrt(Vector4CPU &a)
{
	Vector4CPU result = a;
	result.Sqrt();
	return result;
}

inline Vector4CPU VectorReciprocalSqrt(Vector4CPU &a)
{
	Vector4CPU result = a;
	result.ReciprocalSqrt();
	return result;
}

inline Vector4CPU VectorNormalize(Vector4CPU &a)
{
	Vector4CPU result = a;
	result.Normalize();
	return result;
}

inline float VectorNormalizeAndGetLength(Vector4CPU &a, Vector4CPU &result)
{
	result = a;
	float vectorlength = result.NormalizeAndGetLength();
	return vectorlength;
}

inline Vector4CPU VectorAbs(Vector4CPU &a)
{
	Vector4CPU result = a;
	result.Abs();
	return result;
}

inline Vector4CPU VectorInvertSign(Vector4CPU &a)
{
	Vector4CPU result = a;
	result.InvertSign();

	return result;
}

inline Vector4CPU VectorSaturate(Vector4CPU &a)
{
	Vector4CPU result = a;
	result.Saturate();
	return result;
}

inline Vector4CPU VectorQuaternionToRotation(Vector4CPU &v)
{
	Vector4CPU result;

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

inline Vector4CPU VectorRotationToQuaternion(Vector4CPU &v)
{
	Vector4CPU result;

	float radius_half = v.w * 0.5f;
	float sinvalue, cosvalue;
	FastMath::SinCos(radius_half, sinvalue, cosvalue);

	result.x = v.x * sinvalue;
	result.y = v.y * sinvalue;
	result.z = v.z * sinvalue;
	result.w = cosvalue;

	return result;
}

inline Vector4CPU PointToPlaneDistance(Vector4CPU &point, Vector4CPU &plane)
{
	return VectorDot(point, plane);
}

inline Vector4CPU MirrorPoint(Vector4CPU &point, Vector4CPU &plane)
{
	Vector4CPU mirrored_point;
	
	float d = PointToPlaneDistance(point, plane)[0];
	mirrored_point = point - (2.0f * d ) * plane;

	return mirrored_point;
}

// 計算vector對plane平面鏡射後的3D方向
inline Vector4CPU MirrorVector(Vector4CPU &vector, Vector4CPU &plane)
{
	Vector4CPU mirrored_a, mirrored_b, mirrored_vector;
	Vector4CPU vZero = Vector4CPU::GetZero();

	float d = PointToPlaneDistance(vector, plane)[0];
	mirrored_a = vector - (2.0f * d ) * plane;

	d = plane[3];
	mirrored_b = vZero - (2.0f * d ) * plane;
	
	mirrored_vector = mirrored_a - mirrored_b;
	mirrored_vector[3] = 1.0f;

	return mirrored_vector;
}

#endif // _VECTOR4_SSE_