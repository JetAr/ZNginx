//*******************************************************************************
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

// SearchEngine.cpp: implementation of the CSearchEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "SearchEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSearchEngine::CSearchEngine()
{
}

CSearchEngine::~CSearchEngine()
{
    delete m_pMG;
    delete m_pEval;
}

BYTE CSearchEngine::MakeMove(CHESSMOVE* move)
{
    BYTE nChessID;
    nChessID = CurPosition[move->To.y][move->To.x];
    CurPosition[move->To.y][move->To.x] = CurPosition[move->From.y][move->From.x];
    CurPosition[move->From.y][move->From.x] = NOCHESS;
    return nChessID;
}

void CSearchEngine::UnMakeMove(CHESSMOVE* move,BYTE nChessID)
{
    CurPosition[move->From.y][move->From.x] = CurPosition[move->To.y][move->To.x];
    CurPosition[move->To.y][move->To.x] = nChessID;
}


int CSearchEngine::IsGameOver(BYTE position[10][9], int nDepth)
{
    int i,j;
    BOOL RedLive = FALSE, BlackLive=FALSE;

    for (i = 7; i < 10; i++)
        for (j = 3; j < 6; j++)
        {
            if (position[i][j] == B_KING)
                BlackLive = TRUE;
            if (position[i][j] == R_KING)
                RedLive  = TRUE;
        }

    for (i = 0; i < 3; i++)
        for (j = 3; j < 6; j++)
        {
            if (position[i][j] == B_KING)
                BlackLive = TRUE;
            if (position[i][j] == R_KING)
                RedLive  = TRUE;
        }

    i = (m_nMaxDepth - nDepth + 1) % 2;

    if (!RedLive)
        if (i)
            return 19990 + nDepth;
        else
            return -19990 - nDepth;
    if (!BlackLive)
        if (i)
            return -19990 - nDepth;
        else
            return 19990 + nDepth;
    return 0;
}
