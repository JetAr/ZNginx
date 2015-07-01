// TranspositionTable.cpp: implementation of the CTranspositionTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TranspositionTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//生成64位随机数
LONGLONG rand64(void)
{
    return rand()^((LONGLONG)rand()<<15)^((LONGLONG)rand()<<30)^
		((LONGLONG)rand()<<45)^((LONGLONG)rand()<<60);
}

//生成32位随机数
LONG rand32(void)
{
    return rand()^((LONG)rand()<<15)^((LONG)rand()<<30);
}

CTranspositionTable::CTranspositionTable()
{
	InitializeHashKey();//建立哈希表，创建随机数组
}

CTranspositionTable::~CTranspositionTable()
{
	//释放哈希表所用空间
	delete m_pTT[0];
	delete m_pTT[1];
}

void CTranspositionTable::CalculateInitHashKey(BYTE CurPosition[][GRID_NUM])
{
	int j,k,nStoneType;

	m_HashKey32=0;
	m_HashKey32=0;
	//将所有棋子对应的随希数加总
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

	type=CurPosition[move->StonePos.y][move->StonePos.x];//将棋子在目标位置的随机数添入
	m_HashKey32=m_HashKey32^m_nHashKey32[type][move->StonePos.y][move->StonePos.x];
	m_HashKey64=m_HashKey64^m_ulHashKey64[type][move->StonePos.y][move->StonePos.x];
}

void CTranspositionTable::Hash_UnMakeMove(STONEMOVE *move,BYTE CurPosition[][GRID_NUM])
{
	int type;
	
	type=CurPosition[move->StonePos.y][move->StonePos.x];//将棋子现在位置上的随机数从哈希值当中去除
	m_HashKey32=m_HashKey32^m_nHashKey32[type][move->StonePos.y][move->StonePos.x];
	m_HashKey64=m_HashKey64^m_ulHashKey64[type][move->StonePos.y][move->StonePos.x];
}

int CTranspositionTable::LookUpHashTable(int alpha, int beta, int depth, int TableNo)
{
	int x;
	HashItem* pht;

	//计算二十位哈希地址，如果读者设定的哈希表大小不是 1M*2 的,
	//而是 TableSize*2，TableSize为读者设定的大小
	//则需要修改这一句为m_HashKey32% TableSize
	//下一个函数中这一句也一样
	x=m_HashKey32 & 0xFFFFF;
	pht=&m_pTT[TableNo][x];//取到具体的表项指针

    if(pht->depth>=depth && pht->checksum==m_HashKey64)
	{
		switch(pht->entry_type)//判断数据类型
		{
		case exact://确切值
			return pht->eval;

		case lower_bound://下边界
			if(pht->eval>=beta)
				return pht->eval;
			else 
				break;

		case upper_bound://上边界
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

	x=m_HashKey32 & 0xFFFFF;//计算二十位哈希地址
	pht=&m_pTT[TableNo][x]; //取到具体的表项指针

	//将数据写入哈希表
	pht->checksum=m_HashKey64; //64位校验码
	pht->entry_type=entry_type;//表项类型
	pht->eval=eval;			   //要保存的值
	pht->depth=depth;		   //层次
}

void CTranspositionTable::InitializeHashKey()
{
	int i,j,k;
	
	srand((unsigned)time(NULL));
	
	//填充随机数组
	for(i=0;i<15;i++)
		for(j=0;j<10;j++)
			for(k=0;k<9;k++)
			{
				m_nHashKey32[i][j][k]=rand32();
				m_ulHashKey64[i][j][k]=rand64();
			}

	//申请置换表所用空间。1M "2 个条目，读者也可指定其他大小
	m_pTT[0]=new HashItem[1024*1024];//用于存放取极大值的节点数据
	m_pTT[1]=new HashItem[1024*1024];//用于存放取极小值的节点数据
}
