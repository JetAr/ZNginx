// NegaScout.h: interface for the CNegaScout class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEGASCOUT_H__318EB5E0_DC23_11D5_AEC7_5254AB2E22C7__INCLUDED_)
#define AFX_NEGASCOUT_H__318EB5E0_DC23_11D5_AEC7_5254AB2E22C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SearchEngine.h"
#include "TranspositionTable.h"
#include "HistoryHeuristic.h"

class CNegaScout : public CSearchEngine,
                   public CTranspositionTable,
				   public CHistoryHeuristic
{
public:
	CNegaScout();
	virtual ~CNegaScout();

	virtual void SearchAGoodMove(BYTE position[10][9]);

protected:
	int NegaScout(int depth, int alpha, int beta);

};

#endif // !defined(AFX_NEGASCOUT_H__318EB5E0_DC23_11D5_AEC7_5254AB2E22C7__INCLUDED_)
