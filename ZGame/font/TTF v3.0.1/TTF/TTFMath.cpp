// *************************************************************************************
//	TTFMath.cpp
//
//		Implementation of the TTF math library.
//
//																															Ryan Bogean
//																															April 2012
//																															January 2014
//
// *************************************************************************************

# include <cstdint>
# include <algorithm>

# include "TTFExceptions.h"
# include "TTFMath.h"


using namespace TTFCore;


// ---------------------------------------------------------------------------------------------------------------------------
//	floating point types
// ---------------------------------------------------------------------------------------------------------------------------

// ----- vec2f -----
vec2f::vec2f() {}
vec2f::vec2f(float x_, float y_) : x(x_), y(y_) {}

bool vec2f::operator==(vec2f v) const
{
    return x == v.x && y == v.y;
}
bool vec2f::operator!=(vec2f v) const
{
    return x != v.x || y != v.y;
}

vec2f& vec2f::operator+=(vec2f v)
{
    x += v.x;
    y += v.y;
    return *this;
}

vec2f& vec2f::operator-=(vec2f v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

vec2f& vec2f::operator*=(vec2f v)
{
    x *= v.x;
    y *= v.y;
    return *this;
}

vec2f& vec2f::operator/=(vec2f v)
{
    x /= v.x;
    y /= v.y;
    return *this;
}

vec2f& vec2f::operator+=(float f)
{
    x += f;
    y += f;
    return *this;
}

vec2f& vec2f::operator-=(float f)
{
    x -= f;
    y -= f;
    return *this;
}

vec2f& vec2f::operator*=(float f)
{
    x *= f;
    y *= f;
    return *this;
}

vec2f& vec2f::operator/=(float f)
{
    x /= f;
    y /= f;
    return *this;
}

vec2f TTFCore::operator+(vec2f v0, vec2f v1)
{
    return vec2f(v0.x + v1.x, v0.y + v1.y);
}

vec2f TTFCore::operator-(vec2f v0, vec2f v1)
{
    return vec2f(v0.x - v1.x, v0.y - v1.y);
}

vec2f TTFCore::operator*(vec2f v0, vec2f v1)
{
    return vec2f(v0.x * v1.x, v0.y * v1.y);
}

vec2f TTFCore::operator/(vec2f v0, vec2f v1)
{
    return vec2f(v0.x / v1.x, v0.y / v1.y);
}

vec2f TTFCore::operator+(float f, vec2f v1)
{
    return vec2f(f + v1.x, f + v1.y);
}

vec2f TTFCore::operator-(float f, vec2f v1)
{
    return vec2f(f - v1.x, f - v1.y);
}

vec2f TTFCore::operator*(float f, vec2f v1)
{
    return vec2f(f * v1.x, f * v1.y);
}

vec2f TTFCore::operator/(float f, vec2f v1)
{
    return vec2f(f / v1.x, f / v1.y);
}

vec2f TTFCore::operator+(vec2f v0, float f)
{
    return vec2f(v0.x + f, v0.y + f);
}

vec2f TTFCore::operator-(vec2f v0, float f)
{
    return vec2f(v0.x - f, v0.y - f);
}

vec2f TTFCore::operator*(vec2f v0, float f)
{
    return vec2f(v0.x * f, v0.y * f);
}

vec2f TTFCore::operator/(vec2f v0, float f)
{
    return vec2f(v0.x / f, v0.y / f);
}

float TTFCore::dot(vec2f v0, vec2f v1)
{
    return(v0.x*v1.x) + (v0.y*v1.y);
}

float TTFCore::cross(vec2f v0, vec2f v1)
{
    return(v0.x*v1.y) - (v0.y*v1.x);
}


// ----- matrix3x2f -----
matrix3x2f::matrix3x2f() : a(1.0f), b(0.0f), c(0.0f), d(1.0f), e(0.0f), f(0.0f) {}

vec2f TTFCore::mul(vec2f v, const matrix3x2f& m)
{
    // obviously you can't multiply a 2d vector with a 3x2 matrix
    // in this case we 'extend' the 2d vector to a 3d vector by appending 1.0f
    // course this need'nt be done explicitly

    return vec2f( v.x*m.a + v.y*m.c + m.e, v.x*m.b + v.y*m.d + m.f );
}

vec2f TTFCore::normal(vec2f v)
{
    return v / std::sqrt(dot(v,v));
}

vec2f TTFCore::lerp(vec2f p0, vec2f p1, float f)
{
    if (f <= 0.0f) return p0;
    if (f >= 1.0f) return p1;
    return p0 + ((p1 - p0) * f);
}

vec2f TTFCore::quad_lerp(vec2f p0, vec2f p1, vec2f p2, float f)
{
    if (f <= 0.0f) return p0;
    if (f >= 1.0f) return p1;
    vec2f t0 = p0 + ((p1 - p0) * f);
    vec2f t1 = p1 + ((p2 - p1) * f);
    return t0 + ((t1 - t0) * f);
}


// ---------------------------------------------------------------------------------------------------------------------------
//	vec3f / vec4f
// ---------------------------------------------------------------------------------------------------------------------------

// ----- vec3f -----
vec3f::vec3f() {}
vec3f::vec3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

vec3f TTFCore::normal(vec3f v)
{
    float w = 1.0f / std::sqrt(v.x * v.x + v.y + v.y + v.z*v.z);
    return vec3f(v.x * w, v.y * w, v.z * w);
}

// ----- vec4f -----
vec4f::vec4f() {}
vec4f::vec4f(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}


// ---------------------------------------------------------------------------------------------------------------------------
//	interger functions/types
// ---------------------------------------------------------------------------------------------------------------------------

// ----- vec2t -----
vec2t::vec2t() {}
vec2t::vec2t(int16_t x_, int16_t y_) : x(x_), y(y_) {}

bool vec2t::operator==(vec2t v) const
{
    return x == v.x && y == v.y;
}
bool vec2t::operator!=(vec2t v) const
{
    return x != v.x || y != v.y;
}

vec2t& vec2t::operator+=(vec2t v)
{
    x += v.x;
    y += v.y;
    return *this;
}

vec2t& vec2t::operator-=(vec2t v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

vec2t& vec2t::operator*=(vec2t v)
{
    x *= v.x;
    y *= v.y;
    return *this;
}

vec2t& vec2t::operator/=(vec2t v)
{
    x /= v.x;
    y /= v.y;
    return *this;
}

vec2t& vec2t::operator+=(int16_t v)
{
    x += v;
    y += v;
    return *this;
}

vec2t& vec2t::operator-=(int16_t v)
{
    x -= v;
    y -= v;
    return *this;
}

vec2t& vec2t::operator*=(int16_t v)
{
    x *= v;
    y *= v;
    return *this;
}

vec2t& vec2t::operator/=(int16_t v)
{
    x /= v;
    y /= v;
    return *this;
}

vec2t TTFCore::operator+(vec2t v0, vec2t v1)
{
    return vec2t(v0.x + v1.x, v0.y + v1.y);
}

vec2t TTFCore::operator-(vec2t v0, vec2t v1)
{
    return vec2t(v0.x - v1.x, v0.y - v1.y);
}

vec2t TTFCore::operator*(vec2t v0, vec2t v1)
{
    return vec2t(v0.x * v1.x, v0.y * v1.y);
}

vec2t TTFCore::operator/(vec2t v0, vec2t v1)
{
    return vec2t(v0.x / v1.x, v0.y / v1.y);
}

vec2t TTFCore::operator+(int16_t v, vec2t v1)
{
    return vec2t(v + v1.x, v + v1.y);
}

vec2t TTFCore::operator-(int16_t v, vec2t v1)
{
    return vec2t(v - v1.x, v - v1.y);
}

vec2t TTFCore::operator*(int16_t v, vec2t v1)
{
    return vec2t(v * v1.x, v * v1.y);
}

vec2t TTFCore::operator/(int16_t v, vec2t v1)
{
    return vec2t(v / v1.x, v / v1.y);
}

vec2t TTFCore::operator+(vec2t v0, int16_t v)
{
    return vec2t(v0.x + v, v0.y + v);
}

vec2t TTFCore::operator-(vec2t v0, int16_t v)
{
    return vec2t(v0.x - v, v0.y - v);
}

vec2t TTFCore::operator*(vec2t v0, int16_t v)
{
    return vec2t(v0.x * v, v0.y * v);
}

vec2t TTFCore::operator/(vec2t v0, int16_t v)
{
    return vec2t(v0.x / v, v0.y / v);
}

int32_t TTFCore::dot(vec2t v0, vec2t v1)
{

    int32_t a = v0.x;
    int32_t b = v0.y;
    int32_t c = v1.x;
    int32_t d = v1.y;

    return(a*c) + (b*d);
}

int32_t TTFCore::cross(vec2t v0, vec2t v1)
{

    int32_t a = v0.x;
    int32_t b = v0.y;
    int32_t c = v1.x;
    int32_t d = v1.y;

    return(a*d) - (b*c);
}


// ----- matrix3x2t -----
matrix3x2t::matrix3x2t()
{
    a = 1 << 16;
    b = 0;
    c = 0;
    d = 1 << 16;
    e = 0;
    f = 0;
}

int32_t TTFCore::fxmul(int32_t a, int32_t b)
{
    // 16.16 x 16.16 fixed point multiply
    int64_t r = (static_cast<int64_t>(a) * static_cast<int64_t>(b)) >> 16;
    return static_cast<int32_t>(r);
}

matrix3x2t TTFCore::mul(const matrix3x2t& m0, const matrix3x2t& m1)
{

    matrix3x2t m;
    m.a = fxmul(m0.a, m1.a) + fxmul(m0.b, m1.c);
    m.b = fxmul(m0.a, m1.b) + fxmul(m0.b, m1.d);
    m.c = fxmul(m0.c, m1.a) + fxmul(m0.d, m1.c);
    m.d = fxmul(m0.c, m1.b) + fxmul(m0.d, m1.d);
    m.e = m0.e + m1.e;
    m.f = m0.f + m1.f;

    return m;
}

vec2t TTFCore::mul(vec2t v, const matrix3x2t& m)
{

    const int32_t round = 1 << 15;

    int32_t x = v.x;
    int32_t y = v.y;

    return vec2t(
               static_cast<int16_t>((x*m.a + y*m.c + m.e + round) >> 16),
               static_cast<int16_t>((x*m.b + y*m.d + m.f + round) >> 16)
           );
}


// ----- vec3t -----
vec3t::vec3t() {}

vec3t::vec3t(int16_t x_, int16_t y_, int16_t z_)
{
    x = x_;
    y = y_;
    z = z_;
}

vec3t::vec3t(vec2t v)
{
    x = v.x;
    y = v.y;
    z = 0;
}

vec3t::operator vec3f() const
{
    return vec3f(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

vec3t::operator vec2t() const
{
    return vec2t(x, y);
}

bool TTFCore::operator==(vec3t a, vec3t b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
bool TTFCore::operator!=(vec3t a, vec3t b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

vec3t& TTFCore::operator+=(vec3t& a, vec3t b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

vec3t& TTFCore::operator-=(vec3t& a, vec3t b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

vec3t& TTFCore::operator*=(vec3t& a, vec3t b)
{
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    return a;
}

vec3t& TTFCore::operator/=(vec3t& a, vec3t b)
{
    a.x /= b.x;
    a.y /= b.y;
    a.z /= b.z;
    return a;
}

vec3t& TTFCore::operator+=(vec3t& a, int16_t b)
{
    a.x += b;
    a.y += b;
    a.z += b;
    return a;
}

vec3t& TTFCore::operator-=(vec3t& a, int16_t b)
{
    a.x -= b;
    a.y -= b;
    a.z -= b;
    return a;
}

vec3t& TTFCore::operator*=(vec3t& a, int16_t b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    return a;
}

vec3t& TTFCore::operator/=(vec3t& a, int16_t b)
{
    a.x /= b;
    a.y /= b;
    a.z /= b;
    return a;
}

vec3t TTFCore::operator+(vec3t a, vec3t b)
{
    return vec3t(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3t TTFCore::operator-(vec3t a, vec3t b)
{
    return vec3t(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec3t TTFCore::operator*(vec3t a, vec3t b)
{
    return vec3t(a.x * b.x, a.y * b.y, a.z * b.z);
}

vec3t TTFCore::operator/(vec3t a, vec3t b)
{
    return vec3t(a.x / b.x, a.y / b.y, a.z / b.z);
}

vec3t TTFCore::operator+(int16_t a, vec3t b)
{
    return vec3t(a + b.x, a + b.y, a + b.z);
}

vec3t TTFCore::operator-(int16_t a, vec3t b)
{
    return vec3t(a - b.x, a - b.y, a - b.z);
}

vec3t TTFCore::operator*(int16_t a, vec3t b)
{
    return vec3t(a * b.x, a * b.y, a * b.z);
}

vec3t TTFCore::operator/(int16_t a, vec3t b)
{
    return vec3t(a / b.x, a / b.y, a / b.z);
}

vec3t TTFCore::operator+(vec3t a, int16_t b)
{
    return vec3t(a.x + b, a.y + b, a.z + b);
}

vec3t TTFCore::operator-(vec3t a, int16_t b)
{
    return vec3t(a.x - b, a.y - b, a.z - b);
}

vec3t TTFCore::operator*(vec3t a, int16_t b)
{
    return vec3t(a.x * b, a.y * b, a.z * b);
}

vec3t TTFCore::operator/(vec3t a, int16_t b)
{
    return vec3t(a.x / b, a.y / b, a.z / b);
}

int32_t TTFCore::dot(vec3t a, vec3t b)
{
    return a.x * b.x + a.y *b.y + a.z * b.z;
}

vec3t TTFCore::mul(vec3t v, const matrix3x2t& m)
{
    // not really a true multiply

    const int32_t round = 1 << 15;

    int32_t x = v.x;
    int32_t y = v.y;

    return vec3t(
               static_cast<int16_t>((x*m.a + y*m.c + m.e + round) >> 16),
               static_cast<int16_t>((x*m.b + y*m.d + m.f + round) >> 16),
               v.z
           );
}


// ----- vec4t -----
vec4t::vec4t() {}

vec4t::vec4t(int16_t x_, int16_t y_, int16_t z_, int16_t w_)
{
    x = x_;
    y = y_;
    z = z_;
    w = w_;
}


// ----- various functions -----
int32_t TTFCore::Abs(int32_t x)
{
    return (x < 0) ? -x : x;
}

int32_t TTFCore::IntSqrt(int32_t m)
{
    // by Tristan Muntsinger from http://www.codecodex.com/wiki/Calculate_an_integer_square_root

    if (m <= 0) return 0;
    uint32_t n = static_cast<uint32_t>(m);
    uint32_t c = 0x8000;
    uint32_t g = 0x8000;

    for(;;)
    {
        if(g*g > n) g ^= c;
        c >>= 1;
        if(c == 0) return static_cast<int32_t>(g);
        g |= c;
    }
}

int16_t TTFCore::RoundBy16(int32_t x)
{
    const int32_t round = 1 << 15;
    if(x >= 0) return static_cast<int16_t>((x + round) >> 16);
    else return static_cast<int16_t>((x - round) >> 16);
}

int32_t TTFCore::RoundBy16Large(int32_t x)
{
    const int32_t round = 1 << 15;
    if(x >= 0) return(x + round) >> 16;
    else return(x - round) >> 16;
}

int16_t TTFCore::RoundBy2(int16_t x)
{
    if(x >= 0) return(x + 1) >> 1;
    else return(x - 1) >> 1;
}

vec2t TTFCore::RoundBy2(vec2t v)
{
    return vec2t(RoundBy2(v.x), RoundBy2(v.y));
}


// ---------------------------------------------------------------------------------------------------------------------------
//	triangulation helpers
// ---------------------------------------------------------------------------------------------------------------------------

// ----- vec2t -----
int32_t TTFCore::GetTriSign(vec2t p0, vec2t p1, vec2t p2)
{
    // returns the the winding of the triangle (p0 -> p1 -> p2)
    // +result = CCW rotation = outer, -result = CW rotation = inner
    vec2t e0 = p2 - p0;
    vec2t e1 = p1 - p0;
    return cross(e0,e1);			// use the '2d' cross product to determine sign
}

bool TTFCore::IsContained(vec2t p0, vec2t p1, vec2t p2, vec2t p3)
{
    // returns true if the line p0->p3 is between, to the right of p0->p1 and to the left of p0->p2, the lines p0->p1 and p0->p2
    // if p0->p3 is colinear with either p0->p1 or p0->p2 returns false
    // p0 should be at the middle, with p1, p2, and p3 pointing away
    // if any of the lines (p0,p1), (p0,p2), or (p0,p3) are null (0,0) then IsContained will return false

    // create vectors
    vec2t v1 = p1 - p0;
    vec2t v2 = p2 - p0;
    vec2t v3 = p3 - p0;

    // get cross products
    int32_t c1x2 = cross(v1, v2);
    int32_t c1x3 = cross(v1, v3);
    int32_t c2x3 = cross(v2, v3);

    // determine containment
    if (c1x2 < 0)			return c1x3 < 0 && c2x3 > 0;										// RRL
    else if (c1x2 > 0)	return c1x3 < 0 || c2x3 > 0;											// LRX or LXL
    else						return c1x3 < 0 && c2x3 > 0 && dot(v1, v2) < 0;			// 0RL (if v1 and v2 are directed in the same direction, dot product is positive, the result is degenerate and no vector could be inside)
}

bool TTFCore::Intersect2D(vec2t p0, vec2t p1, vec2t p2, vec2t p3)
{
    // returns true if the line segments (p0, p1) and (p2, p3) intersect
    // null lines return false
    // identical lines that are non-null always return true
    // lines intersection at the end points returns false, but a line vertex intersection is true
    // this means lines can terminate at vertices, just can't pass through them

    // the algorithm in short:
    // line 1 = p + t.r, p is initial point, t is parameter, r is direction vector
    // line 2 = q + u.s
    // which gives us p + t.r = q + u.s
    // solving for t = (qp x s) / (r x s)
    // solving for u = (qp x r) / (r x s)

    // check null lines
    if (p0 == p1 || p2 == p3) return false;				// return false if either line segment is null

    // check for identical lines
    // logically I would place this within the (rxs == 0) test
    // but for performance sake this is 'hoisted' out and above
    if ((p0 == p2 && p1 == p3) || (p0 == p3 && p1 == p2)) return true;	// lines are identical

    // initialize variables
    vec2t r = p1 - p0;
    vec2t s = p3 - p2;
    int32_t rxs = cross(r, s);
    vec2t qp = p2 - p0;
    int32_t tt = cross(qp, s);
    int32_t ut = cross(qp, r);

    // new interesection test
    if (rxs == 0)  																								// parallel
    {
        if (tt == 0 && ut == 0)  																			// colinear
        {
            int32_t qpr = dot(qp, r);
            int32_t qps = dot(qp, s);
            return (qpr > 0 && qpr < dot(r, r)) || (qps > 0 && qps < dot(s, s));		// intersect
        }
        else return false;
    }
    else
    {

        // check for vertex sharing
        // shared vertex and not colinear -> no way they can intersect
        if (p0 == p2 || p0 == p3 || p1 == p2 || p1 == p3) return false;

        // we use an inclusive check here (>= not >, and <= not <) so that T-like intersections return true
        // the case where lines intersect only at the vertex is aready covereted above
        if (rxs > 0) return tt >= 0 && tt <= rxs && ut >= 0 && ut <= rxs;
        else return tt <= 0 && tt >= rxs && ut <= 0 && ut >= rxs;
    }

    // old intersection test
    // didn't handle identical lines properly, didn't handle T's properly, was slower
    //if (rxs > 0)		return tt > 0 && tt < rxs && ut > 0 && ut < rxs;
    //else if (rxs < 0)	return tt < 0 && tt > rxs && ut < 0 && ut > rxs;
    //else					return tt == 0 && ut == 0 && Intersect1D(p0, p1, p2, p3);
}

int32_t TTFCore::SqrdLen(vec2t p0, vec2t p1)
{
    // calculates the squared length (ie. doesn't use the square root)
    vec2t v = p1 - p0;
    return dot(v, v);
}

vec2t TTFCore::MidPoint(vec2t p0, vec2t p1)
{
    return (p0 + p1) / 2;
}

vec2t TTFCore::ScaledNormal(vec2t v, int16_t w)
{
    // a bit verbose because we perform intermediate calculations with 32-bit integers
    int32_t x = v.x;
    int32_t y = v.y;
    int32_t len = IntSqrt(x*x + y*y);
    x = (x * static_cast<int32_t>(w)) / len;
    y = (y * static_cast<int32_t>(w)) / len;
    return vec2t(static_cast<int16_t>(x), static_cast<int16_t>(y));
}

vec2t TTFCore::ScaledPerpCCW(vec2t v, int16_t w)
{
    return ScaledNormal(vec2t(-v.y, v.x), w);
}

vec2t TTFCore::ScaledPerpCCW(vec2t p0, vec2t p1, int16_t w)
{
    return ScaledPerpCCW(p1 - p0, w);
}

vec2t TTFCore::ScaledPerpCCW(vec2t p0, vec2t p1, vec2t p2, int16_t w)
{
    // returns the average of the perpendicular vectors of p0 -> p1, p1 -> p2, scaled by w
    vec2t n0 = ScaledPerpCCW(p0, p1, 256);		// large number, but not too large, this gives us ~8 bits of fractional precision
    vec2t n1 = ScaledPerpCCW(p1, p2, 256);
    return ScaledNormal((n0 + n1) / 2, w);
}

vec2t TTFCore::lerp(vec2t p0, vec2t p1, uint16_t f)
{
    int32_t dx = static_cast<int32_t>(p1.x - p0.x) * static_cast<int32_t>(f);
    int32_t dy = static_cast<int32_t>(p1.y - p0.y) * static_cast<int32_t>(f);
    return vec2t(p0.x + RoundBy16(dx), p0.y + RoundBy16(dy));
}

vec2t TTFCore::quad_lerp(vec2t p0, vec2t p1, vec2t p2, uint16_t f)
{
    return lerp(lerp(p0,p1,f),lerp(p1,p2,f),f);
}


// ----- vec3t -----
int32_t TTFCore::GetTriSign(vec3t p0, vec3t p1, vec3t p2)
{
    return GetTriSign(static_cast<vec2t>(p0), static_cast<vec2t>(p1), static_cast<vec2t>(p2));
}

bool TTFCore::IsContained(vec3t p0, vec3t p1, vec3t p2, vec3t p3)
{
    return IsContained(static_cast<vec2t>(p0), static_cast<vec2t>(p1), static_cast<vec2t>(p2), static_cast<vec2t>(p3));
}

bool TTFCore::Intersect2D(vec3t p0, vec3t p1, vec3t p2, vec3t p3)
{
    return Intersect2D(static_cast<vec2t>(p0), static_cast<vec2t>(p1), static_cast<vec2t>(p2), static_cast<vec2t>(p3));
}

int32_t TTFCore::SqrdLen(vec3t p0, vec3t p1)
{
    return SqrdLen(static_cast<vec2t>(p0), static_cast<vec2t>(p1));
}

vec3t TTFCore::MidPoint(vec3t p0, vec3t p1)
{
    return MidPoint(static_cast<vec2t>(p0), static_cast<vec2t>(p1));
}

vec3t TTFCore::ScaledNormal(vec3t v, int16_t w)
{
    return ScaledNormal(static_cast<vec2t>(v), w);
}

vec3t TTFCore::ScaledPerpCCW(vec3t v, int16_t w)
{
    return ScaledPerpCCW(static_cast<vec2t>(v), w);
}

vec3t TTFCore::ScaledPerpCCW(vec3t p0, vec3t p1, int16_t w)
{
    return ScaledPerpCCW(static_cast<vec2t>(p0), static_cast<vec2t>(p1), w);
}

vec3t TTFCore::ScaledPerpCCW(vec3t p0, vec3t p1, vec3t p2, int16_t w)
{
    return ScaledPerpCCW(static_cast<vec2t>(p0), static_cast<vec2t>(p1), static_cast<vec2t>(p2), w);
}

vec3t TTFCore::lerp(vec3t p0, vec3t p1, uint16_t f)
{
    int32_t dx = static_cast<int32_t>(p1.x - p0.x) * static_cast<int32_t>(f);
    int32_t dy = static_cast<int32_t>(p1.y - p0.y) * static_cast<int32_t>(f);
    int32_t dz = static_cast<int32_t>(p1.z - p0.z) * static_cast<int32_t>(f);
    return vec3t(p0.x + RoundBy16(dx), p0.y + RoundBy16(dy), p0.z + RoundBy16(dz));
}

vec3t TTFCore::quad_lerp(vec3t p0, vec3t p1, vec3t p2, uint16_t f)
{
    return lerp(lerp(p0,p1,f),lerp(p1,p2,f),f);
}
