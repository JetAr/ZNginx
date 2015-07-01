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

//����ʷ��¼����������Ŀȫ��Ϊ��ֵ
void CHistoryHeuristic::ResetHistoryTable()
{
    memset(m_HistoryTable,10,GRID_COUNT*sizeof(int));
}

//����ʷ�÷ֱ���ȡ�����߷�����ʷ�÷�
int CHistoryHeuristic::GetHistoryScore(STONEMOVE* move)
{
    return m_HistoryTable[move->StonePos.x][move->StonePos.y];
}

//��һ����߷�������ʷ��¼
void CHistoryHeuristic::EnterHistoryScore(STONEMOVE* move,int depth)
{
    m_HistoryTable[move->StonePos.x][move->StonePos.y]+=2<<depth;
}

//���߷����д�С��������
//STONEMOVE* sourceԭʼ����
//STONEMOVE* targetĿ�����
//�ϲ�source[l��m]�� source[m +1��r]��target[l��r]
void CHistoryHeuristic::Merge(STONEMOVE* source,STONEMOVE* target,int l,int m,int r)
{
    //��С��������
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
    //�Ӵ�С����
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

//�ϲ���СΪ S ������������
//direction �Ǳ�־,ָ���ǴӴ�С���Ǵ�С��������
void CHistoryHeuristic::MergePass(STONEMOVE* source,STONEMOVE* target,const int s,const int n,const BOOL direction)
{
    int i=0;

    while(i<=n-2*s)
    {
        //�ϲ���СΪ s�����ڶ���������
        if(direction)
            Merge(source,target,i,i+s-1,i+2*s-1);
        else
            Merge_A(source,target,i,i+s-1,i+2*s-1);

        i=i+2*s;
    }

    if(i+s<n)//ʣ���Ԫ�ظ���С��2s
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
