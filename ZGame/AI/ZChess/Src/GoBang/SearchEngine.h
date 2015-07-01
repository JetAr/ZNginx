// SearchEngine.h: interface for the CSearchEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEARCHENGINE_H__0CFB7AC4_C204_470F_86D2_4C586642DEB8__INCLUDED_)
#define AFX_SEARCHENGINE_H__0CFB7AC4_C204_470F_86D2_4C586642DEB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HistoryHeuristic.h"
#include "Eveluation.h"
#include "MoveGenerator.h"

class CSearchEngine  
{
public:
	CSearchEngine();
	virtual ~CSearchEngine();

public:
	virtual void SearchAGoodMove(BYTE position[][GRID_NUM ],int Type)=0;//���Ľӿڣ�Ϊ��ǰ�����߳���һ��
	virtual void SetSearchDepth(int nDepth){m_nSearchDepth=nDepth;};	   //�趨����������
	virtual void SetEveluator(CEveluation* pEval){m_pEval=pEval;};	   //�趨��������
	virtual void SetMoveGenerator(CMoveGenerator* pMG){m_pMG=pMG;};	   //�趨�߷�������
	void SetThinkProgress(CProgressCtrl* pThinkProgress){m_pThinkProgress=pThinkProgress;};
																   //�趨��ʾ˼�����ȵĽ�����
protected:
	virtual BYTE MakeMove(STONEMOVE* move,int type);//�˺������ڸ���ĳһ�߷���������֮�������
	virtual void UnMakeMove(STONEMOVE* move);		//�˺������ڻָ�ĳһ�߷�����������Ϊ�߹�֮ǰ��
	virtual int IsGameOver(BYTE position[GRID_NUM][GRID_NUM],int nDepth);
													//�˺��������жϵ�ǰ�����Ƿ��ѷֳ�ʤ��

protected:
	CProgressCtrl* m_pThinkProgress;	 //������ʾ˼�����ȵĽ�����ָ��
	BYTE CurPosition[GRID_NUM][GRID_NUM];//����ʱ���ڵ�ǰ�ڵ�����״̬������
	STONEMOVE m_cmBestMove;				 //��¼����߷��ı���	
	CMoveGenerator* m_pMG;				 //�߷�������ָ��	
	CEveluation* m_pEval;				 //��ֵ����ָ��	
	int m_nSearchDepth;					 //����������
	int m_nMaxDepth;					 //��ǰ����������������
};

#endif // !defined(AFX_SEARCHENGINE_H__0CFB7AC4_C204_470F_86D2_4C586642DEB8__INCLUDED_)
