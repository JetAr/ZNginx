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

// MTD_f.cpp: implementation of the CMTD_f class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "MTD_f.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMTD_f::CMTD_f()
{

}

CMTD_f::~CMTD_f()
{

}
void CMTD_f::SearchAGoodMove(BYTE position[10][9])
{
	int firstguess = 0;
	memcpy(CurPosition, position, 90);
	m_nMaxDepth = m_nSearchDepth;
	CalculateInitHashKey(CurPosition);
	for (m_nMaxDepth  = 1; m_nMaxDepth <= m_nSearchDepth; m_nMaxDepth++)
	{ 
		firstguess = mtdf(firstguess, m_nMaxDepth);
	}
	MakeMove(&m_cmBackupBM);
	memcpy(position, CurPosition, 90);
	//z return 0;
}

int CMTD_f::mtdf(int firstguess,int depth)
{
	int g,lowerbound, upperbound,beta;
	
	g=firstguess;
	upperbound=20000;
	lowerbound=-20000;
	while(lowerbound < upperbound)
	{
		m_cmBackupBM = m_cmBestMove;
		if(g==lowerbound)
			beta=g+1;
		else 
			beta=g;
		g=FAlphaBeta(depth,beta-1,beta);
		if(g<beta)
			upperbound=g;
		else
			lowerbound=g;
	}
	return g;
}

int CMTD_f::FAlphaBeta(int depth, int alpha, int beta)
{
	int current = -19999 ;
	int score;
	int Count,i;
	BYTE type;

	i = IsGameOver(CurPosition, depth);
	if (i != 0)
		return i;


	score = LookUpHashTable(alpha, beta, depth,(m_nMaxDepth-depth)%2); 
	if (score!= 66666)
		return score;

	if (depth <= 0)	//叶子节点取估值
	{
		current = m_pEval->Eveluate(CurPosition, (m_nMaxDepth-depth)%2);
		EnterHashTable(exact, current, depth,(m_nMaxDepth-depth)%2);
		return current;
	}


	Count = m_pMG->CreatePossibleMove(CurPosition, depth, (m_nMaxDepth-depth)%2);
/*
	for (i=0;i<Count;i++) 
	{
		m_pMG->m_MoveList[depth][i].Score = 
			           GetHistoryScore(&m_pMG->m_MoveList[depth][i]);
	}
	MergeSort(m_pMG->m_MoveList[depth], Count, 0);

	int bestmove = -1;
*/
	int eval_is_exact = 0;

	for (i=0;i<Count;i++) 
	{

		Hash_MakeMove(&m_pMG->m_MoveList[depth][i], CurPosition);
		type = MakeMove(&m_pMG->m_MoveList[depth][i]);

		score = -FAlphaBeta(depth - 1, -beta, -alpha);
		
		Hash_UnMakeMove(&m_pMG->m_MoveList[depth][i],type, CurPosition); 
		UnMakeMove(&m_pMG->m_MoveList[depth][i],type); 

				
		if (score > current)
		{
			current = score;
//	  	    bestmove = i;
			if(depth == m_nMaxDepth)
				m_cmBestMove = m_pMG->m_MoveList[depth][i];
			if (score >= beta) //beta剪枝
			{
				EnterHashTable(lower_bound, score, depth,(m_nMaxDepth-depth)%2);
//				EnterHistoryScore(&m_pMG->m_MoveList[depth][i], depth);
				return current;
			}
			if (score > alpha)
			{
				eval_is_exact = 1;
				alpha = score;
			}
		}

	}
/*	if (bestmove != -1)
	EnterHistoryScore(&m_pMG->m_MoveList[depth][bestmove], depth);
*/  if (eval_is_exact) 
		EnterHashTable(exact, alpha, depth,(m_nMaxDepth-depth)%2);
    else 
		EnterHashTable(upper_bound, current, depth,(m_nMaxDepth-depth)%2);
	return current;
}

