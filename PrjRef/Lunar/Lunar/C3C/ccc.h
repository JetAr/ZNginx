#ifndef CCC
#define CCC

class sTermClass
{
public:
    sTermClass(int year,int month,int date,int time,int zone,int calType,int monthLength);
    char sTermInMonth[2][32];
    char sTermName[32];
    char sTermTime[32];
};
class lunarClass
{
public:
    lunarClass(int year,int month,int date,int calType);
    int lunarDate;
    char lunarDate_Str[32];
    int lunarMonth;
    char lunarMonth_Str[32];
    char syzygyType[32];
    int syzygyTime;
    char syzygyTime_Str[32];
    char syzygyName[32];
    int ecliType;
    char ecliType_Str[32];
    int ecliTime;
    char ecliTime_Str[32];
    char strlunFtvl[64];
};
class sTermClass;
class lunarClass;

class CCCC
{
public:
    CCCC();
    int mi;

    CCCC(int year,int month,int date,int hour=0,int min=0,int sec=0,int zone=8,int calType=1);
    ~CCCC();
public:
    int year;
    int month;
    int date;
    int hour;
    int min;
    int sec;
    int zone;
    int calType;
    char calType_Str[32];
    int JulianDay;
    int dayinweek;
    char dayinweek_Str[32];
    char day_ENStr[32];
    int solarZodiac;
    char solarZodiac_Str[32];
    int iyGz;
    char yGz_Str[32];
    int imGz;
    char mGz_Str[32];
    int idGz;
    char dGz_Str[32];
    int ihGz;
    char hGz_Str[32];
    int istars28;
    char stars28_Str[32];
    char strsFtvl[256];
    char strjqFtvl[256];
    int monthLength;
    char monthLength_Str[32];
    char chineseZodiacName[32];
    char strchineseEra[256];
    sTermClass* solarTerm;
    lunarClass* lunar;
};
#endif
