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

// PVS_Engine.cpp: implementation of the CPVS_Engine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "PVS_Engine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPVS_Engine::CPVS_Engine()
{

}

CPVS_Engine::~CPVS_Engine()
{

}
void CPVS_Engine::SearchAGoodMove(BYTE position[10][9])
{
	memcpy(CurPosition, position, 90);
	m_nMaxDepth = m_nSearchDepth;

//	for (m_nMaxDepth = 1; m_nMaxDepth <= m_nSearchDepth; m_nMaxDepth++)
	{
	   PrincipalVariation(m_nMaxDepth, -20000, 20000);
	}
	
	MakeMove(&m_cmBestMove);
	memcpy(position, CurPosition, 90);
}

int CPVS_Engine::PrincipalVariation(int depth, int alpha, int beta)
{
	int score;
	int Count,i;
	BYTE type;
	int best;

	i = IsGameOver(CurPosition, depth);
	if (i != 0)
		return i;

	if (depth <= 0)	//Ҷ�ӽڵ�ȡ��ֵ
		return m_pEval->Eveluate(CurPosition, (m_nMaxDepth-depth)%2);
	
	Count = m_pMG->CreatePossibleMove(CurPosition, depth, (m_nMaxDepth-depth)%2);
	

	type = MakeMove(&m_pMG->m_MoveList[depth][0]);
	best = -PrincipalVariation( depth-1, -beta, -alpha);
	UnMakeMove(&m_pMG->m_MoveList[depth][0],type); 
	if(depth == m_nMaxDepth)
		m_cmBestMove = m_pMG->m_MoveList[depth][0];

	for (i=1;i<Count;i++) 
	{
		
		if(best < beta) 
		{
			if (best > alpha) 
				alpha = best;
			type = MakeMove(&m_pMG->m_MoveList[depth][i]);
			score = -PrincipalVariation(depth-1, -alpha-1, -alpha);
			if (score > alpha && score < beta) 
			{
				best = -PrincipalVariation(depth-1, -beta, -score);
				if(depth == m_nMaxDepth)
					m_cmBestMove = m_pMG->m_MoveList[depth][i];
			}
			else if (score > best)
			{
				best = score;
				if(depth == m_nMaxDepth)
					m_cmBestMove = m_pMG->m_MoveList[depth][i];
			}
			UnMakeMove(&m_pMG->m_MoveList[depth][i],type); 
		}
	}

	return best;
}