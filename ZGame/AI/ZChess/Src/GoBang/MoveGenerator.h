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
    STONEMOVE m_MoveList[10][225];//用以记录走法的数组

protected:
    int m_nMoveCount;//此变量用以记录走法的总数
};

#endif // !defined(AFX_MOVEGENERATOR_H__BB5A9F7B_7AFA_4FFF_B48D_86F04CB429FC__INCLUDED_)
