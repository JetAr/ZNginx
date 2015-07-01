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

// Eveluation.cpp: implementation of the CEveluation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "Eveluation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int BA0[10][9]=
{
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{90,90,110,120,120,120,110,90,90},
	{90,90,110,120,120,120,110,90,90},
	{70,90,110,110,110,110,110,90,70},
	{70,70,70, 70, 70,  70, 70,70,70},
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
};

const int BA1[10][9]=
{
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{0,  0,  0,  0,  0,  0,  0,  0,  0},
	{70,70,70, 70, 70,70, 70,70, 70},
	{70,90,110,110,110,110,110,90,70},
	{90,90,110,120,120,120,110,90,90},
	{90,90,110,120,120,120,110,90,90},
	{0,  0, 0,  0,  0,  0,  0,  0,  0},
};

int CEveluation:: GetBingValue(int x, int y, BYTE CurSituation[][9])
{
	if (CurSituation[y][x] == R_PAWN)
		return BA0[y][x];
	
	if (CurSituation[y][x] == B_PAWN)
		return BA1[y][x];

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEveluation::CEveluation()
{
	m_BaseValue[B_KING] = BASEVALUE_KING; 
	m_BaseValue[B_CAR] = BASEVALUE_CAR; 
	m_BaseValue[B_HORSE] = BASEVALUE_HORSE; 
	m_BaseValue[B_BISHOP] = BASEVALUE_BISHOP; 
	m_BaseValue[B_ELEPHANT] = BASEVALUE_ELEPHANT; 
	m_BaseValue[B_CANON] = BASEVALUE_CANON; 
	m_BaseValue[B_PAWN] = BASEVALUE_PAWN; 

	m_BaseValue[R_KING] = BASEVALUE_KING; 
	m_BaseValue[R_CAR] = BASEVALUE_CAR; 
	m_BaseValue[R_HORSE] = BASEVALUE_HORSE; 
	m_BaseValue[R_BISHOP] = BASEVALUE_BISHOP; 
	m_BaseValue[R_ELEPHANT] = BASEVALUE_ELEPHANT; 
	m_BaseValue[R_CANON] = BASEVALUE_CANON; 
	m_BaseValue[R_PAWN] = BASEVALUE_PAWN; 

	m_FlexValue[B_KING] = FLEXIBILITY_KING; 
	m_FlexValue[B_CAR] = FLEXIBILITY_CAR; 
	m_FlexValue[B_HORSE] = FLEXIBILITY_HORSE; 
	m_FlexValue[B_BISHOP] = FLEXIBILITY_BISHOP; 
	m_FlexValue[B_ELEPHANT] = FLEXIBILITY_ELEPHANT; 
	m_FlexValue[B_CANON] = FLEXIBILITY_CANON; 
	m_FlexValue[B_PAWN] = FLEXIBILITY_PAWN; 

	m_FlexValue[R_KING] = FLEXIBILITY_KING; 
	m_FlexValue[R_CAR] = FLEXIBILITY_CAR; 
	m_FlexValue[R_HORSE] = FLEXIBILITY_HORSE; 
	m_FlexValue[R_BISHOP] = FLEXIBILITY_BISHOP; 
	m_FlexValue[R_ELEPHANT] = FLEXIBILITY_ELEPHANT; 
	m_FlexValue[R_CANON] = FLEXIBILITY_CANON; 
	m_FlexValue[R_PAWN] = FLEXIBILITY_PAWN; 

}

CEveluation::~CEveluation()
{
}
int count=0;
int CEveluation::Eveluate(BYTE position[10][9], BOOL bIsRedTurn)
{
	int i, j, k;
	int nChessType, nTargetType;
	count++;

	memset(m_chessValue,0, 360);
	memset(m_AttackPos,0, 180); 
	memset(m_GuardPos,0, 90); 
	memset(m_FlexibilityPos, 0, 90);

	for(i = 0; i < 10; i++)
		for(j = 0; j < 9; j++)
		{
			if(position[i][j] != NOCHESS)
			{
				nChessType = position[i][j];
				GetRelatePiece(position, j, i);
				for (k = 0; k < nPosCount; k++)
				{
					nTargetType = position[RelatePos[k].y][RelatePos[k].x];
					if (nTargetType == NOCHESS)
					{
						m_FlexibilityPos[i][j]++;	
					}
					else
					{
						if (IsSameSide(nChessType, nTargetType))
						{
							m_GuardPos[RelatePos[k].y][RelatePos[k].x]++;
						}else
						{
							m_AttackPos[RelatePos[k].y][RelatePos[k].x]++;
							m_FlexibilityPos[i][j]++;	
							switch (nTargetType)
							{
							case R_KING:
								if (!bIsRedTurn)
									return 18888;
								break;
							case B_KING:
								if (bIsRedTurn)
									return 18888;
								break;
							default:
								m_AttackPos[RelatePos[k].y][RelatePos[k].x] += (30 + (m_BaseValue[nTargetType] - m_BaseValue[nChessType])/10)/10;
								break;
							}
						}
					}
				}
			}
		}

	for(i = 0; i < 10; i++)
		for(j = 0; j < 9; j++)
		{
			if(position[i][j] != NOCHESS)
			{
				nChessType = position[i][j];
				m_chessValue[i][j]++;
				m_chessValue[i][j] += m_FlexValue[nChessType] * m_FlexibilityPos[i][j];
				m_chessValue[i][j] += GetBingValue(j, i, position);
			}
		}
	int nHalfvalue;
	for(i = 0; i < 10; i++)
		for(j = 0; j < 9; j++)
		{
			if(position[i][j] != NOCHESS)
			{
				nChessType = position[i][j];
				nHalfvalue = m_BaseValue[nChessType]/16;
				m_chessValue[i][j] += m_BaseValue[nChessType];
				
				if (IsRed(nChessType))
				{
					if (m_AttackPos[i][j])
					{
						if (bIsRedTurn)
						{
							if (nChessType == R_KING)
							{
								m_chessValue[i][j]-= 20;
							}
							else
							{
								m_chessValue[i][j] -= nHalfvalue * 2;
								if (m_GuardPos[i][j])
									m_chessValue[i][j] += nHalfvalue;
							}
						}
						else
						{
							if (nChessType == R_KING)
								return 18888;
							m_chessValue[i][j] -= nHalfvalue*10;
							if (m_GuardPos[i][j])
								m_chessValue[i][j] += nHalfvalue*9;
						}
						m_chessValue[i][j] -= m_AttackPos[i][j];
					}
					else
					{
						if (m_GuardPos[i][j])
							m_chessValue[i][j] += 5;
					}
				}
				else
				{
					if (m_AttackPos[i][j])
					{
						if (!bIsRedTurn)
						{
							if (nChessType == B_KING)									
							{
								m_chessValue[i][j]-= 20;
							}
							else
							{
								m_chessValue[i][j] -= nHalfvalue * 2;
								if (m_GuardPos[i][j])
									m_chessValue[i][j] += nHalfvalue;
							}
						}
						else
						{
							if (nChessType == B_KING)
								return 18888;
							m_chessValue[i][j] -= nHalfvalue*10;
							if (m_GuardPos[i][j])
								m_chessValue[i][j] += nHalfvalue*9;
						}
						m_chessValue[i][j] -= m_AttackPos[i][j];
					}
					else
					{
						if (m_GuardPos[i][j])
							m_chessValue[i][j] += 5;
					}
				}
			}
		}

	int nRedValue = 0; 
	int	nBlackValue = 0;

	for(i = 0; i < 10; i++)
		for(j = 0; j < 9; j++)
		{
			nChessType = position[i][j];
//			if (nChessType == R_KING || nChessType == B_KING)
//				m_chessValue[i][j] = 10000;	
			if (nChessType != NOCHESS)
			{
				if (IsRed(nChessType))
				{
					nRedValue += m_chessValue[i][j];	
				}
				else
				{
					nBlackValue += m_chessValue[i][j];	
				}
			}
		}
		if (bIsRedTurn)
		{
			return nRedValue - nBlackValue;
		}
		else
		{
			return  nBlackValue-nRedValue ;
		}
}





void CEveluation::AddPoint(int  x, int y)
{
	RelatePos[nPosCount].x = x;
	RelatePos[nPosCount].y = y;
	nPosCount++;
}

int CEveluation::GetRelatePiece(BYTE position[10][9], int j, int i)
{
	nPosCount = 0;
	BYTE nChessID;
	BYTE flag;
	int x,y;
	
	nChessID = position[i][j];
	switch(nChessID)
	{
	case R_KING:
	case B_KING:
		
		for (y = 0; y < 3; y++)
			for (x = 3; x < 6; x++)
				if (CanTouch(position, j, i, x, y))
					AddPoint(x, y);
		for (y = 7; y < 10; y++)
			for (x = 3; x < 6; x++)
				if (CanTouch(position, j, i, x, y))
					AddPoint(x, y);
		break;
						
	case R_BISHOP:
		
		for (y = 7; y < 10; y++)
			for (x = 3; x < 6; x++)
				if (CanTouch(position, j, i, x, y))
					AddPoint(x, y);
		break;
				
	case B_BISHOP:
		
		for (y = 0; y < 3; y++)
			for (x = 3; x < 6; x++)
				if (CanTouch(position, j, i, x, y))
					AddPoint(x, y);
		break;
				
	case R_ELEPHANT:
	case B_ELEPHANT:
		
		x=j+2;
		y=i+2;
		if(x < 9 && y < 10  && CanTouch(position, j, i, x, y))
			AddPoint(x, y);
		
		x=j+2;
		y=i-2;
		if(x < 9 && y>=0  &&  CanTouch(position, j, i, x, y))
			AddPoint(x, y);
		
		x=j-2;
		y=i+2;
		if(x>=0 && y < 10  && CanTouch(position, j, i, x, y))
			AddPoint(x, y);
		
		x=j-2;
		y=i-2;
		if(x>=0 && y>=0  && CanTouch(position, j, i, x, y))
			AddPoint(x, y);
		break;
		
		case R_HORSE:		
		case B_HORSE:		
			x=j+2;
			y=i+1;
			if((x < 9 && y < 10) &&CanTouch(position, j, i, x, y))
				AddPoint(x, y);
					
					x=j+2;
					y=i-1;
					if((x < 9 && y >= 0) &&CanTouch(position, j, i, x, y))
						AddPoint(x, y);
					
					x=j-2;
					y=i+1;
					if((x >= 0 && y < 10) &&CanTouch(position, j, i, x, y))
						AddPoint(x, y);
					
					x=j-2;
					y=i-1;
					if((x >= 0 && y >= 0) &&CanTouch(position, j, i, x, y))
						AddPoint(x, y);
					
					x=j+1;
					y=i+2;
					if((x < 9 && y < 10) &&CanTouch(position, j, i, x, y))
						AddPoint(x, y);
					x=j-1;
					y=i+2;
					if((x >= 0 && y < 10) &&CanTouch(position, j, i, x, y))
						AddPoint(x, y);
					x=j+1;
					y=i-2;
					if((x < 9 && y >= 0) &&CanTouch(position, j, i, x, y))
						AddPoint(x, y);
					x=j-1;
					y=i-2;
					if((x >= 0 && y >= 0) &&CanTouch(position, j, i, x, y))
						AddPoint(x, y);
					break;
					
				case R_CAR:
				case B_CAR:
					x=j+1;
					y=i;
					while(x < 9)
					{
						if( NOCHESS == position[y][x] )
							AddPoint(x, y);
						else
						{
							AddPoint(x, y);
							break;
						}
						x++;
					}
					
					x = j-1;
					y = i;
					while(x >= 0)
					{
						if( NOCHESS == position[y][x] )
							AddPoint(x, y);
						else
						{
							AddPoint(x, y);
							break;
						}
						x--;
					}
					
					x=j;
					y=i+1;//
					while(y < 10)
					{
						if( NOCHESS == position[y][x])
							AddPoint(x, y);
						else
						{
							AddPoint(x, y);
							break;
						}
						y++;
					}
					
					x = j;
					y = i-1;//�W��
					while(y>=0)
					{
						if( NOCHESS == position[y][x])
							AddPoint(x, y);
						else
						{
							AddPoint(x, y);
							break;
						}
						y--;
					}
					break;
					
				case R_PAWN:
					y = i - 1;
					x = j;
					
					if(y >= 0)
						AddPoint(x, y);
					
					if(i < 5)
					{
						y=i;
						x=j+1;
						if(x < 9 )
							AddPoint(x, y);
						x=j-1;
						if(x >= 0 )
							AddPoint(x, y);
					}
					break;
					
				case B_PAWN:
					y = i + 1;
					x = j;
					
					if(y < 10 )
						AddPoint(x, y);
					
					if(i > 4)
					{
						y=i;
						x=j+1;
						if(x < 9)
							AddPoint(x, y);
						x=j-1;
						if(x >= 0)
							AddPoint(x, y);
					}
					break;
					
				case B_CANON:
				case R_CANON:
					
					x=j+1;		//�k
					y=i;
					flag=FALSE;
					while(x < 9)		
					{
						if( NOCHESS == position[y][x] )
						{
							if(!flag)
								AddPoint(x, y);
						}
						else
						{
							if(!flag)
								flag=TRUE;
							else 
							{
								AddPoint(x, y);
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
								AddPoint(x, y);
						}
						else
						{
							if(!flag)
								flag=TRUE;
							else 
							{
								AddPoint(x, y);
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
								AddPoint(x, y);
						}
						else
						{
							if(!flag)
								flag=TRUE;
							else 
							{
								AddPoint(x, y);
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
								AddPoint(x, y);
						}
						else
						{
							if(!flag)
								flag=TRUE;
							else 
							{
								AddPoint(x, y);
								break;
							}
						}
						y--;
					}
					break;
					
				default:
					break;
					
				}
				return nPosCount ;				
}

BOOL CEveluation::CanTouch(BYTE position[10][9], int nFromX, int nFromY, int nToX, int nToY)
{
	int i, j;
	int nMoveChessID, nTargetID;
	
	if (nFromY ==  nToY && nFromX == nToX)
		return FALSE;//Ŀ����Դ��ͬ
	
	nMoveChessID = position[nFromY][nFromX];
	nTargetID = position[nToY][nToX];
	
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
		
		if(position[j][i] != (BYTE)NOCHESS)
			return FALSE;//������
		
		break;
	case B_CANON:    
	case R_CANON:    
		
		if(nFromY!=nToY && nFromX!=nToX)
			return FALSE;	//����ֱ��:
		
		//�ڲ�����ʱ������·���в���������:------------------
		
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
		//�������ڲ�����-------------------------------------
		//����ʱ:=======================================
		else	
		{
			int nCount=0;
			if(nFromY == nToY)
			{
				if(nFromX < nToX)
				{
					for(i=nFromX+1;i<nToX;i++)
						if(position[nFromY][i]!=NOCHESS)
							nCount++;
						if(nCount != 1)
							return FALSE;
				}
				else
				{
					for(i=nToX+1;i<nFromX;i++)
						if(position[nFromY][i] != NOCHESS)
							nCount++;
						if(nCount!=1)
							return FALSE;
				}
			}
			else
			{
				if(nFromY<nToY)
				{
					for(j=nFromY+1;j<nToY;j++)
						if(position[j][nFromX]!=NOCHESS)
							nCount++;
						if(nCount!=1)
							return FALSE;
				}
				else
				{
					for(j=nToY+1;j<nFromY;j++)
						if(position[j][nFromX] != NOCHESS)
							nCount++;
						if(nCount!=1)
							return FALSE;
				}
			}
		}
		//�������ڳ���ʱ================================
		break;
	default:
		return FALSE;
	}
	
	return TRUE;
}


