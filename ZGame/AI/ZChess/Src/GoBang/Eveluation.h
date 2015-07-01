// Eveluation.h: interface for the CEveluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVELUATION_H__9008C0A2_E110_45CC_8C46_1015E86736E4__INCLUDED_)
#define AFX_EVELUATION_H__9008C0A2_E110_45CC_8C46_1015E86736E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//这组宏定义了用以代表几种棋型的数字
#define STWO		1  //眠二
#define STHREE		2  //眠三
#define SFOUR		3  //冲四
#define TWO			4  //活二
#define THREE		5  //活三
#define FOUR		6  //活四
#define FIVE		7  //五连
#define NOTYPE		11 //未定义
#define ANALSISED   255//已分析过的
#define TOBEANALSIS 0  //已分析过的

//棋子位置价值表
extern int PosValue[15][15];

class CEveluation  
{
public:
	CEveluation();
	virtual ~CEveluation();

public:
	virtual int Eveluate(BYTE position[][GRID_NUM ],BOOL bIsWhiteTurn);//估值函数,对传入的棋盘打分,bIsWhiteTurn标明轮到谁走棋

protected:
	int AnalysisHorizon(BYTE position[][GRID_NUM],int i,int j); //分析水平方向上某点及其周边的棋型	
	int AnalysisVertical(BYTE position[][GRID_NUM],int i,int j);//分析垂直方向上某点及其周边的棋型
	int AnalysisLeft(BYTE position[][GRID_NUM],int i,int j);    //分析左斜45度方向上某点及其周边的棋型
	int AnalysisRight(BYTE position[][GRID_NUM],int i,int j);   //分析右斜45度方向上某点及其周边的棋型
	int AnalysisLine(BYTE* position,int GridNum,int StonePos);  //分析给定行上某点及其周边的棋型
	
protected:
	BYTE m_LineRecord[30];				   //存放AnalysisLine分析结果的数组
	int TypeRecord[GRID_NUM ][GRID_NUM][4];//存放全部分析结果的数组,有三个维度,用于存放水平、垂直、左斜、右斜 4 个方向上所有棋型分析结果
	int TypeCount[2][20];				   //存放统记过的分析结果的数组
};

#endif // !defined(AFX_EVELUATION_H__9008C0A2_E110_45CC_8C46_1015E86736E4__INCLUDED_)
