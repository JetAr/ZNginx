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
// ��Ȩ����
// ---------------
// ���ļ�����֮�����ǡ��˻����ĳ������ָ�ϡ��ķ��������й������һ����
// ��������ѵ�ʹ��, ���� ������Ϊ��Ӧ�ó����һ���֡�
// ������������δ������������ɵ�����·ַ���Դ���롣
// �������Ӧ�ó���ʹ������Щ���룬������Ӧ�ó��������
// ���� e-mail <hidebug@hotmail.com> ���������͵�������
// �˴��벢�������κα�֤��ʹ���ߵ��Գз��ա�
//
// ��С�� <hidebug@hotmail.com>

// HistoryHeuristic.h: interface for the CHistoryHeuristic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTORYHEURISTIC_H__5870AB20_E3F1_11D5_AEC7_5254AB2E22C7__INCLUDED_)
#define AFX_HISTORYHEURISTIC_H__5870AB20_E3F1_11D5_AEC7_5254AB2E22C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHistoryHeuristic
{
public:
    CHistoryHeuristic();
    virtual ~CHistoryHeuristic();

    void ResetHistoryTable();
    int GetHistoryScore(CHESSMOVE *move);
    void EnterHistoryScore(CHESSMOVE *move,int depth);
    void MergeSort(CHESSMOVE *source, int n, BOOL direction);
protected:
    void Merge(CHESSMOVE *source, CHESSMOVE *target, int l,int m, int r);
    void MergePass(CHESSMOVE *source, CHESSMOVE *target, const  int s, const  int n, const BOOL direction);
    void Merge_A(CHESSMOVE *source, CHESSMOVE *target, int l,int m, int r);
    int m_HistoryTable[90][90];
    CHESSMOVE m_TargetBuff[100];

};

#endif // !defined(AFX_HISTORYHEURISTIC_H__5870AB20_E3F1_11D5_AEC7_5254AB2E22C7__INCLUDED_)
