// SearchEngine.cpp: implementation of the CSearchEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Renju.h"
#include "SearchEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSearchEngine::CSearchEngine()
{

}

CSearchEngine::~CSearchEngine()
{
	delete m_pMG;  //ɾȥ�������������ϵ��߷�������
	delete m_pEval;//ɾȥ�������������ϵĹ�ֵ����
}

BYTE CSearchEngine::MakeMove(STONEMOVE* move,int type)
{
	CurPosition[move->StonePos.y][move->StonePos.x]=type;
	
	return 0;
}

void CSearchEngine::UnMakeMove(STONEMOVE* move)
{
	CurPosition[move->StonePos.y][move->StonePos.x]=NOSTONE;
}

int CSearchEngine::IsGameOver(BYTE position[][GRID_NUM],int nDepth)
{
	int score,i;//����Ҫ�µ�������ɫ
	
	i=(m_nMaxDepth-nDepth)%2;
	score=m_pEval->Eveluate(position,i);//���ù�ֵ����
	if(abs(score)>8000)//�����ֵ�������ؼ�ֵ������������Ϸ����
		return score;//���ؼ�ֵ
	
	return 0;//����δ����
}