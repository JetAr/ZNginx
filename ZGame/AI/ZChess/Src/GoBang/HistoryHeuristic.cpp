// HistoryHeuristic.cpp: implementation of the CHistoryHeuristic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Renju.h"
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

//将历史记录表中所有项目全置为初值
void CHistoryHeuristic::ResetHistoryTable()
{
    memset(m_HistoryTable,10,GRID_COUNT*sizeof(int));
}

//从历史得分表中取给定走法的历史得分
int CHistoryHeuristic::GetHistoryScore(STONEMOVE* move)
{
    return m_HistoryTable[move->StonePos.x][move->StonePos.y];
}

//将一最佳走法汇入历史记录
void CHistoryHeuristic::EnterHistoryScore(STONEMOVE* move,int depth)
{
    m_HistoryTable[move->StonePos.x][move->StonePos.y]+=2<<depth;
}

//对走法队列从小到大排序
//STONEMOVE* source原始队列
//STONEMOVE* target目标队列
//合并source[l…m]和 source[m +1…r]至target[l…r]
void CHistoryHeuristic::Merge(STONEMOVE* source,STONEMOVE* target,int l,int m,int r)
{
    //从小到大排序
    int i=l;
    int j=m+1;
    int k=l;

    while(i<=m && j<=r)
        if(source[i].Score<=source[j].Score)
            target[k++]=source[i++];
        else
            target[k++]=source[j++];

    if(i>m)
        for(int q=j; q<=r; q++)
            target[k++]=source[q];
    else
        for(int q=i; q<=m; q++)
            target[k++]=source[q];
}

void CHistoryHeuristic::Merge_A(STONEMOVE* source,STONEMOVE* target,int l,int m,int r)
{
    //从大到小排序
    int i=l;
    int j=m+1;
    int k=l;

    while(i<=m &&j<=r)
        if(source[i].Score>=source[j].Score)
            target[k++]=source[i++];
        else
            target[k++]=source[j++];

    if(i>m)
        for(int q=j; q<=r; q++)
            target[k++]=source[q];
    else
        for(int q=i; q<=m; q++)
            target[k++]=source[q];
}

//合并大小为 S 的相邻子数组
//direction 是标志,指明是从大到小还是从小到大排序
void CHistoryHeuristic::MergePass(STONEMOVE* source,STONEMOVE* target,const int s,const int n,const BOOL direction)
{
    int i=0;

    while(i<=n-2*s)
    {
        //合并大小为 s的相邻二段子数组
        if(direction)
            Merge(source,target,i,i+s-1,i+2*s-1);
        else
            Merge_A(source,target,i,i+s-1,i+2*s-1);

        i=i+2*s;
    }

    if(i+s<n)//剩余的元素个数小于2s
    {
        if(direction)
            Merge(source,target,i,i+s-1,n-1);
        else
            Merge_A(source,target,i,i+s-1,n-1);
    }
    else
        for(int j=i; j<=n-1; j++)
            target[j]=source[j];
}

void CHistoryHeuristic::MergeSort(STONEMOVE* source,int n,BOOL direction)
{
    int s=1;

    while(s<n)
    {
        MergePass(source,m_TargetBuff,s,n,direction);
        s+=s;

        MergePass(m_TargetBuff,source,s,n,direction);
        s+=s;
    }
}
