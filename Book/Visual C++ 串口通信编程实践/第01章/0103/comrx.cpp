/////////////////////////////////////////////////////////////
//COMRX.CPP  for asyn serial communication (only RX)
//edited by Xiong Guangming and Gong Jianwei 
//Turbo C++3.0
/////////////////////////////////////////////////////////////
#include <stdio.h>
#include <dos.h>
#include <conio.h>

#define BUFFLEN 1024

void InitCOM();  //��ʼ������
void OpenPort();  //�򿪴���
void ClosePort(); //�رմ��ڣ��ͷŴ�����Դ
//�µ��жϺ�����ע����TC2.0�£����溯����...Ҫȥ��
void interrupt far  asyncint(...);
//�ж����������ڱ����ж��ֳ�
void interrupt(*asyncoldvect)(...);

unsigned char Buffer[BUFFLEN];
int buffin=0;
int buffout=0;
//unsigned char ch;

//COM1������Ӳ���жϺ�ΪIQR4����Ӧ���ж�����ΪΪ0CH
//��COM1
void OpenPort()
{
	unsigned char ucTemp;
	InitCOM();  //��ʼ������

	//�����ɲ����������ж�����ֵ����������Ϊ�жϺ�����Զ��ַ
	asyncoldvect=getvect(0x0c);
	disable();       //���ж�
	inportb(0x3f8);
	inportb(0x3fe);
	inportb(0x3fb);
	inportb(0x3fa);
	outportb(0x3fc,0x08|0x0b);
	outportb(0x3f9,0x01);
	ucTemp=inportb(0x21)&0xef;
	outportb(0x21,ucTemp);
	setvect(0x0c,asyncint);
	enable();       //���ж�
}

//�жϷ�����򣬴�COM1��������
//ע����TC2.0�£����溯����...Ҫȥ��
void interrupt far asyncint(...)
{
	//unsigned char ch;
	Buffer[buffin++] = inportb(0x3f8);// ���ַ���������
	if (buffin >= BUFFLEN)  // ��������
		buffin=0;           // ָ�븴λ
	outportb(0x20,0x20);
}

void ClosePort(void) //�ر��ж�
{
	disable();
	outportb(0x3f9,0x00);
	outportb(0x3fc,0x00);
	outportb(0x21,inportb(0x21)&0x10);
	enable();
	setvect(0x0c,asyncoldvect);
}

void InitCOM()// ��COM1���ڳ�ʼ�������ô��ڲ���
{

	outportb(0x3fb,0x80);  //�����ò�����
	/* ���ò����ʣ���λ��ǰ����λ�ں�(���ֲ���������������)
     ������    ��Ƶ��H   ��Ƶ��L
	 ...
	 4800       00        18H
	 7200       00        10H
	 9600       00        0CH
	 ....	 
  */
	outportb(0x3f8,0x0C);   //������Ϊ9600bps
	outportb(0x3f9,0x00);

	/*����ֹͣλ����żУ��λ����
	D7��Ϊ1��ʾ���ò����ʣ�Ϊ0��������
	D6��Ϊ1���ǣ�ǿ����������������߼�0��Ϊ0�����
	D5��1��У��λ�ɱ䣻0�����䣻
	D4D3����0����У��λ��01����У��λ��11��żУ��λ��
	D2��0,1��ֹͣλ��1��1.5��ֹͣλ��
	D1D0��00��5λ����λ��01��6λ����λ��10��7λ����λ��11��8λ����λ��
	*/
	outportb(0x3fb,0x03);   //8������λ��1��ֹͣλ������żУ��

	outportb(0x3fc,0x08|0x0b);
	outportb(0x3f9,0x01);
}


unsigned char read_char(void)
{
	unsigned unch;
	if(buffout != buffin)
	{
		unch = Buffer[buffout];
		buffout++;
		if(buffout >= BUFFLEN)
			buffout=0;
		return(unch);
	}
	else
		return(0xff);
}

//����Ϊ������
void main()
{
	unsigned char unChar;
	short bExit_Flag=0;

	OpenPort(); //�򿪴���

	fprintf(stdout, "\n\nReady to Receive DATA\n"
			"press [ESC] to quit...\n\n");

	do {
		if (kbhit())
		{
			unChar=getch();
			/* Look for an ESC key */
			switch (unChar)
			{
			case 0x1B:   //ESC��ASCIIֵΪ27
				bExit_Flag = 1;  /* Exit program */
				break;
			//You may want to handle other keys here
			}
		}
		unChar = read_char();  //�ӻ������ж���
		if (unChar != 0xff)
		{
			fprintf(stdout,"%c",unChar);
		}
	} while (!bExit_Flag);

	ClosePort(); //�رմ���
}
