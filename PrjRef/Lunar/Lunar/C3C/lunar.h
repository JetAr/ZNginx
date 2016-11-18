#ifndef LUNAR
#define LUNAR

//test
class lunDate
{
public:
    lunDate(int y,int m,int d,int calType);
    double date;
    char* syzygyType;
    double syzygyTime;
    double ecliType;
    double ecliTime;
};
class lunarCal
{
public:
    lunarCal(int y,int m,int d,int calType);
    double date;
    char* syzygyType;
    double syzygyTime;
    double ecliType;
    double ecliTime;
    double month;
};
#endif
