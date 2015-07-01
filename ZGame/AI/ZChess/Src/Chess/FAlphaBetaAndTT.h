// FAlphaBetaAndTT.h: interface for the CFAlphaBetaAndTT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FALPHABETAANDTT_H__716F8222_CEEA_11D5_AEC7_5254AB2E22C7__INCLUDED_)
#define AFX_FALPHABETAANDTT_H__716F8222_CEEA_11D5_AEC7_5254AB2E22C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FAlphaBetaEngine.h"
#include "TranspositionTable.h"

class CFAlphaBetaAndTT : public CFAlphaBetaEngine, public CTranspositionTable
{
public:
	CFAlphaBetaAndTT();
	virtual ~CFAlphaBetaAndTT();
    virtual SearchAGoodMove(BYTE position[10][9]);
	int FAlphaBeta(int depth, int alpha, int beta);

};

#endif // !defined(AFX_FALPHABETAANDTT_H__716F8222_CEEA_11D5_AEC7_5254AB2E22C7__INCLUDED_)
