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

// NegaScout_TT_HH.cpp: implementation of the NegaScout_TT_HH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "NegaScout_TT_HH.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNegaScout_TT_HH::CNegaScout_TT_HH()
{

}

CNegaScout_TT_HH::~CNegaScout_TT_HH()
{

}

void CNegaScout_TT_HH::SearchAGoodMove(BYTE position[10][9])
{
    memcpy(CurPosition, position, 90);
    m_nMaxDepth = m_nSearchDepth;
    CalculateInitHashKey(CurPosition);
    ResetHistoryTable();
//	m_nMaxDepth = 1;
//	NegaScout(m_nMaxDepth, -20000, 20000);
//	m_nMaxDepth = m_nSearchDepth;
//	for (m_nMaxDepth = 1; m_nMaxDepth <= m_nSearchDepth; m_nMaxDepth++)
    NegaScout(m_nMaxDepth, -20000, 20000);
    MakeMove(&m_cmBestMove);
    memcpy(position, CurPosition, 90);
}

int CNegaScout_TT_HH::NegaScout(int depth, int alpha, int beta)
{
    int Count,i;
    BYTE type;
    int a,b,t;
    int side;
    int score;

    i = IsGameOver(CurPosition, depth);
    if (i != 0)
        return i;

    side = (m_nMaxDepth-depth)%2;

    score = LookUpHashTable(alpha, beta, depth, side);
    if (score != 66666)
        return score;

    if (depth <= 0)	//Ҷ�ӽڵ�ȡ��ֵ
    {
        score = m_pEval->Eveluate(CurPosition, side );
        EnterHashTable(exact, score, depth, side );
        return score;
    }

    Count = m_pMG->CreatePossibleMove(CurPosition, depth, side);

    for (i=0; i<Count; i++)
    {
        m_pMG->m_MoveList[depth][i].Score =
            GetHistoryScore(&m_pMG->m_MoveList[depth][i]);
    }
    MergeSort(m_pMG->m_MoveList[depth], Count, 0);

    int bestmove=-1;

    a = alpha;
    b = beta;
    int eval_is_exact = 0;
    for ( i = 0; i < Count; i++ )
    {
        Hash_MakeMove(&m_pMG->m_MoveList[depth][i], CurPosition);
        type = MakeMove(&m_pMG->m_MoveList[depth][i]);

        t = -NegaScout(depth-1 , -b, -a );

        if (t > a && t < beta && i > 0)
        {
            a = -NegaScout (depth-1, -beta, -t );     /* re-search */
            eval_is_exact = 1;
            if(depth == m_nMaxDepth)
                m_cmBestMove = m_pMG->m_MoveList[depth][i];
            bestmove = i;
        }
        Hash_UnMakeMove(&m_pMG->m_MoveList[depth][i],type, CurPosition);
        UnMakeMove(&m_pMG->m_MoveList[depth][i],type);
        if (a < t)
        {
            eval_is_exact = 1;
            a=t;
            if(depth == m_nMaxDepth)
                m_cmBestMove = m_pMG->m_MoveList[depth][i];
        }
        if ( a >= beta )
        {
            EnterHashTable(lower_bound, a, depth,side);
            EnterHistoryScore(&m_pMG->m_MoveList[depth][i], depth);
            return a;
        }
        b = a + 1;                      /* set new null window */
    }
    if (bestmove != -1)
        EnterHistoryScore(&m_pMG->m_MoveList[depth][bestmove], depth);
    if (eval_is_exact)
        EnterHashTable(exact, a, depth,side);
    else
        EnterHashTable(upper_bound, a, depth,side);
    return a;
}