// COPYRIGHT NOTES
// ---------------
// This source code is a part of chess which is an example of <Game Programing guide>.
// You may use, compile or redistribute it as part of your application
// for free.
// You cannot redistribute sources without the official agreement of the author.
// If distribution of you application which contents code below was occured, place
// e-mail <hidebug@hotmail.com> on it is to be appreciated.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
//
// Spring Wang <hidebug@hotmail.com>

// ---------------
// 版权声明
// ---------------
// 本文件所含之代码是《人机博弈程序设计指南》的范例程序中国象棋的一部分
// 您可以免费的使用, 编译 或者作为您应用程序的一部分。
// 但，您不能在未经作者书面许可的情况下分发此源代码。
// 如果您的应用程序使用了这些代码，在您的应用程序界面上
// 放入 e-mail <hidebug@hotmail.com> 是令人欣赏的做法。
// 此代码并不含有任何保证，使用者当自承风险。
//
// 王小春 <hidebug@hotmail.com>

// HistoryHeuristic.cpp: implementation of the CHistoryHeuristic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "HistoryHeuristic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHistoryHeuristic::CHistoryHeuristic()
{
}

CHistoryHeuristic::~CHistoryHeuristic()
{
}

void CHistoryHeuristic::ResetHistoryTable()
{
    memset(m_HistoryTable, 10, 8100*4);
}

int CHistoryHeuristic::GetHistoryScore(CHESSMOVE *move)
{
    int nFrom, nTo;

    nFrom = move->From.y*9+move->From.x;
    nTo = move->To.y*9+move->To.x;

    return m_HistoryTable[nFrom][nTo];
}
void CHistoryHeuristic::EnterHistoryScore(CHESSMOVE *move,int depth)
{
    int nFrom, nTo;
    nFrom = move->From.y*9+move->From.x;
    nTo = move->To.y*9+move->To.x;

    m_HistoryTable[nFrom][nTo] += 2<<depth;
}
//
void CHistoryHeuristic::Merge(CHESSMOVE *source, CHESSMOVE *target, int l,int m, int r)
{
    //从小到大排序
    int i = l;
    int j = m + 1;
    int k = l;

    while((i <= m) && (j <= r))
        if (source[i].Score <= source[j].Score)
            target[k++] = source[i++];
        else
            target[k++] = source[j++];

    if(i > m)
        for (int q = j; q <= r; q++)
            target[k++] = source[q];
    else
        for(int q = i; q <= m; q++)
            target[k++] = source[q];
}

void CHistoryHeuristic::Merge_A(CHESSMOVE *source, CHESSMOVE *target, int l,int m, int r)
{
    //从大到小排序
    int i = l;
    int j = m + 1;
    int k = l;

    while((i <= m) && (j <= r))
        if (source[i].Score >= source[j].Score)
            target[k++] = source[i++];
        else
            target[k++] = source[j++];

    if(i > m)
        for (int q = j; q <= r; q++)
            target[k++] = source[q];
    else
        for(int q = i; q <= m; q++)
            target[k++] = source[q];
}


void CHistoryHeuristic::MergePass(CHESSMOVE *source, CHESSMOVE *target, const  int s, const  int n, const BOOL direction)
{
    int i = 0;

    while(i <= n - 2 * s)
    {
        //合并大小为s的相邻二段子数组
        if (direction)
            Merge(source, target, i, i + s - 1, i + 2 * s - 1);
        else
            Merge_A(source, target, i, i + s - 1, i + 2 * s - 1);
        i=i+2*s;
    }

    if (i + s < n) //剩余的元素个数小於2s
    {
        if (direction)
            Merge(source, target, i, i + s - 1, n - 1);
        else
            Merge_A(source, target, i, i + s - 1, n - 1);
    }
    else
        for (int j = i; j <= n - 1; j++)
            target[j] = source[j];
}


void CHistoryHeuristic::MergeSort(CHESSMOVE *source, int n, BOOL direction)
{
    int s = 1;
    while(s < n)
    {
        MergePass(source, m_TargetBuff, s, n, direction);
        s += s;
        MergePass(m_TargetBuff, source, s, n, direction);
        s += s;
    }
}

