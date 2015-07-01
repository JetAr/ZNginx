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

    //ʹ����ʷ�������еľ�̬�鲢���������߷����н�������
    //����Ϊ����߼�֦Ч��
//	CHistoryHeuristic::MergeSort(m_MoveList[nPly],m_nMoveCount,0);
    CHistoryHeuristic history;
    history.MergeSort(m_MoveList[nPly],m_nMoveCount,0);

    return m_nMoveCount;//���غϷ��߷�����
}

//��m_MoveList�в���һ���߷�
//nToX��Ŀ��λ�ú�����
//nToY��Ŀ��λ��������
//nPly�Ǵ��߷����ڵĲ��
int CMoveGenerator::AddMove(int nToX, int nToY, int nPly)
{
    m_MoveList[nPly][m_nMoveCount].StonePos.x=nToX;
    m_MoveList[nPly][m_nMoveCount].StonePos.y=nToY;

    m_nMoveCount++;

    m_MoveList[nPly][m_nMoveCount].Score=PosValue[nToY][nToX];//ʹ��λ�ü�ֵ��������ǰ�߷��ļ�ֵ

    return m_nMoveCount;
}
