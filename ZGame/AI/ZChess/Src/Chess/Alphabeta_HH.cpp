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

// Alphabeta_HH.cpp: implementation of the CAlphabeta_HH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "Alphabeta_HH.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlphabeta_HH::CAlphabeta_HH()
{

}

CAlphabeta_HH::~CAlphabeta_HH()
{

}

void CAlphabeta_HH::SearchAGoodMove(BYTE position[10][9])
{
	memcpy(CurPosition, position, 90);
	m_nMaxDepth = m_nSearchDepth;
	ResetHistoryTable();
	alphabeta(m_nMaxDepth, -20000, 20000);
	MakeMove(&m_cmBestMove);
	memcpy(position, CurPosition, 90);
}

int CAlphabeta_HH::alphabeta(int depth, int alpha, int beta)
{
	int score;
	int Count,i;
	BYTE type;

	i = IsGameOver(CurPosition, depth);
	if (i != 0)
		return i;

	if (depth <= 0)	//Ҷ�ӽڵ�ȡ��ֵ
		return m_pEval->Eveluate(CurPosition, (m_nMaxDepth-depth)%2);
	
	Count = m_pMG->CreatePossibleMove(CurPosition, depth, (m_nMaxDepth-depth)%2);
	
	for (i=0;i<Count;i++) 
	{
		m_pMG->m_MoveList[depth][i].Score = 
			           GetHistoryScore(&m_pMG->m_MoveList[depth][i]);
	}
	MergeSort(m_pMG->m_MoveList[depth], Count, 0);
	int bestmove = -1;
	for (i=0;i<Count;i++) 
	{

		type = MakeMove(&m_pMG->m_MoveList[depth][i]);
		score = -alphabeta(depth - 1, -beta, -alpha);
		UnMakeMove(&m_pMG->m_MoveList[depth][i],type); 

		if (score > alpha)
		{
			alpha = score;
			if(depth == m_nMaxDepth)
				m_cmBestMove = m_pMG->m_MoveList[depth][i];
			bestmove = i;
		}
        if (alpha >= beta) 
		{
			bestmove = i;
			break;
		}
				
	}
	if (bestmove != -1)
	EnterHistoryScore(&m_pMG->m_MoveList[depth][bestmove], depth);
	return alpha;
}

