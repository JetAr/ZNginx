// TranspositionTable.cpp: implementation of the CTranspositionTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TranspositionTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//����64λ�����
LONGLONG rand64(void)
{
    return rand()^((LONGLONG)rand()<<15)^((LONGLONG)rand()<<30)^
		((LONGLONG)rand()<<45)^((LONGLONG)rand()<<60);
}

//����32λ�����
LONG rand32(void)
{
    return rand()^((LONG)rand()<<15)^((LONG)rand()<<30);
}

CTranspositionTable::CTranspositionTable()
{
	InitializeHashKey();//������ϣ�������������
}

CTranspositionTable::~CTranspositionTable()
{
	//�ͷŹ�ϣ�����ÿռ�
	delete m_pTT[0];
	delete m_pTT[1];
}

void CTranspositionTable::CalculateInitHashKey(BYTE CurPosition[][GRID_NUM])
{
	int j,k,nStoneType;

	m_HashKey32=0;
	m_HashKey32=0;
	//���������Ӷ�Ӧ����ϣ������
	for(j=0;j<GRID_NUM;j++)
		for(k=0;k<GRID_NUM;k++)
		{
			nStoneType=CurPosition[j][k];
			if(nStoneType!=NOSTONE)
			{
				m_HashKey32=m_HashKey32^m_nHashKey32[nStoneType][j][k]; 
				m_HashKey64=m_HashKey64^m_ulHashKey64[nStoneType][j][k]; 
			}
		}
}

void CTranspositionTable::Hash_MakeMove(STONEMOVE *move,BYTE CurPosition[][GRID_NUM])
{
	int type;

	type=CurPosition[move->StonePos.y][move->StonePos.x];//��������Ŀ��λ�õ����������
	m_HashKey32=m_HashKey32^m_nHashKey32[type][move->StonePos.y][move->StonePos.x];
	m_HashKey64=m_HashKey64^m_ulHashKey64[type][move->StonePos.y][move->StonePos.x];
}

void CTranspositionTable::Hash_UnMakeMove(STONEMOVE *move,BYTE CurPosition[][GRID_NUM])
{
	int type;
	
	type=CurPosition[move->StonePos.y][move->StonePos.x];//����������λ���ϵ�������ӹ�ϣֵ����ȥ��
	m_HashKey32=m_HashKey32^m_nHashKey32[type][move->StonePos.y][move->StonePos.x];
	m_HashKey64=m_HashKey64^m_ulHashKey64[type][move->StonePos.y][move->StonePos.x];
}

int CTranspositionTable::LookUpHashTable(int alpha, int beta, int depth, int TableNo)
{
	int x;
	HashItem* pht;

	//�����ʮλ��ϣ��ַ����������趨�Ĺ�ϣ���С���� 1M*2 ��,
	//���� TableSize*2��TableSizeΪ�����趨�Ĵ�С
	//����Ҫ�޸���һ��Ϊm_HashKey32% TableSize
	//��һ����������һ��Ҳһ��
	x=m_HashKey32 & 0xFFFFF;
	pht=&m_pTT[TableNo][x];//ȡ������ı���ָ��

    if(pht->depth>=depth && pht->checksum==m_HashKey64)
	{
		switch(pht->entry_type)//�ж���������
		{
		case exact://ȷ��ֵ
			return pht->eval;

		case lower_bound://�±߽�
			if(pht->eval>=beta)
				return pht->eval;
			else 
				break;

		case upper_bound://�ϱ߽�
			if (pht->eval<=alpha)
				return pht->eval;
			else 
				break;
        }
	}

	return 66666;
}

void CTranspositionTable::EnterHashTable(ENTRY_TYPE entry_type, short eval, short depth, int TableNo)
{
	int x;
	HashItem* pht;

	x=m_HashKey32 & 0xFFFFF;//�����ʮλ��ϣ��ַ
	pht=&m_pTT[TableNo][x]; //ȡ������ı���ָ��

	//������д���ϣ��
	pht->checksum=m_HashKey64; //64λУ����
	pht->entry_type=entry_type;//��������
	pht->eval=eval;			   //Ҫ�����ֵ
	pht->depth=depth;		   //���
}

void CTranspositionTable::InitializeHashKey()
{
	int i,j,k;
	
	srand((unsigned)time(NULL));
	
	//����������
	for(i=0;i<15;i++)
		for(j=0;j<10;j++)
			for(k=0;k<9;k++)
			{
				m_nHashKey32[i][j][k]=rand32();
				m_ulHashKey64[i][j][k]=rand64();
			}

	//�����û������ÿռ䡣1M "2 ����Ŀ������Ҳ��ָ��������С
	m_pTT[0]=new HashItem[1024*1024];//���ڴ��ȡ����ֵ�Ľڵ�����
	m_pTT[1]=new HashItem[1024*1024];//���ڴ��ȡ��Сֵ�Ľڵ�����
}
