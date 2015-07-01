// HistoryHeuristic.h: interface for the CHistoryHeuristic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTORYHEURISTIC_H__EE973ED4_1D7D_4566_BDA2_7B19DF4B09AF__INCLUDED_)
#define AFX_HISTORYHEURISTIC_H__EE973ED4_1D7D_4566_BDA2_7B19DF4B09AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"

//��ʷ������
class CHistoryHeuristic
{
public:
    CHistoryHeuristic();
    virtual ~CHistoryHeuristic();

public:
    void ResetHistoryTable();						  //ȡĳһ�߷�����ʷ�÷�
    int GetHistoryScore(STONEMOVE* move);			  //��ĳһ����߷�������ʷ��¼��
    void EnterHistoryScore(STONEMOVE* move,int depth);//�Ե�ǰ�߷����н��й鲢����
//	static void MergeSort(STONEMOVE* source,int n,BOOL direction);
    void MergeSort(STONEMOVE* source,int n,BOOL direction);

protected:
//	static void Merge(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);  //���ںϲ�����õ��������ݶ�,MergeSort����
    //��С��������,MergePass����
//	static void MergePass(STONEMOVE* source,STONEMOVE* target,const int s,const int n,const BOOL direction);
//	static void Merge_A(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);//�Ӵ�С���� MergePass����
    void Merge(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);  //���ںϲ�����õ��������ݶ�,MergeSort����
    //��С��������,MergePass����
    void MergePass(STONEMOVE* source,STONEMOVE* target,const int s,const int n,const BOOL direction);
    void Merge_A(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);//�Ӵ�С���� MergePass����

protected:
    int m_HistoryTable[GRID_NUM][GRID_NUM];//��ʷ�÷ֱ�
//	static STONEMOVE m_TargetBuff[225];    //�����õĻ������
    STONEMOVE m_TargetBuff[225];    //�����õĻ������
};

#endif // !defined(AFX_HISTORYHEURISTIC_H__EE973ED4_1D7D_4566_BDA2_7B19DF4B09AF__INCLUDED_)
