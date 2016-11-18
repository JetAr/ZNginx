#include "stdafx.h"

//--------转换成字符串的函数--------//
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "mystring.h"

//将单位天的纯小数转换成如同5:08的时间格式
char* dToStr(double dv,char* time)
{
    if(!time)
    {
        return 0;
    }
    int h=floor(dv*24);
    int min=floor((dv*24-h)*60);
    sprintf(time,"%02d:02d",h,min);
    return time;
}

//公元年份
char* yearStr(int v,char* year)
{
    if(!year)
    {
        return 0;
    }
    if(v<=0)
    {
        sprintf(year,"前%d",(-v+1));
    }
    else
    {
        sprintf(year,"%d",v);
    }
    return year;
}

//星期
char* dayStr(int v,char* day)
{
    if(!day)
    {
        return 0;
    }
    switch(v%7)
    {
    case 0:
        strcpy(day,"日");
        break;
    case 1:
        strcpy(day,"一");
        break;
    case 2:
        strcpy(day,"二");
        break;
    case 3:
        strcpy(day,"三");
        break;
    case 4:
        strcpy(day,"四");
        break;
    case 5:
        strcpy(day,"五");
        break;
    case 6:
        strcpy(day,"六");
        break;
    }

    return day;
}

char* dayENStr(int v,char* day)
{
    if(!day)
    {
        return 0;
    }
    static char s[7][10]= {"SUNDAY","MONDAY","TUESDAY","WEDNESDAY","THURSDAY","FRIDAY","SATURDAY"};
    strcpy(day,s[v%7]);
    return day;
}

//星座
char* szodStr(int v,char* star)
{
    if(!star)
    {
        return 0;
    }
    static char s[12][8]= {"摩羯座","宝瓶座","双鱼座","白羊座","金牛座","双子座","巨蟹座","狮子座","处女座","天秤座","天蝎座","射手座"};
    strcpy(star,s[v%12]);
    return star;
}

//干支
char* gzStr(int v,char* ganzhi)
{
    if(!ganzhi)
    {
        return 0;
    }
    static char tiangan[10][4] = {"癸","甲","乙","丙","丁","戊","己","庚","辛","壬"};
    static char dizhi[12][4] = {"亥","子","丑","寅","卯","辰","巳","午","未","申","酉","戌"};
    strcpy(ganzhi,tiangan[v%10]);
    strcat(ganzhi,dizhi[v%12]);
    return ganzhi;
}

//生肖
char* zodStr(int v,char* sx)
{
    if(!sx)
    {
        return 0;
    }
    v%=12;
    static char shengxiao[12][4] = {"猪","鼠","牛","虎","兔","龙","蛇","马","羊","猴","鸡","狗"};
    strcpy(sx,shengxiao[v]);
    return sx;
}

//农历月数
char* lunMStr(int v,char* yueshu)
{
    if(!yueshu)
    {
        return 0;
    }
    int v0=abs(v);
    static char str[10][4]= {"一","二","三","四","五","六","七","八","九","十"};
    char vstr[4];
    strcpy(vstr,str[(v0-1)%10]);
    if(v0>10)
        strcpy(yueshu,"十");
    if(v0==1)
        strcpy(yueshu,"正");
    if(v<0)
        strcpy(yueshu,"闰");
    strcat(yueshu,vstr);
    return yueshu;
}

//农历日数
char* lunDStr(int v,char* nongli)
{
    if(!nongli)
    {
        return 0;
    }
    static char str[15][4]= {"十","一","二","三","四","五","六","七","八","九","初","十","廿","三"};
    strcpy(nongli,str[(int)(floor((float)(v/10))+10)]);
    strcat(nongli,str[v%10]);
    if(v==10)
        strcpy(nongli,"初十");
    return nongli;
}

//节气
char* sStr(int v,char* jieqi)
{
    if(!jieqi)
    {
        return 0;
    }
    static char str[24][6] = {"小寒","大寒","立春","雨水","惊蛰","春分","清明","谷雨","立夏","小满","芒种","夏至","小暑","大暑",
                              "立秋","处暑","白露","秋分","寒露","霜降","立冬","小雪","大雪","冬至"
                             };
    strcpy(jieqi,str[v]);
    return jieqi;
}

//公历类型
char* calTypeStr(int v,char* instr)
{
    if(!instr)
    {
        return 0;
    }
    if(v==1)
    {
        strcpy(instr,"公历");
        return instr;
    }
    else if(v==2)
    {
        strcpy(instr,"格里历");
        return instr;
    }
    else
    {
        strcpy(instr,"儒略历");
        return instr;
    }
    return 0;
}

//是否为格里历
char* ifgStr(int v,char* instr)
{
    if(!instr)
    {
        return 0;
    }
    if(v==-1)
    {
        strcpy(instr,"不存在");
        return instr;
    }
    else
    {
        if(v)
        {
            strcpy(instr,"格里历");
            return instr;
        }
        else
        {
            strcpy(instr,"儒略历");
            return instr;
        }
    }
    return 0;
}

//朔望
char* syzygyStr(char* syzygyType,char* suowang)
{
    if(0 == syzygyType || 0 == suowang)
    {
        return 0;
    }
    if(strcmp(syzygyType,"newMoon") == 0)
    {
        strcpy(suowang,"朔");
        return suowang;
    }
    if(strcmp(syzygyType,"fullMoon") == 0)
    {
        strcpy(suowang,"望");
        return suowang;
    }
    return 0;
}

//日月食
char* ecliStr(int v,char* instr)
{
    if(!instr)
    {
        return 0;
    }
    if(v==1)
        strcpy(instr,"日食");
    if(v==2)
        strcpy(instr,"月全食");
    if(v==3)
        strcpy(instr,"月偏食");
    return instr;
}

//二十八宿
char* stars28Str(int n,char* instr)
{
    if(!instr)
    {
        return 0;
    }
    static char s[28][4]= {"角","亢","氐","房","心","尾","箕","斗","牛","女","虚","危","室","壁","奎","娄","胃","昴","毕","觜","参","井","鬼","柳","星","张","翼","轸"};
    strcpy(instr,s[n]);
    return instr;
}

//月大月小
char* mLStr(int mL,char* instr)
{
    if(!instr)
    {
        return 0;
    }
    if(mL==31)
    {
        strcpy(instr,"大");
    }
    else if(mL==30)
    {
        strcpy(instr,"小");
    }
    else
    {
        strcpy(instr,"平");
    }
    return instr;
}
