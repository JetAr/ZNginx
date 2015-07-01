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

// AspirationSearch.cpp: implementation of the CAspirationSearch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "AspirationSearch.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAspirationSearch::CAspirationSearch()
{
}

CAspirationSearch::~CAspirationSearch()
{
}

void CAspirationSearch::SearchAGoodMove(BYTE position[10][9])
{
    int x,y;
    memcpy(CurPosition, position, 90);
    m_nMaxDepth = m_nSearchDepth-1;
    x= FAlphaBeta(m_nMaxDepth, -20000, 20000);
    m_nMaxDepth = m_nSearchDepth;
    y = FAlphaBeta(m_nMaxDepth, x-50, x+50);
    if (y < x-50)
        FAlphaBeta(m_nMaxDepth, -20000, y);
    if (y > x+50)
        FAlphaBeta(m_nMaxDepth, y, 20000);
    MakeMove(&m_cmBestMove);
    memcpy(position, CurPosition, 90);
    //z return 0;
}