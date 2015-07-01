#ifndef define_h_
#define define_h_

#define BOARD_POS_X 0 //棋盘到父窗左边的距离
#define BOARD_POS_Y 0 //棋盘到父窗顶部的距离
#define BOARD_WIDTH 32 //棋盘上格子的宽度
#define GRID_NUM    15 //每一行(列)的棋盘交点数
#define GRID_COUNT  225//棋盘上交点总数
#define BLACK		0  //黑棋用0表示
#define WHITE		1  //白棋用1表示
#define NOSTONE 0xFF   //没有棋子

//这个宏用以检查某一坐标是否是棋盘上的有效落子点
#define IsValidPos(x,y) ((x>=0 && x<GRID_NUM) && (y>=0 && y<GRID_NUM)

//用以表示棋子位置的结构
typedef struct _stoneposition
{
	BYTE x;
	BYTE y;
}STONEPOS;

//这个结构用以表示走法
typedef struct _stonemove
{
	STONEPOS StonePos;//棋子位置
	int Score;		  //走法的分数
}STONEMOVE;

#endif