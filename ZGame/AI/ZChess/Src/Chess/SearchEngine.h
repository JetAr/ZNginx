// COPYRIGHT NOTES
// ---------------
// This source code is a part of chess which is an example of <Game Programing guide>.
// You may use, compile or redistribute it as part of your application
// for free.
// You cannot redistribute sources without the official agreement of the author.
// If distribution of you application which contents code below was occured, place
// e-mail <hidebug@hotmail.com> on it is to be appreciated.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
//
// Spring Wang <hidebug@hotmail.com>

// ---------------
// 版权声明
// ---------------
// 本文件所含之代码是《人机博弈程序设计指南》的范例程序中国象棋的一部分
// 您可以免费的使用, 编译 或者作为您应用程序的一部分。
// 但，您不能在未经作者书面许可的情况下分发此源代码。
// 如果您的应用程序使用了这些代码，在您的应用程序界面上
// 放入 e-mail <hidebug@hotmail.com> 是令人欣赏的做法。
// 此代码并不含有任何保证，使用者当自承风险。
//
// 王小春 <hidebug@hotmail.com>

// SearchEngine.h: interface for the CSearchEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCHENGINE_H__2AF7A220_CB28_11D5_AEC7_5254AB2E22C7__INCLUDED_)
#define AFX_SEARCHENGINE_H__2AF7A220_CB28_11D5_AEC7_5254AB2E22C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MoveGenerator.h"
#include "Eveluation.h"

class CSearchEngine
{
public:
    CSearchEngine();
    virtual ~CSearchEngine();

    virtual void SearchAGoodMove(BYTE position[10][9])=0;

    virtual void SetSearchDepth(int nDepth)
    {
        m_nSearchDepth = nDepth;
    };

    virtual void SetEveluator(CEveluation *pEval)
    {
        m_pEval = pEval;
    };

    virtual void SetMoveGenerator(CMoveGenerator *pMG)
    {
        m_pMG = pMG;
    };

protected:
    virtual BYTE MakeMove(CHESSMOVE* move);
    virtual void UnMakeMove(CHESSMOVE* move,BYTE type);
    virtual int IsGameOver(BYTE position[10][9], int nDepth);
    BYTE CurPosition[10][9];
    CHESSMOVE m_cmBestMove;
    CMoveGenerator *m_pMG;
    CEveluation *m_pEval;
    int m_nSearchDepth;
    int m_nMaxDepth;
};

#endif // !defined(AFX_SEARCHENGINE_H__2AF7A220_CB28_11D5_AEC7_5254AB2E22C7__INCLUDED_)
