ï»?/ Lunar.cpp : å®ä¹æ§å¶å°åºç¨ç¨åºçå¥å£ç¹ã?
//

#include "stdafx.h"
#include "Lunar.h"
#include "C3C\ccc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// å¯ä¸çåºç¨ç¨åºå¯¹è±?

CWinApp theApp;

using namespace std;

int CalcLunarDay()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    printf("è¯·è¾å¥å¹´ææ¥ : \nSample : %04d:%02d:%02d\n\n",st.wYear,st.wMonth,st.wDay);
    while(1)
    {
        int y,m,d = 0;
        scanf("%d:%d:%d",&y,&m,&d);
        if(d == 0)
        {
            break;
        }

        CCCC test(y,m,d);
        /*printf("ææ%s,%s,èæ¥æ?s,åå:%sæ?s,ååèæ¥æ?s\n",test.dayinweek_Str,test.strchineseEra,test.strsFtvl,
        	test.lunar->lunarMonth_Str,test.lunar->lunarDate_Str,
        	test.lunar->strlunFtvl);*/
        printf("[%04d.%02d.%02d] : åå %sæ?s, èæ¥ %s, ååèæ¥ %s\n",
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
    printf("ä»æ¥[%02d.%02d] : %sæ?s\n",st.wMonth,st.wDay,LunarDay.lunar->lunarMonth_Str,LunarDay.lunar->lunarDate_Str);

    return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;

    // åå§å?MFC å¹¶å¨å¤±è´¥æ¶æ¾ç¤ºéè¯?
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO: æ´æ¹éè¯¯ä»£ç ä»¥ç¬¦åæ¨çéè¦?
        _tprintf(_T("éè¯¯: MFC åå§åå¤±è´¥\n"));
        nRetCode = 1;
    }
    else
    {
        // TODO: å¨æ­¤å¤ä¸ºåºç¨ç¨åºçè¡ä¸ºç¼åä»£ç ã?
        CalcLunarForToday();
        CalcLunarDay();
    }

    return nRetCode;
}
