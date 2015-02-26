#include "ccc.h"
#include <stdio.h>

int main()
{
    while(1)
    {
        printf("请输入年月日:");
        int y,m,d = 0;
        scanf("%d:%d:%d",&y,&m,&d);
        if(d == 0)
        {
            break;
        }
        CCCC test(y,m,d);
        printf("星期%s,%s,节日是%s,农历:%s月%s,农历节日是%s\n",test.dayinweek_Str,test.strchineseEra,test.strsFtvl,
               test.lunar->lunarMonth_Str,test.lunar->lunarDate_Str,
               test.lunar->strlunFtvl);
    }
    return 0;
}
