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
// ��Ȩ����
// ---------------
// ���ļ�����֮�����ǡ��˻����ĳ������ָ�ϡ��ķ��������й������һ����
// ��������ѵ�ʹ��, ���� ������Ϊ��Ӧ�ó����һ���֡�
// ������������δ������������ɵ�����·ַ���Դ���롣
// �������Ӧ�ó���ʹ������Щ���룬������Ӧ�ó��������
// ���� e-mail <hidebug@hotmail.com> ���������͵�������
// �˴��벢�������κα�֤��ʹ���ߵ��Գз��ա�
//
// ��С�� <hidebug@hotmail.com>

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
