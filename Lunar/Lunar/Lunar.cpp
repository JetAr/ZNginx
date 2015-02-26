�?/ Lunar.cpp : 定义控制台应用程序的入口点�?
//

#include "stdafx.h"
#include "Lunar.h"
#include "C3C\ccc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对�?

CWinApp theApp;

using namespace std;

int CalcLunarDay()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    printf("请输入年月日 : \nSample : %04d:%02d:%02d\n\n",st.wYear,st.wMonth,st.wDay);
    while(1)
    {
        int y,m,d = 0;
        scanf("%d:%d:%d",&y,&m,&d);
        if(d == 0)
        {
            break;
        }

        CCCC test(y,m,d);
        /*printf("星期%s,%s,节日�?s,农历:%s�?s,农历节日�?s\n",test.dayinweek_Str,test.strchineseEra,test.strsFtvl,
        	test.lunar->lunarMonth_Str,test.lunar->lunarDate_Str,
        	test.lunar->strlunFtvl);*/
        printf("[%04d.%02d.%02d] : 农历 %s�?s, 节日 %s, 农历节日 %s\n",
               y,m,d,
               test.lunar->lunarMonth_Str,test.lunar->lunarDate_Str,
               test.strsFtvl,
               test.lunar->strlunFtvl);
    }
    return 0;
}

int CalcLunarForToday()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    CCCC LunarDay(st.wYear,st.wMonth,st.wDay);
    printf("今日[%02d.%02d] : %s�?s\n",st.wMonth,st.wDay,LunarDay.lunar->lunarMonth_Str,LunarDay.lunar->lunarDate_Str);

    return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;

    // 初始�?MFC 并在失败时显示错�?
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO: 更改错误代码以符合您的需�?
        _tprintf(_T("错误: MFC 初始化失败\n"));
        nRetCode = 1;
    }
    else
    {
        // TODO: 在此处为应用程序的行为编写代码�?
        CalcLunarForToday();
        CalcLunarDay();
    }

    return nRetCode;
}
