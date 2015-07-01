// HistoryHeuristic.h: interface for the CHistoryHeuristic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTORYHEURISTIC_H__EE973ED4_1D7D_4566_BDA2_7B19DF4B09AF__INCLUDED_)
#define AFX_HISTORYHEURISTIC_H__EE973ED4_1D7D_4566_BDA2_7B19DF4B09AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"

//历史启发类
class CHistoryHeuristic
{
public:
    CHistoryHeuristic();
    virtual ~CHistoryHeuristic();

public:
    void ResetHistoryTable();						  //取某一走法的历史得分
    int GetHistoryScore(STONEMOVE* move);			  //将某一最佳走法汇入历史记录表
    void EnterHistoryScore(STONEMOVE* move,int depth);//对当前走法队列进行归并排序
//	static void MergeSort(STONEMOVE* source,int n,BOOL direction);
    void MergeSort(STONEMOVE* source,int n,BOOL direction);

protected:
//	static void Merge(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);  //用于合并排序好的相邻数据段,MergeSort调用
    //从小到大排序,MergePass调用
//	static void MergePass(STONEMOVE* source,STONEMOVE* target,const int s,const int n,const BOOL direction);
//	static void Merge_A(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);//从大到小排序 MergePass调用
    void Merge(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);  //用于合并排序好的相邻数据段,MergeSort调用
    //从小到大排序,MergePass调用
    void MergePass(STONEMOVE* source,STONEMOVE* target,const int s,const int n,const BOOL direction);
    void Merge_A(STONEMOVE* source,STONEMOVE* target,int l,int m,int r);//从大到小排序 MergePass调用

protected:
    int m_HistoryTable[GRID_NUM][GRID_NUM];//历史得分表
//	static STONEMOVE m_TargetBuff[225];    //排序用的缓冲队列
    STONEMOVE m_TargetBuff[225];    //排序用的缓冲队列
};

#endif // !defined(AFX_HISTORYHEURISTIC_H__EE973ED4_1D7D_4566_BDA2_7B19DF4B09AF__INCLUDED_)
