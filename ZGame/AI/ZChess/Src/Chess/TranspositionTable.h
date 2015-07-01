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


// TranspositionTable.h: interface for the CTranspositionTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSPOSITIONTABLE_H__716F8220_CEEA_11D5_AEC7_5254AB2E22C7__INCLUDED_)
#define AFX_TRANSPOSITIONTABLE_H__716F8220_CEEA_11D5_AEC7_5254AB2E22C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ENTRY_TYPE { exact , lower_bound, upper_bound};

typedef struct HASHITEM
{
    LONGLONG checksum;	// or long long might be even better
    ENTRY_TYPE  entry_type;
    short depth;
    short eval;
} HashItem;

class CTranspositionTable
{
public:
    CTranspositionTable();
    virtual ~CTranspositionTable();

    void CalculateInitHashKey(BYTE CurPosition[10][9]);
    void Hash_MakeMove(CHESSMOVE* move,BYTE CurPosition[10][9]);
    void Hash_UnMakeMove(CHESSMOVE* move,BYTE nChessID,BYTE CurPosition[10][9]);

    int LookUpHashTable(int alpha, int beta, int depth,int TableNo);
    void EnterHashTable(ENTRY_TYPE entry_type, short eval, short depth,int TableNo);

    UINT m_nHashKey32[15][10][9];
    ULONGLONG m_ulHashKey64[15][10][9];
    HashItem *m_pTT[10];
    void InitializeHashKey();
    UINT m_HashKey32;
    LONGLONG m_HashKey64;
};

#endif // !defined(AFX_TRANSPOSITIONTABLE_H__716F8220_CEEA_11D5_AEC7_5254AB2E22C7__INCLUDED_)
