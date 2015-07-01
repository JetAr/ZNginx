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

// AlphaBetaAndTT.h: interface for the CAlphaBetaAndTT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALPHABETAANDTT_H__8E8EE2C0_CEBC_11D5_AEC7_5254AB2E22C7__INCLUDED_)
#define AFX_ALPHABETAANDTT_H__8E8EE2C0_CEBC_11D5_AEC7_5254AB2E22C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SearchEngine.h"
#include "TranspositionTable.h"

class CAlphaBetaAndTT : public CSearchEngine, public CTranspositionTable
{
public:
    CAlphaBetaAndTT();
    virtual ~CAlphaBetaAndTT();

    virtual void SearchAGoodMove(BYTE position[10][9]);
protected:
    int alphabeta(int depth, int alpha, int beta);
};

#endif // !defined(AFX_ALPHABETAANDTT_H__8E8EE2C0_CEBC_11D5_AEC7_5254AB2E22C7__INCLUDED_)
