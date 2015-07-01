// MinimaxEngine.h: interface for the CMinimaxEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MINIMAXENGINE_H__6C3A4900_CDED_11D5_AEC7_5254AB2E22C7__INCLUDED_)
#define AFX_MINIMAXENGINE_H__6C3A4900_CDED_11D5_AEC7_5254AB2E22C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SearchEngine.h"

class CMinimaxEngine : public CSearchEngine  
{
public:
	CMinimaxEngine();
	virtual ~CMinimaxEngine();

};

#endif // !defined(AFX_MINIMAXENGINE_H__6C3A4900_CDED_11D5_AEC7_5254AB2E22C7__INCLUDED_)
