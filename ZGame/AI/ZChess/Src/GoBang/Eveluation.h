// Eveluation.h: interface for the CEveluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVELUATION_H__9008C0A2_E110_45CC_8C46_1015E86736E4__INCLUDED_)
#define AFX_EVELUATION_H__9008C0A2_E110_45CC_8C46_1015E86736E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//����궨�������Դ��������͵�����
#define STWO		1  //�߶�
#define STHREE		2  //����
#define SFOUR		3  //����
#define TWO			4  //���
#define THREE		5  //����
#define FOUR		6  //����
#define FIVE		7  //����
#define NOTYPE		11 //δ����
#define ANALSISED   255//�ѷ�������
#define TOBEANALSIS 0  //�ѷ�������

//����λ�ü�ֵ��
extern int PosValue[15][15];

class CEveluation  
{
public:
	CEveluation();
	virtual ~CEveluation();

public:
	virtual int Eveluate(BYTE position[][GRID_NUM ],BOOL bIsWhiteTurn);//��ֵ����,�Դ�������̴��,bIsWhiteTurn�����ֵ�˭����

protected:
	int AnalysisHorizon(BYTE position[][GRID_NUM],int i,int j); //����ˮƽ������ĳ�㼰���ܱߵ�����	
	int AnalysisVertical(BYTE position[][GRID_NUM],int i,int j);//������ֱ������ĳ�㼰���ܱߵ�����
	int AnalysisLeft(BYTE position[][GRID_NUM],int i,int j);    //������б45�ȷ�����ĳ�㼰���ܱߵ�����
	int AnalysisRight(BYTE position[][GRID_NUM],int i,int j);   //������б45�ȷ�����ĳ�㼰���ܱߵ�����
	int AnalysisLine(BYTE* position,int GridNum,int StonePos);  //������������ĳ�㼰���ܱߵ�����
	
protected:
	BYTE m_LineRecord[30];				   //���AnalysisLine�������������
	int TypeRecord[GRID_NUM ][GRID_NUM][4];//���ȫ���������������,������ά��,���ڴ��ˮƽ����ֱ����б����б 4 ���������������ͷ������
	int TypeCount[2][20];				   //���ͳ�ǹ��ķ������������
};

#endif // !defined(AFX_EVELUATION_H__9008C0A2_E110_45CC_8C46_1015E86736E4__INCLUDED_)
