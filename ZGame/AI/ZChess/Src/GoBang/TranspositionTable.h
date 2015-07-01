// TranspositionTable.h: interface for the CTranspositionTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSPOSITIONTABLE_H__C6292E32_89E6_46D7_8A85_E335AD8C4BBA__INCLUDED_)
#define AFX_TRANSPOSITIONTABLE_H__C6292E32_89E6_46D7_8A85_E335AD8C4BBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"

//������ö���͵��������ͣ���ȷ���±߽磬�ϱ߽�
enum ENTRY_TYPE{exact,lower_bound,upper_bound};

//��ϣ����Ԫ�صĽṹ����
typedef struct HASHITEM
{
	LONGLONG checksum;	  //64λУ����
	ENTRY_TYPE entry_type;//��������
	short depth;		  //ȡ�ô�ֵʱ�Ĳ��
	short eval;			  //�ڵ��ֵ
}HashItem;

//�û�����
class CTranspositionTable  
{
public:
	CTranspositionTable();
	virtual ~CTranspositionTable();

public:
	void EnterHashTable(ENTRY_TYPE entry_type, short eval, short depth,int TableNo);
																     //����ǰ�ڵ��ֵ�����ϣ��
	int LookUpHashTable(int alpha, int beta, int depth,int TableNo); //��ѯ��ϣ���е�ǰ�ڵ�����
	void Hash_UnMakeMove(STONEMOVE* move,BYTE CurPosition[][GRID_NUM]);  
																	 //���������߷��Ĺ�ϣֵ����ԭ���߹�֮ǰ��
	void Hash_MakeMove(STONEMOVE* move,BYTE CurPosition[][GRID_NUM]);//���������߷������������µĹ�ϣֵ
	void CalculateInitHashKey(BYTE CurPosition[][GRID_NUM]);		 //����������̵Ĺ�ϣֵ
	void InitializeHashKey();										 //��ʼ��������飬������ϣ��

public:	
	UINT m_nHashKey32[15][10][9];	   //32λ������飬��������32λ��ϣֵ
	ULONGLONG m_ulHashKey64[15][10][9];//64λ������飬��������64λ��ϣֵ
	HashItem *m_pTT[10];			   //�û���ͷָ��
	UINT m_HashKey32;				   //32λ��ϣֵ
	LONGLONG m_HashKey64;			   //64 λ��ϣֵ
};

#endif // !defined(AFX_TRANSPOSITIONTABLE_H__C6292E32_89E6_46D7_8A85_E335AD8C4BBA__INCLUDED_)
