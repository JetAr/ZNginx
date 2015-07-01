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

// AlphaBetaAndTT.cpp: implementation of the CAlphaBetaAndTT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "AlphaBetaAndTT.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAlphaBetaAndTT::CAlphaBetaAndTT()
{
}

CAlphaBetaAndTT::~CAlphaBetaAndTT()
{
}


void CAlphaBetaAndTT::SearchAGoodMove(BYTE position[10][9])
{
    memcpy(CurPosition, position, 90);
    CalculateInitHashKey(CurPosition);
    m_nMaxDepth = m_nSearchDepth;
    alphabeta(m_nMaxDepth, -20000, 20000);
    MakeMove(&m_cmBestMove);
    memcpy(position, CurPosition, 90);
}


int CAlphaBetaAndTT::alphabeta(int depth, int alpha, int beta)
{
    int score;
    int Count,i;
    BYTE type;
    int side;

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

    int eval_is_exact = 0;

    for (i=0; i<Count; i++)
    {
        Hash_MakeMove(&m_pMG->m_MoveList[depth][i], CurPosition);
        type = MakeMove(&m_pMG->m_MoveList[depth][i]);

        score = -alphabeta(depth - 1, -beta, -alpha);

        Hash_UnMakeMove(&m_pMG->m_MoveList[depth][i],type, CurPosition);
        UnMakeMove(&m_pMG->m_MoveList[depth][i],type);
        if (score >= beta)
        {
            EnterHashTable(lower_bound, score, depth,side);
            return score;
        }

        if (score > alpha)
        {
            alpha = score;
            eval_is_exact = 1;
            if(depth == m_nMaxDepth)
                m_cmBestMove = m_pMG->m_MoveList[depth][i];
        }

    }

    if (eval_is_exact)
        EnterHashTable(exact, alpha, depth,side);
    else
        EnterHashTable(upper_bound, alpha, depth,side);
    return alpha;
}



