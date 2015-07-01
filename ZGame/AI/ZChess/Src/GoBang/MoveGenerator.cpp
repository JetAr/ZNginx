// MoveGenerator.cpp: implementation of the CMoveGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Renju.h"
#include "MoveGenerator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMoveGenerator::CMoveGenerator()
{

}

CMoveGenerator::~CMoveGenerator()
{

}

int CMoveGenerator::CreatePossibleMove(BYTE position[][GRID_NUM], int nPly, int nSide)
{
    int i,j;

    m_nMoveCount=0;

    for(i=0; i<GRID_NUM; i++)
        for(j=0; j<GRID_NUM; j++)
        {
            if(position[i][j]==(BYTE)NOSTONE)
                AddMove(j,i,nPly);
        }

    //使用历史启发类中的静态归并排序函数对走法队列进行排序
    //这是为了提高剪枝效率
//	CHistoryHeuristic::MergeSort(m_MoveList[nPly],m_nMoveCount,0);
    CHistoryHeuristic history;
    history.MergeSort(m_MoveList[nPly],m_nMoveCount,0);

    return m_nMoveCount;//返回合法走法个数
}

//在m_MoveList中插入一个走法
//nToX是目标位置横坐标
//nToY是目标位置纵坐标
//nPly是此走法所在的层次
int CMoveGenerator::AddMove(int nToX, int nToY, int nPly)
{
    m_MoveList[nPly][m_nMoveCount].StonePos.x=nToX;
    m_MoveList[nPly][m_nMoveCount].StonePos.y=nToY;

    m_nMoveCount++;

    m_MoveList[nPly][m_nMoveCount].Score=PosValue[nToY][nToX];//使用位置价值表评估当前走法的价值

    return m_nMoveCount;
}
