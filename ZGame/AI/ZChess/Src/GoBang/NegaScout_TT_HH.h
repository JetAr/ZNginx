// NegaScout_TT_HH.h: interface for the CNegaScout_TT_HH class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEGASCOUT_TT_HH_H__DB5C0A53_E715_4037_A42B_B8D5704521B4__INCLUDED_)
#define AFX_NEGASCOUT_TT_HH_H__DB5C0A53_E715_4037_A42B_B8D5704521B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SearchEngine.h"
#include "TranspositionTable.h"
#include "HistoryHeuristic.h"

class CNegaScout_TT_HH:public CTranspositionTable,public CHistoryHeuristic,public CSearchEngine
{
public:
    CNegaScout_TT_HH();
    virtual ~CNegaScout_TT_HH();

public:
    virtual void SearchAGoodMove(BYTE position[GRID_NUM][GRID_NUM],int Type);

protected:
    int NegaScout(int depth, int alpha, int beta);//NegaScoutËÑË÷º¯Êý
};

#endif // !defined(AFX_NEGASCOUT_TT_HH_H__DB5C0A53_E715_4037_A42B_B8D5704521B4__INCLUDED_)
