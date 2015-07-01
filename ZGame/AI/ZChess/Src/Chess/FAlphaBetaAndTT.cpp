// FAlphaBetaAndTT.cpp: implementation of the CFAlphaBetaAndTT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "FAlphaBetaAndTT.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFAlphaBetaAndTT::CFAlphaBetaAndTT()
{
}

CFAlphaBetaAndTT::~CFAlphaBetaAndTT()
{
}
extern int prevalue;
CFAlphaBetaAndTT::SearchAGoodMove(BYTE position[10][9])
{
//	int res;

	memcpy(CurPosition, position, 90);
	
	m_nMaxDepth = m_nSearchDepth;
	CalculateInitHashKey(CurPosition);
	int mju = 3;
	FAlphaBeta(m_nMaxDepth, -20000, 20000);
/*	Preview = prevalue;
	if (prevalue  == 19999)
	{
		res = FAlphaBeta(m_nMaxDeepth, -prevalue, prevalue);
	MakeMove(&m_cmBestMove);
	memcpy(position, CurPosition, 90);
	return 0;
	}
	else
		res = FAlphaBeta(m_nMaxDeepth, prevalue - mju, prevalue + mju);
	prevalue = res;
	if (res < Preview - mju)
		prevalue = FAlphaBeta(m_nMaxDeepth, -20000, res);
	if (res > Preview + mju)
		prevalue = FAlphaBeta(m_nMaxDeepth, res, 20000);
*/
  /*
	//	res = FAlphaBeta(m_nMaxDeepth, -20000, 20000);

	res = FAlphaBeta(m_nMaxDeepth, -10, 10);
	
	if (res < -10)
		FAlphaBeta(m_nMaxDeepth, -20000, res);

	if (res > 10)
		FAlphaBeta(m_nMaxDeepth, res, 20000);
*/
	MakeMove(&m_cmBestMove);
	memcpy(position, CurPosition, 90);
	return 0;
}

int CFAlphaBetaAndTT::FAlphaBeta(int depth, int alpha, int beta)
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

	if (depth > 1)
	{
		for (i=0;i<Count;i++) 
		{
//			Hash_MakeMove(&m_pMG->m_MoveList[depth][i], CurPosition);
			type = MakeMove(&m_pMG->m_MoveList[depth][i]);
			
//			score = LookUpHashTable(alpha, beta, 0);
			
//			if (score == 66666)
				m_pMG->m_MoveList[depth][i].Score = m_pEval->Eveluate(CurPosition,(m_nMaxDepth-depth+1)%2);
//			else
//			{
//		hcount++;
//			}
//			Hash_UnMakeMove(&m_pMG->m_MoveList[depth][i],type, CurPosition); 
			UnMakeMove(&m_pMG->m_MoveList[depth][i],type); 
		}
		
	MergeSort(m_pMG->m_MoveList[depth],Count,1);
	}     
	int eval_is_exact = 0;

	for (i=0;i<Count;i++) 
	{

		Hash_MakeMove(&m_pMG->m_MoveList[depth][i], CurPosition);
		type = MakeMove(&m_pMG->m_MoveList[depth][i]);

		score = -FAlphaBeta(depth - 1, -beta, -alpha);
		
		Hash_UnMakeMove(&m_pMG->m_MoveList[depth][i],type, CurPosition); 
		UnMakeMove(&m_pMG->m_MoveList[depth][i],type); 

				
		if (score >= current)
		{
			current = score;
			if(depth == m_nMaxDepth)
				m_cmBestMove = m_pMG->m_MoveList[depth][i];
			if (score > beta) //beta剪枝
			{
				EnterHashTable(lower_bound, score, depth,(m_nMaxDepth-depth)%2);
				return current;
			}
			if (score >= alpha)
			{
				eval_is_exact = 1;
				alpha = score;
			}
		}

	}

    if (eval_is_exact) 
		EnterHashTable(exact, alpha, depth,(m_nMaxDepth-depth)%2);
    else 
		EnterHashTable(upper_bound, current, depth,(m_nMaxDepth-depth)%2);
	return current;
}

