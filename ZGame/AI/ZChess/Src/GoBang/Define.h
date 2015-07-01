#ifndef define_h_
#define define_h_

#define BOARD_POS_X 0 //���̵�������ߵľ���
#define BOARD_POS_Y 0 //���̵����������ľ���
#define BOARD_WIDTH 32 //�����ϸ��ӵĿ��
#define GRID_NUM    15 //ÿһ��(��)�����̽�����
#define GRID_COUNT  225//�����Ͻ�������
#define BLACK		0  //������0��ʾ
#define WHITE		1  //������1��ʾ
#define NOSTONE 0xFF   //û������

//��������Լ��ĳһ�����Ƿ��������ϵ���Ч���ӵ�
#define IsValidPos(x,y) ((x>=0 && x<GRID_NUM) && (y>=0 && y<GRID_NUM)

//���Ա�ʾ����λ�õĽṹ
typedef struct _stoneposition
{
	BYTE x;
	BYTE y;
}STONEPOS;

//����ṹ���Ա�ʾ�߷�
typedef struct _stonemove
{
	STONEPOS StonePos;//����λ��
	int Score;		  //�߷��ķ���
}STONEMOVE;

#endif