#include "stdafx.h"

//引用基本日历函数库
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "ganzhi.h"
#include "lunar.h"
#include "mystring.h"
#include "sterm.h"
#include "solar.h"
#include "ccc.h"
#include "ftvl.h"
#include "era.h"

//节气子类
sTermClass::sTermClass(int year,int month,int date,int time,int zone,int calType,int monthLength)
{
    int sN0=2*month-2;
    int sDt0=S(year,sN0,1,calType);
    int sD0=revD0(year,floor((float)sDt0),calType);
    int sM0=floor((float)(sD0/100));
    int sDate0=sD0%100;

    int sN1=2*month-1;
    int sDt1=S(year,sN1,1,calType);
    int sD1=revD0(year,floor((float)sDt1),calType);
    int sM1=floor((float)(sD1/100));
    int sDate1=sD1%100;

    int sN2=2*month;
    int sDt2=S(year,sN2,1,calType);
    int sD2=revD0(year,floor((float)sDt2),calType);
    int sM2=floor((float)(sD2/100));
    int sDate2=sD2%100;

    int sN3=2*month+1;
    if(sN3>24)
        sN3-=24;
    int sDt3=S(year,sN3,1,calType);
    int sD3=revD0(year,floor((float)(sDt3)),calType);
    int sM3=floor((float)(sD3/100));
    int sDate3=sD3%100;

    if(sM0==month)
    {
        sN2=sN1;
        sN1=sN0;
        sDt2=sDt1;
        sDt1=sDt0;
        sDate2=sDate1;
        sDate1=sDate0;
    }

    if(sM3==month)
    {
        sN1=sN2;
        sN2=sN3;
        sDt1=sDt2;
        sDt2=sDt3;
        sDate1=sDate2;
        sDate2=sDate3;
    }

    sN1=rem(sN1-1,24)+1;
    sN2=rem(sN2-1,24)+1;

    if(sDate2>monthLength)
    {
        sDate2-=monthLength;
    }

    char sT1[64];
    char sT2[64];

    char strtmp[32];

    strcpy(sT1,sStr(sN1,strtmp));
    strcat(sT1,":");
    sprintf(strtmp,"%d",month);
    strcat(sT1,strtmp);
    strcat(sT1,"月");
    sprintf(strtmp,"%d",sDate1);
    strcat(sT1,strtmp);
    strcat(sT1,"日");
    strcat(sT1,dToStr(tail(sDt1),strtmp));

    strcpy(sT2,sStr(sN2,strtmp));
    strcat(sT2,":");
    sprintf(strtmp,"%d",month);
    strcat(sT2,strtmp);
    strcat(sT2,"月");
    sprintf(strtmp,"%d",sDate2);
    strcat(sT2,strtmp);
    strcat(sT2,"日");
    strcat(sT2,dToStr(tail(sDt2),strtmp));



    strcpy(sTermInMonth[0],sT1);
    strcpy(sTermInMonth[1],sT2);

    if(date==sDate1)
    {

        strcpy(sTermName,sStr(sN1,strtmp));
        strcpy(sTermTime,dToStr(tail(sDt1),strtmp));
    }
    else if(date==sDate2)
    {
        strcpy(sTermName,sStr(sN2,strtmp));
        strcpy(sTermTime,dToStr(tail(sDt2),strtmp));

    }
    else
    {
        strcpy(sTermName,"");
        strcpy(sTermTime,"");
    }
}

//农历子类
lunarClass::lunarClass(int year,int month,int date,int calType)
{
    lunarCal lunarInfo(year,month,date,calType);

    this->lunarDate_Str[0] = 0;
    lunarMonth_Str[0] = 0;
    syzygyType[0] = 0;
    syzygyTime_Str[0] = 0;
    syzygyName[0] = 0;
    ecliType_Str[0] = 0;
    strlunFtvl[0] = 0;

    this->lunarDate=lunarInfo.date;
    lunDStr(lunarDate,this->lunarDate_Str);
    this->lunarMonth=lunarInfo.month;
    lunMStr(lunarMonth,lunarMonth_Str);
    strcpy(this->syzygyType,lunarInfo.syzygyType);
    this->syzygyTime=lunarInfo.syzygyTime;
    if(syzygyTime==-1)
    {
        strcpy(syzygyTime_Str,"");
    }
    else
    {
        dToStr(syzygyTime,syzygyTime_Str);
    }
    syzygyStr(syzygyType,syzygyName);
    ecliType=lunarInfo.ecliType;
    ecliStr(lunarInfo.ecliType,ecliType_Str);
    ecliTime=lunarInfo.ecliTime;
    if(ecliTime==-1)
    {
        strcpy(ecliTime_Str,"");
    }
    else
    {
        dToStr(ecliTime,ecliTime_Str);
    }

    if(lunarMonth == 12 && lunarDate == 29)//某些年除夕在12.29
    {
        lunarCal lunarInfo(year,month,date+1,calType);
        if(lunarInfo.date == 1)
        {
            strcpy(this->strlunFtvl,"除夕");
        }
    }
    lunFtvl(lunarMonth,lunarDate,this->strlunFtvl);

}


//中国日历类（Chinese Calendar Class C++ version CCCC）
CCCC::CCCC(int year,int month,int date,int hour,int min,int sec,int zone,int calType)
{
    //////////////////////////  公历属性  //////////////////////////////
    calType_Str[0]=0;
    dayinweek_Str[0]=0;
    solarZodiac_Str[0]=0;
    yGz_Str[0]=0;
    mGz_Str[0]=0;
    dGz_Str[0]=0;

    hGz_Str[0]=0;
    stars28_Str[0]=0;
    strsFtvl[0]=0;
    strjqFtvl[0]=0;

    monthLength_Str[0]=0;
    chineseZodiacName[0]=0;
    strchineseEra[0]=0;

    this->year=year;
    this->month=month;
    this->date=date;
    this->hour=hour;
    this->min=min;
    this->sec=sec;
    this->zone=zone;
    this->calType=calType;
    calTypeStr(this->calType,this->calType_Str);
    this->JulianDay=JD(this->year,this->month,this->date,this->hour,this->min,this->sec,this->zone,this->calType);
    this->dayinweek=Day(this->year,this->month,this->date,this->calType);
    dayStr(this->dayinweek,this->dayinweek_Str);
    dayENStr(this->dayinweek,this->day_ENStr);
    this->solarZodiac=sZod(this->month,this->date);

    szodStr(this->solarZodiac,solarZodiac_Str);

    this->iyGz=yGz(this->year,this->month,this->date,this->hour,this->calType);

    gzStr(this->iyGz,this->yGz_Str);

    this->imGz=mGz(this->year,this->month,this->date,this->hour,this->calType);

    gzStr(this->imGz,this->mGz_Str);
    this->idGz=dGz(this->year,this->month,this->date,this->hour,this->calType);

    gzStr(this->idGz,this->dGz_Str);

    this->ihGz=hGz(this->year,this->month,this->date,this->hour,this->calType);

    gzStr(this->ihGz,this->hGz_Str);

    this->istars28=stars28(this->year,this->month,this->date,this->calType);

    stars28Str(this->istars28,this->stars28_Str);

    //z 节日
    sFtvl(this->year,this->month,this->date,this->calType,this->strsFtvl);

    //z 节气
    jqFtvl(this->year,this->month,this->date,this->calType,this->strjqFtvl);

    this->monthLength=D0(this->year,this->month+1,1,this->calType)-D0(this->year,this->month,1,this->calType);

    mLStr(this->monthLength,this->monthLength_Str);

    zodStr(this->iyGz,this->chineseZodiacName);

    //年号
    chineseEra(this->year,this->strchineseEra);

    ///////////////////////所在阳历月的两个节气日期和交节时刻/////////////////////////
    this->solarTerm = new sTermClass(year,month,date,0,zone,calType,this->monthLength);

    //////////////////////////  农历属性  //////////////////////////////
    this->lunar=new lunarClass(year,month,date,calType);
}

CCCC::~CCCC()
{
    delete solarTerm;
    delete lunar;
}




