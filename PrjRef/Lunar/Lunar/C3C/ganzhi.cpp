#include "stdafx.h"

#include "solar.h"
#include "sterm.h"
#include <math.h>

//天干
int gan(int x)
{
    return x%10;
}

//地支
int zhi(int x)
{
    return x%12;
}

//年干支
int yGz(int y,int m,int d,int h,int calType)
{
    if((D0(y,m,d,calType)+h/24)<S(y,3,1,calType)-1)  //判断是否过立春
        y-=1;
    return round(rem(y-3-1,60)+1);
}

//月干支
int  mGz(int y,int m,int d,int h,int calType)
{

    int  sN0=2*m-2;
    int  sDt0=S(y,sN0,1,calType);
    int  sD0=revD0(y,floor((float)sDt0),calType);
    int  sM0=floor((float)(sD0/100));
    int  sDate0=sD0%100+tail(sDt0);

    int  sN1=2*m-1;
    int  sDt1=S(y,sN1,1,calType);
    int  sD1=revD0(y,floor((float)sDt1),calType);
    int  sM1=floor((float)(sD1/100));
    int   sDate1=sD1%100+tail(sDt1);

    int  sN2=2*m;
    int  sDt2=S(y,sN2,1,calType);
    int  sD2=revD0(y,floor((float)sDt2),calType);
    int  sM2=floor((float)(sD2/100));
    int   sDate2=sD2%100+tail(sDt2);

    int  sN3=2*m+1;
    if(sN3>24)
        sN3-=24;
    int  sDt3=S(y,sN3,1,calType);
    int  sD3=revD0(y,floor((float)sDt3));
    int  sM3=floor((float)(sD3/100));
    int   sDate3=sD3%100+tail(sDt3);

    if(sM0==m)
    {
        sN2=sN1;
        sN1=sN0;
        sDt2=sDt1;
        sDt1=sDt0;
        sDate2=sDate1;
        sDate1=sDate0;
    }

    if(sM3==m)
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

    int  mL=D0(y,m,31,calType)-D0(y,m,0,calType);
    if(sDate2>mL)
        sDate2-=mL;

    int  jqDate=(sN1%2==1)?sDate1:sDate2;  //"节气"(n为奇数)所在的阳历日数

    int  gzM=((d+h/24)<jqDate)?(m-2):(m-1);   //干支月
    if(gzM<=0)
        gzM+=12;
    return round(rem(12*gan(yGz(y,m,d,h,calType))+gzM-10-1,60)+1);
}

//日干支
int  dGz(int y,int m,int d,int h,int calType)
{
    int  gzD=(h<23)?ESD(y,m,d,calType):ESD(y,m,d,calType)+1;
    return round(rem(gzD+15-1,60)+1);
}

//时干支
int  hGz(int y,int m,int d,int h,int calType)
{
    int  v=12*gan(dGz(y,m,d,h,calType))+floor((float)((h+1)/2))-11;
    if(h==23)
        v-=12;
    return round(rem(v-1,60)+1);
}

//甲子纳音
char* GZNY(int gz)
{
    char s[30][7]=
    {
        "海中金",
        "炉中火",
        "大林木",
        "路旁土",
        "剑锋金",
        "山头火",
        "洞下水",
        "城墙土",
        "白腊金",
        "杨柳木",
        "泉中水",
        "屋上土",
        "霹雷火",
        "松柏木",
        "常流水",
        "沙中金",
        "山下火",
        "平地木",
        "壁上土",
        "金箔金",
        "佛灯火",
        "天河水",
        "大驿土",
        "钗钏金",
        "桑松木",
        "大溪水",
        "沙中土",
        "天上火",
        "石榴木",
        "大海水"
    };


    return s[(int)floor((float)((gz-1)/2))];
}

//二十八宿
int stars28(int y,int m,int d,int calType)
{
    int  v=ESD(y,m,d,calType);
    return round(rem(v-4-1,28)+1);
}
