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

//define.h // data structure and macros
#ifndef define_h_
#define define_h_

#define NOCHESS 0    //û������

#define B_KING       1  //��˧
#define B_CAR        2  //�ڳ�
#define B_HORSE      3  //����
#define B_CANON      4  //����
#define B_BISHOP     5  //��ʿ
#define B_ELEPHANT   6  //����
#define B_PAWN       7  //����
#define B_BEGIN      B_KING
#define B_END        B_PAWN

#define R_KING       8   //��˧
#define R_CAR        9   //�쳵
#define R_HORSE      10  //����
#define R_CANON      11  //����
#define R_BISHOP     12  //��ʿ
#define R_ELEPHANT   13  //����
#define R_PAWN       14  //���
#define R_BEGIN      R_KING
#define R_END        R_PAWN

#define IsBlack(x)  (x>=B_BEGIN&&x<=B_END)
#define IsRed(x)  (x>=R_BEGIN&&x<=R_END)
#define IsSameSide(x,y)  ((IsBlack(x)&&IsBlack(y))||(IsRed(x)&&IsRed(y)))

typedef struct _chessmanposition
{
    BYTE		x;
    BYTE		y;
} CHESSMANPOS;

typedef struct _chessmove
{
    short	  	    ChessID;	//
    CHESSMANPOS	From;
    CHESSMANPOS	To;			//
    int			    Score;		//
} CHESSMOVE;

#endif //define_h_