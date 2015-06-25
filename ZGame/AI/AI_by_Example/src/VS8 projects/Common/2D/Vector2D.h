#ifndef S2DVECTOR_H
#define S2DVECTOR_H
//------------------------------------------------------------------------
//
//  Name:   Vector2D.h
//
//  Desc:   2D vector struct
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <math.h>
#include <windows.h>
#include <iosfwd>
#include <limits>
#include "misc/utils.h"

//z 2015-06-25 15:10:25 L.189'31775 T521082737 .K
struct Vector2D
{
    double x;
    double y;
    
    //z ctor
    Vector2D():x(0.0),y(0.0) {}
    Vector2D(double a, double b):x(a),y(b) {}

    //sets x and y to zero
    //z 置零
    void Zero()
    {
        x=0.0;
        y=0.0;
    }

    //returns true if both x and y are zero
    //z 是否在原点（零值）
    bool isZero()const
    {
        return (x*x + y*y) < MinDouble;
    }

    //returns the length of the vector
    //z 返回矢量长度
    inline double    Length()const;

    //returns the squared length of the vector (thereby avoiding the sqrt)
    //z 返回矢量长度的平方
    inline double    LengthSq()const;

    //z 归一化
    inline void      Normalize();
    //z 点积
    inline double    Dot(const Vector2D& v2)const;

    //z 判断方向，是顺时针还是逆时针
    //z x方向向右，y方向向下
    //returns positive if v2 is clockwise of this vector,
    //negative if anticlockwise (assuming the Y axis is pointing down,
    //X axis to right like a Window app)
    inline int       Sign(const Vector2D& v2)const;
    
    //z perpendicular 垂直；成直角
    //returns the vector that is perpendicular to this one.
    inline Vector2D  Perp()const;

    //adjusts x and y so that the length of the vector does not exceed max
    //z 调整 x 和 y 以使得矢量的长度不超过 max 
    inline void      Truncate(double max);

    //z 求两个矢量的距离
    //returns the distance between this vector and th one passed as a parameter
    inline double    Distance(const Vector2D &v2)const;

    //squared version of above.
    //z 两个矢量距离的平方
    inline double    DistanceSq(const Vector2D &v2)const;

    //z 反射
    inline void      Reflect(const Vector2D& norm);

    //returns the vector that is the reverse of this vector
    //z 取反
    inline Vector2D  GetReverse()const;


    //we need some overloaded operators
    const Vector2D& operator+=(const Vector2D &rhs)
    {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    const Vector2D& operator-=(const Vector2D &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    //z 数乘
    const Vector2D& operator*=(const double& rhs)
    {
        x *= rhs;
        y *= rhs;

        return *this;
    }

    const Vector2D& operator/=(const double& rhs)
    {
        x /= rhs;
        y /= rhs;

        return *this;
    }

    bool operator==(const Vector2D& rhs)const
    {
        return (isEqual(x, rhs.x) && isEqual(y,rhs.y) );
    }

    bool operator!=(const Vector2D& rhs)const
    {
        return (x != rhs.x) || (y != rhs.y);
    }

};

//-----------------------------------------------------------------------some more operator overloads
inline Vector2D operator*(const Vector2D &lhs, double rhs);
inline Vector2D operator*(double lhs, const Vector2D &rhs);
inline Vector2D operator-(const Vector2D &lhs, const Vector2D &rhs);
inline Vector2D operator+(const Vector2D &lhs, const Vector2D &rhs);
inline Vector2D operator/(const Vector2D &lhs, double val);
std::ostream& operator<<(std::ostream& os, const Vector2D& rhs);
std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs);


//------------------------------------------------------------------------member functions

//------------------------- Length ---------------------------------------
//
//  returns the length of a 2D vector
//------------------------------------------------------------------------
inline double Vector2D::Length()const
{
    return sqrt(x * x + y * y);
}


//------------------------- LengthSq -------------------------------------
//
//  returns the squared length of a 2D vector
//------------------------------------------------------------------------
inline double Vector2D::LengthSq()const
{
    return (x * x + y * y);
}


//------------------------- Vec2DDot -------------------------------------
//
//  calculates the dot product
//z 点积
//------------------------------------------------------------------------
inline double Vector2D::Dot(const Vector2D &v2)const
{
    return x*v2.x + y*v2.y;
}

//------------------------ Sign ------------------------------------------
//
//  returns positive if v2 is clockwise of this vector,
//  minus if anticlockwise (Y axis pointing down, X axis to right)
//------------------------------------------------------------------------
enum {clockwise = 1, anticlockwise = -1};

//z 看起来是在比较矢量斜率
//z v2相对this是顺时针还是逆时针
inline int Vector2D::Sign(const Vector2D& v2)const
{
    if (y*v2.x > x*v2.y)
    {
        return anticlockwise;
    }
    else
    {
        return clockwise;
    }
}

//------------------------------ Perp ------------------------------------
//
//  Returns a vector perpendicular to this vector
//------------------------------------------------------------------------
//z 2015-06-25 16:53:56 L.189'25564 T1817402301.K
//z 2维平面上与x，y垂直的线有俩，(y,-x) 以及 (-y,x) ，两个。
//z 这里使用了其中一个
inline Vector2D Vector2D::Perp()const
{
    return Vector2D(-y, x);
}

//------------------------------ Distance --------------------------------
//
//  calculates the euclidean distance between two vectors
//z 2015-06-25 16:54:47 L.189'25513 T1817438206.K
//z 计算两个矢量的欧拉距离
//------------------------------------------------------------------------
inline double Vector2D::Distance(const Vector2D &v2)const
{
    double ySeparation = v2.y - y;
    double xSeparation = v2.x - x;

    return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}


//------------------------------ DistanceSq ------------------------------
//
//  calculates the euclidean distance squared between two vectors
//------------------------------------------------------------------------
inline double Vector2D::DistanceSq(const Vector2D &v2)const
{
    double ySeparation = v2.y - y;
    double xSeparation = v2.x - x;

    return ySeparation*ySeparation + xSeparation*xSeparation;
}

//----------------------------- Truncate ---------------------------------
//
//  truncates a vector so that its length does not exceed max
//------------------------------------------------------------------------
inline void Vector2D::Truncate(double max)
{
    //z 如果长度大于max
    if (this->Length() > max)
    {
        //z 首先归一化
        this->Normalize();

        //z 而后乘以max，使得长度最多为max
        *this *= max;
    }
}

//--------------------------- Reflect ------------------------------------
//
//  given a normalized vector this method reflects the vector it
//  is operating upon. (like the path of a ball bouncing off a wall)
//------------------------------------------------------------------------
inline void Vector2D::Reflect(const Vector2D& norm)
{
    *this += 2.0 * this->Dot(norm) * norm.GetReverse();
}

//----------------------- GetReverse ----------------------------------------
//
//  returns the vector that is the reverse of this vector
//z 对矢量取反，由(x,y)变成(-x,－y)。
//------------------------------------------------------------------------
inline Vector2D Vector2D::GetReverse()const
{
    return Vector2D(-this->x, -this->y);
}


//------------------------- Normalize ------------------------------------
//
//  normalizes a 2D Vector
//z 对矢量进行归一化
//------------------------------------------------------------------------
inline void Vector2D::Normalize()
{
    //z 先求矢量长度
    double vector_length = this->Length();
    
    //z 在大于epsilon的情况下
    if (vector_length > std::numeric_limits<double>::epsilon())
    {
        this->x /= vector_length;
        this->y /= vector_length;
    }
}


//------------------------------------------------------------------------non member functions

inline Vector2D Vec2DNormalize(const Vector2D &v)
{
    Vector2D vec = v;

    double vector_length = vec.Length();

    if (vector_length > std::numeric_limits<double>::epsilon())
    {
        vec.x /= vector_length;
        vec.y /= vector_length;
    }

    return vec;
}


inline double Vec2DDistance(const Vector2D &v1, const Vector2D &v2)
{

    double ySeparation = v2.y - v1.y;
    double xSeparation = v2.x - v1.x;

    return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}

inline double Vec2DDistanceSq(const Vector2D &v1, const Vector2D &v2)
{

    double ySeparation = v2.y - v1.y;
    double xSeparation = v2.x - v1.x;

    return ySeparation*ySeparation + xSeparation*xSeparation;
}

inline double Vec2DLength(const Vector2D& v)
{
    return sqrt(v.x*v.x + v.y*v.y);
}

inline double Vec2DLengthSq(const Vector2D& v)
{
    return (v.x*v.x + v.y*v.y);
}


inline Vector2D POINTStoVector(const POINTS& p)
{
    return Vector2D(p.x, p.y);
}

inline Vector2D POINTtoVector(const POINT& p)
{
    return Vector2D((double)p.x, (double)p.y);
}

inline POINTS VectorToPOINTS(const Vector2D& v)
{
    POINTS p;
    p.x = (short)v.x;
    p.y = (short)v.y;

    return p;
}

inline POINT VectorToPOINT(const Vector2D& v)
{
    POINT p;
    p.x = (long)v.x;
    p.y = (long)v.y;

    return p;
}



//------------------------------------------------------------------------operator overloads
inline Vector2D operator*(const Vector2D &lhs, double rhs)
{
    Vector2D result(lhs);
    result *= rhs;
    return result;
}

inline Vector2D operator*(double lhs, const Vector2D &rhs)
{
    Vector2D result(rhs);
    result *= lhs;
    return result;
}

//overload the - operator
inline Vector2D operator-(const Vector2D &lhs, const Vector2D &rhs)
{
    Vector2D result(lhs);
    result.x -= rhs.x;
    result.y -= rhs.y;

    return result;
}

//overload the + operator
inline Vector2D operator+(const Vector2D &lhs, const Vector2D &rhs)
{
    Vector2D result(lhs);
    result.x += rhs.x;
    result.y += rhs.y;

    return result;
}

//overload the / operator
inline Vector2D operator/(const Vector2D &lhs, double val)
{
    Vector2D result(lhs);
    result.x /= val;
    result.y /= val;

    return result;
}

///////////////////////////////////////////////////////////////////////////////


//treats a window as a toroid
inline void WrapAround(Vector2D &pos, int MaxX, int MaxY)
{
    if (pos.x > MaxX)
    {
        pos.x = 0.0;
    }

    if (pos.x < 0)
    {
        pos.x = (double)MaxX;
    }

    if (pos.y < 0)
    {
        pos.y = (double)MaxY;
    }

    if (pos.y > MaxY)
    {
        pos.y = 0.0;
    }
}

//returns true if the point p is not inside the region defined by top_left
//and bot_rgt
inline bool NotInsideRegion(Vector2D p,
                            Vector2D top_left,
                            Vector2D bot_rgt)
{
    return (p.x < top_left.x) || (p.x > bot_rgt.x) ||
           (p.y < top_left.y) || (p.y > bot_rgt.y);
}

inline bool InsideRegion(Vector2D p,
                         Vector2D top_left,
                         Vector2D bot_rgt)
{
    return !((p.x < top_left.x) || (p.x > bot_rgt.x) ||
             (p.y < top_left.y) || (p.y > bot_rgt.y));
}

inline bool InsideRegion(Vector2D p, int left, int top, int right, int bottom)
{
    return !( (p.x < left) || (p.x > right) || (p.y < top) || (p.y > bottom) );
}

//------------------ isSecondInFOVOfFirst -------------------------------------
//
//  returns true if the target position is in the field of view of the entity
//  positioned at posFirst facing in facingFirst
//-----------------------------------------------------------------------------
inline bool isSecondInFOVOfFirst(Vector2D posFirst,
                                 Vector2D facingFirst,
                                 Vector2D posSecond,
                                 double    fov)
{
    Vector2D toTarget = Vec2DNormalize(posSecond - posFirst);

    return facingFirst.Dot(toTarget) >= cos(fov/2.0);
}





#endif
