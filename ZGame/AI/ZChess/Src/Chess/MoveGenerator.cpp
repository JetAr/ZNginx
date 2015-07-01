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

// MoveGenerator.cpp: implementation of the CMoveGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "MoveGenerator.h"

#ifdef _DEBUG
#undef THIS_FILE
static BYTE THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMoveGenerator::CMoveGenerator()
{

}

CMoveGenerator::~CMoveGenerator()
{

}

int CMoveGenerator::AddMove(int nFromX, int nFromY, int nToX, int nToY,int nPly)
{
    m_MoveList[nPly][m_nMoveCount].From.x = nFromX;
    m_MoveList[nPly][m_nMoveCount].From.y = nFromY;
    m_MoveList[nPly][m_nMoveCount].To.x = nToX;
    m_MoveList[nPly][m_nMoveCount].To.y = nToY;
    m_nMoveCount++;
    return m_nMoveCount;
}

int CMoveGenerator::CreatePossibleMove(BYTE position[10][9], int nPly, int nSide)
{
    int     nChessID;
    int		i,j;

    m_nMoveCount = 0;
    for (i = 0; i < 10; i++)
        for (j = 0; j < 9; j++)
        {
            if (position[i][j] != NOCHESS)
            {
                nChessID = position[i][j];
                if (!nSide && IsRed(nChessID))
                    continue;
                if (nSide && IsBlack(nChessID))
                    continue;
                switch(nChessID)
                {
                case R_KING:
                case B_KING:
                    Gen_KingMove(position, i, j, nPly);
                    break;

                case R_BISHOP:
                    Gen_RBishopMove(position, i, j, nPly);
                    break;

                case B_BISHOP:
                    Gen_BBishopMove(position, i, j, nPly);
                    break;

                case R_ELEPHANT:
                case B_ELEPHANT:
                    Gen_ElephantMove(position, i, j, nPly);
                    break;

                case R_HORSE:
                case B_HORSE:
                    Gen_HorseMove(position, i, j, nPly);
                    break;

                case R_CAR:
                case B_CAR:
                    Gen_CarMove(position, i, j, nPly);
                    break;

                case R_PAWN:
                    Gen_RPawnMove(position, i, j, nPly);
                    break;

                case B_PAWN:
                    Gen_BPawnMove(position, i, j, nPly);
                    break;

                case B_CANON:
                case R_CANON:
                    Gen_CanonMove(position, i, j, nPly);
                    break;

                default:
                    break;

                }
            }
        }

    return m_nMoveCount;
}



void CMoveGenerator::Gen_KingMove(BYTE position[10][9], int i, int j, int nPly)
{
    int x,  y;
    for (y = 0; y < 3; y++)
        for (x = 3; x < 6; x++)
            if (IsValidMove(position, j, i, x, y))
                AddMove(j, i, x, y, nPly);

    for (y = 7; y < 10; y++)
        for (x = 3; x < 6; x++)
            if (IsValidMove(position, j, i, x, y))
                AddMove(j, i, x, y, nPly);
}

void CMoveGenerator::Gen_RBishopMove(BYTE position[10][9], int i, int j, int nPly)
{
    int x,  y;
    for (y = 7; y < 10; y++)
        for (x = 3; x < 6; x++)
            if (IsValidMove(position, j, i, x, y))
                AddMove(j, i, x, y, nPly);
}

void CMoveGenerator::Gen_BBishopMove(BYTE position[10][9], int i, int j, int nPly)
{
    int x,  y;
    for (y = 0; y < 3; y++)
        for (x = 3; x < 6; x++)
            if (IsValidMove(position, j, i, x, y))
                AddMove(j, i, x, y, nPly);
}

void CMoveGenerator::Gen_ElephantMove(BYTE position[10][9], int i, int j, int nPly)
{
    int x,  y;

    x=j+2;
    y=i+2;
    if(x < 9 && y < 10  && IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);

    x=j+2;
    y=i-2;
    if(x < 9 && y>=0  &&  IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);

    x=j-2;
    y=i+2;
    if(x>=0 && y < 10  && IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);

    x=j-2;
    y=i-2;
    if(x>=0 && y>=0  && IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);

}

void CMoveGenerator::Gen_HorseMove(BYTE position[10][9], int i, int j, int nPly)
{
    int x,  y;

    x=j+2;
    y=i+1;
    if((x < 9 && y < 10) &&IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);

    x=j+2;
    y=i-1;
    if((x < 9 && y >= 0) &&IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);

    x=j-2;
    y=i+1;
    if((x >= 0 && y < 10) &&IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);

    x=j-2;
    y=i-1;
    if((x >= 0 && y >= 0) &&IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);

    x=j+1;
    y=i+2;
    if((x < 9 && y < 10) &&IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);
    x=j-1;
    y=i+2;
    if((x >= 0 && y < 10) &&IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);
    x=j+1;
    y=i-2;
    if((x < 9 && y >= 0) &&IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);
    x=j-1;
    y=i-2;
    if((x >= 0 && y >= 0) &&IsValidMove(position, j, i, x, y))
        AddMove(j, i, x, y, nPly);

}

void CMoveGenerator::Gen_RPawnMove(BYTE position[10][9], int i, int j, int nPly)
{
    int x, y;
    int nChessID;

    nChessID = position[i][j];

    y = i - 1;
    x = j;

    if(y > 0 && !IsSameSide(nChessID, position[y][x]))
        AddMove(j, i, x, y, nPly);

    if(i < 5)
    {
        y=i;
        x=j+1;
        if(x < 9 && !IsSameSide(nChessID, position[y][x]))
            AddMove(j, i, x, y, nPly);
        x=j-1;
        if(x >= 0 && !IsSameSide(nChessID, position[y][x]))
            AddMove(j, i, x, y, nPly);
    }
}


void CMoveGenerator::Gen_BPawnMove(BYTE position[10][9], int i, int j, int nPly)
{
    int x, y;
    int nChessID;

    nChessID = position[i][j];

    y = i + 1;
    x = j;

    if(y < 10 && !IsSameSide(nChessID, position[y][x]))
        AddMove(j, i, x, y, nPly);

    if(i > 4)
    {
        y=i;
        x=j+1;
        if(x < 9 && !IsSameSide(nChessID, position[y][x]))
            AddMove(j, i, x, y, nPly);
        x=j-1;
        if(x >= 0 && !IsSameSide(nChessID, position[y][x]))
            AddMove(j, i, x, y, nPly);
    }

}

void CMoveGenerator::Gen_CarMove(BYTE position[10][9], int i, int j, int nPly)
{
    int x,  y;
    int nChessID;

    nChessID = position[i][j];

    x=j+1;
    y=i;
    while(x < 9)
    {
        if( NOCHESS == position[y][x] )
            AddMove(j, i, x, y, nPly);
        else
        {
            if(!IsSameSide(nChessID, position[y][x]))
                AddMove(j, i, x, y, nPly);
            break;
        }
        x++;
    }

    x = j-1;
    y = i;
    while(x >= 0)
    {
        if( NOCHESS == position[y][x] )
            AddMove(j, i, x, y, nPly);
        else
        {
            if(!IsSameSide(nChessID, position[y][x]))
                AddMove(j, i, x, y, nPly);
            break;
        }
        x--;
    }

    x=j;
    y=i+1;//
    while(y < 10)
    {
        if( NOCHESS == position[y][x])
            AddMove(j, i, x, y, nPly);
        else
        {
            if(!IsSameSide(nChessID, position[y][x]))
                AddMove(j, i, x, y, nPly);
            break;
        }
        y++;
    }

    x = j;
    y = i-1;//
    while(y>=0)
    {
        if( NOCHESS == position[y][x])
            AddMove(j, i, x, y, nPly);
        else
        {
            if(!IsSameSide(nChessID, position[y][x]))
                AddMove(j, i, x, y, nPly);
            break;
        }
        y--;
    }
}

void CMoveGenerator::Gen_CanonMove(BYTE position[10][9], int i, int j, int nPly)
{
    int x, y;
    BOOL	flag;
    int nChessID;

    nChessID = position[i][j];

    x=j+1;		//�k
    y=i;
    flag=FALSE;
    while(x < 9)
    {
        if( NOCHESS == position[y][x] )
        {
            if(!flag)
                AddMove(j, i, x, y, nPly);
        }
        else
        {
            if(!flag)
                flag=TRUE;
            else
            {
                if(!IsSameSide(nChessID, position[y][x]))
                    AddMove(j, i, x, y, nPly);
                break;
            }
        }
        x++;
    }

    x=j-1;
    flag=FALSE;
    while(x>=0)
    {
        if( NOCHESS == position[y][x] )
        {
            if(!flag)
                AddMove(j, i, x, y, nPly);
        }
        else
        {
            if(!flag)
                flag=TRUE;
            else
            {
                if(!IsSameSide(nChessID, position[y][x]))
                    AddMove(j, i, x, y, nPly);
                break;
            }
        }
        x--;
    }
    x=j;
    y=i+1;
    flag=FALSE;
    while(y < 10)
    {
        if( NOCHESS == position[y][x] )
        {
            if(!flag)
                AddMove(j, i, x, y, nPly);
        }
        else
        {
            if(!flag)
                flag=TRUE;
            else
            {
                if(!IsSameSide(nChessID, position[y][x]))
                    AddMove(j, i, x, y, nPly);
                break;
            }
        }
        y++;
    }

    y=i-1;	//�W
    flag=FALSE;
    while(y>=0)
    {
        if( NOCHESS == position[y][x] )
        {
            if(!flag)
                AddMove(j, i, x, y, nPly);
        }
        else
        {
            if(!flag)
                flag=TRUE;
            else
            {
                if(!IsSameSide(nChessID, position[y][x]))
                    AddMove(j, i, x, y, nPly);
                break;
            }
        }
        y--;
    }

}

BOOL CMoveGenerator::IsValidMove(BYTE position[10][9], int nFromX, int nFromY, int nToX, int nToY)
{
    int i, j;
    int nMoveChessID, nTargetID;

    if (nFromY ==  nToY && nFromX == nToX)
        return FALSE;//Ŀ����Դ��ͬ

    nMoveChessID = position[nFromY][nFromX];
    nTargetID = position[nToY][nToX];

    if (IsSameSide(nMoveChessID, nTargetID))
        return FALSE;//���ܳ��Լ�����

    switch(nMoveChessID)
    {
    case B_KING:
        if (nTargetID == R_KING)//�Ͻ�����?
        {
            if (nFromX != nToX)
                return FALSE;
            for (i = nFromY + 1; i < nToY; i++)
                if (position[i][nFromX] != NOCHESS)
                    return FALSE;
        }
        else
        {
            if (nToY > 2 || nToX > 5 || nToX < 3)
                return FALSE;//Ŀ����ھŹ�֮��
            if(abs(nFromY - nToY) + abs(nToX - nFromX) > 1)
                return FALSE;//��˧ֻ��һ��ֱ��:
        }
        break;
    case R_BISHOP:

        if (nToY < 7 || nToX > 5 || nToX < 3)
            return FALSE;//ʿ���Ź�

        if (abs(nFromY - nToY) != 1 || abs(nToX - nFromX) != 1)
            return FALSE;	//ʿ��б��

        break;

    case B_BISHOP:   //��ʿ

        if (nToY > 2 || nToX > 5 || nToX < 3)
            return FALSE;//ʿ���Ź�

        if (abs(nFromY - nToY) != 1 || abs(nToX - nFromX) != 1)
            return FALSE;	//ʿ��б��

        break;

    case R_ELEPHANT://����

        if(nToY < 5)
            return FALSE;//�಻�ܹ���

        if(abs(nFromX-nToX) != 2 || abs(nFromY-nToY) != 2)
            return FALSE;//��������

        if(position[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NOCHESS)
            return FALSE;//���۱���ס��

        break;

    case B_ELEPHANT://����

        if(nToY > 4)
            return FALSE;//�಻�ܹ���

        if(abs(nFromX-nToX) != 2 || abs(nFromY-nToY) != 2)
            return FALSE;//��������

        if(position[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NOCHESS)
            return FALSE;//���۱���ס��

        break;

    case B_PAWN:     //�ڱ�

        if(nToY < nFromY)
            return FALSE;//������ͷ

        if( nFromY < 5 && nFromY == nToY)
            return FALSE;//������ǰֻ��ֱ��

        if(nToY - nFromY + abs(nToX - nFromX) > 1)
            return FALSE;//��ֻ��һ��ֱ��:

        break;

    case R_PAWN:    //���

        if(nToY > nFromY)
            return FALSE;//������ͷ

        if( nFromY > 4 && nFromY == nToY)
            return FALSE;//������ǰֻ��ֱ��

        if(nFromY - nToY + abs(nToX - nFromX) > 1)
            return FALSE;//��ֻ��һ��ֱ��:

        break;

    case R_KING:
        if (nTargetID == B_KING)//�Ͻ�����?
        {
            if (nFromX != nToX)
                return FALSE;//����������ͬһ��
            for (i = nFromY - 1; i > nToY; i--)
                if (position[i][nFromX] != NOCHESS)
                    return FALSE;//�м��б����
        }
        else
        {
            if (nToY < 7 || nToX > 5 || nToX < 3)
                return FALSE;//Ŀ����ھŹ�֮��
            if(abs(nFromY - nToY) + abs(nToX - nFromX) > 1)
                return FALSE;//��˧ֻ��һ��ֱ��:
        }
        break;

    case B_CAR:
    case R_CAR:

        if(nFromY != nToY && nFromX != nToX)
            return FALSE;	//����ֱ��:

        if(nFromY == nToY)
        {
            if(nFromX < nToX)
            {
                for(i = nFromX + 1; i < nToX; i++)
                    if(position[nFromY][i] != NOCHESS)
                        return FALSE;
            }
            else
            {
                for(i = nToX + 1; i < nFromX; i++)
                    if(position[nFromY][i] != NOCHESS)
                        return FALSE;
            }
        }
        else
        {
            if(nFromY < nToY)
            {
                for(j = nFromY + 1; j < nToY; j++)
                    if(position[j][nFromX] != NOCHESS)
                        return FALSE;
            }
            else
            {
                for(j= nToY + 1; j < nFromY; j++)
                    if(position[j][nFromX] != NOCHESS)
                        return FALSE;
            }
        }

        break;

    case B_HORSE:
    case R_HORSE:

        if(!((abs(nToX-nFromX)==1 && abs(nToY-nFromY)==2)
                ||(abs(nToX-nFromX)==2&&abs(nToY-nFromY)==1)))
            return FALSE;//��������

        if	(nToX-nFromX==2)
        {
            i=nFromX+1;
            j=nFromY;
        }
        else if	(nFromX-nToX==2)
        {
            i=nFromX-1;
            j=nFromY;
        }
        else if	(nToY-nFromY==2)
        {
            i=nFromX;
            j=nFromY+1;
        }
        else if	(nFromY-nToY==2)
        {
            i=nFromX;
            j=nFromY-1;
        }

        if(position[j][i] != NOCHESS)
            return FALSE;//������

        break;

    case B_CANON:
    case R_CANON:

        if(nFromY!=nToY && nFromX!=nToX)
            return FALSE;	//����ֱ��:

        //�ڲ�����ʱ������·���в���������

        if(position[nToY][nToX] == NOCHESS)
        {
            if(nFromY == nToY)
            {
                if(nFromX < nToX)
                {
                    for(i = nFromX + 1; i < nToX; i++)
                        if(position[nFromY][i] != NOCHESS)
                            return FALSE;
                }
                else
                {
                    for(i = nToX + 1; i < nFromX; i++)
                        if(position[nFromY][i]!=NOCHESS)
                            return FALSE;
                }
            }
            else
            {
                if(nFromY < nToY)
                {
                    for(j = nFromY + 1; j < nToY; j++)
                        if(position[j][nFromX] != NOCHESS)
                            return FALSE;
                }
                else
                {
                    for(j = nToY + 1; j < nFromY; j++)
                        if(position[j][nFromX] != NOCHESS)
                            return FALSE;
                }
            }
        }
        //�ڳ���ʱ
        else
        {
            int count=0;
            if(nFromY == nToY)
            {
                if(nFromX < nToX)
                {
                    for(i=nFromX+1; i<nToX; i++)
                        if(position[nFromY][i]!=NOCHESS)
                            count++;
                    if(count != 1)
                        return FALSE;
                }
                else
                {
                    for(i=nToX+1; i<nFromX; i++)
                        if(position[nFromY][i] != NOCHESS)
                            count++;
                    if(count!=1)
                        return FALSE;
                }
            }
            else
            {
                if(nFromY<nToY)
                {
                    for(j=nFromY+1; j<nToY; j++)
                        if(position[j][nFromX]!=NOCHESS)
                            count++;
                    if(count!=1)
                        return FALSE;
                }
                else
                {
                    for(j=nToY+1; j<nFromY; j++)
                        if(position[j][nFromX] != NOCHESS)
                            count++;
                    if(count!=1)
                        return FALSE;
                }
            }
        }
        break;
    default:
        return FALSE;
    }

    return TRUE;
}
