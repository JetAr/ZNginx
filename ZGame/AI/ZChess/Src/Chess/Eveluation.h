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

// Eveluation.h: interface for the CEveluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVELUATION_H__2AF7A221_CB28_11D5_AEC7_5254AB2E22C7__INCLUDED_)
#define AFX_EVELUATION_H__2AF7A221_CB28_11D5_AEC7_5254AB2E22C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define BASEVALUE_PAWN 100
#define BASEVALUE_BISHOP 250
#define BASEVALUE_ELEPHANT 250
#define BASEVALUE_CAR 500
#define BASEVALUE_HORSE 350
#define BASEVALUE_CANON 350
#define BASEVALUE_KING 10000

#define FLEXIBILITY_PAWN 15
#define FLEXIBILITY_BISHOP 1
#define FLEXIBILITY_ELEPHANT 1
#define FLEXIBILITY_CAR 6
#define FLEXIBILITY_HORSE 12
#define FLEXIBILITY_CANON 6
#define FLEXIBILITY_KING 0

class CEveluation
{
public:
    CEveluation();
    virtual ~CEveluation();

    virtual int Eveluate(BYTE position[10][9], BOOL bIsRedTurn);

protected:
    int GetRelatePiece(BYTE position[10][9],int j, int i);
    BOOL CanTouch(BYTE position[10][9], int nFromX, int nFromY, int nToX, int nToY);
    void AddPoint(int x, int y);
    int GetBingValue(int x, int y, BYTE CurSituation[][9]);
    int m_BaseValue[15];
    int m_FlexValue[15];
    short m_AttackPos[10][9];
    BYTE m_GuardPos[10][9];
    BYTE m_FlexibilityPos[10][9];
    int m_chessValue[10][9];
    int nPosCount;
    POINT RelatePos[20];

};

#endif // !defined(AFX_EVELUATION_H__2AF7A221_CB28_11D5_AEC7_5254AB2E22C7__INCLUDED_)
