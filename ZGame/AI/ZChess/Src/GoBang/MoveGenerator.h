// MoveGenerator.h: interface for the CMoveGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVEGENERATOR_H__BB5A9F7B_7AFA_4FFF_B48D_86F04CB429FC__INCLUDED_)
#define AFX_MOVEGENERATOR_H__BB5A9F7B_7AFA_4FFF_B48D_86F04CB429FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HistoryHeuristic.h"
#include "Eveluation.h"

class CMoveGenerator
{
public:
    CMoveGenerator();
    virtual ~CMoveGenerator();

public:
    int CreatePossibleMove(BYTE position[GRID_NUM ][GRID_NUM ],int nPly,int nSide);

protected:
    int AddMove(int nToX,int nToY,int nPly);

public:
    STONEMOVE m_MoveList[10][225];//���Լ�¼�߷�������

protected:
    int m_nMoveCount;//�˱������Լ�¼�߷�������
};

#endif // !defined(AFX_MOVEGENERATOR_H__BB5A9F7B_7AFA_4FFF_B48D_86F04CB429FC__INCLUDED_)
