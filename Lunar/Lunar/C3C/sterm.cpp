#include "stdafx.h"

#include <math.h>
#include "solar.h"
#include "sterm.h"
//节气函数
double S(int y,int n,int pd,int calType)   //pd取值为0或1，分别表示平气和定气,该函数返回节气的D0值
{
    double juD=y*(365.2423112-6.4e-14*(y-100)*(y-100)-3.047e-8*(y-100))+15.218427*n+1721050.71301;//儒略日
    double  tht=3e-4*y-0.372781384-0.2617913325*n;//角度
    double  yrD=(1.945*sin(tht)-0.01206*sin(2*tht))*(1.048994-2.583e-5*y);//年差实均数
    double  shuoD=-18e-4*sin(2.313908653*y-0.439822951-3.0443*n);//朔差实均数
    double  vs=(pd)?(juD+yrD+shuoD-ESD(y,1,0,calType)-1721425):(juD-ESD(y,1,0,calType)-1721425);
    return vs;
}
