#include "Obstacle.h"
#include <fstream>

void Obstacle::Write(std::ostream& os)const
{
    os << std::endl;
    os << EntityType() << ",";//z 写入了实体类型，位置和半径
    os << Pos() << ",";
    os << BRadius();
}

//z 2015-07-02 11:03 读入坐标和半径，并进行设置。
void Obstacle::Read(std::ifstream& in)
{
    double x, y, r;

    in >> x >> y >> r;

    SetPos(Vector2D(x,y));

    SetBRadius(r);
}
