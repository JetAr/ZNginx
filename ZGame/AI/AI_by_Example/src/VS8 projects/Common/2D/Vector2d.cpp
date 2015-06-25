#include "Vector2D.h"
#include <fstream>

//z 2015-06-25 16:18:02 L.189'27718 T1812838133.K 向stream写入和读取 Vector2D
std::ostream& operator<<(std::ostream& os, const Vector2D& rhs)
{
    os << " " << rhs.x << " " << rhs.y;

    return os;
}


std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs)
{
    is >> lhs.x >> lhs.y;

    return is;
}

