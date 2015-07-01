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
	delete m_pMG;  //删去挂在搜索引擎上的走法产生器
	delete m_pEval;//删去挂在搜索引擎上的估值核心
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
	int score,i;//计算要下的棋子颜色
	
	i=(m_nMaxDepth-nDepth)%2;
	score=m_pEval->Eveluate(position,i);//调用估值函数
	if(abs(score)>8000)//如果估值函数返回极值，给定局面游戏结束
		return score;//返回极值
	
	return 0;//返回未结束
}