#include <reg51.h>
#include <stdio.h>

void main(void)
{
    SCON = 0x50; //���ڷ�ʽ1,�������
    TMOD = 0x20; //��ʱ��1��ʱ��ʽ 2
    TCON = 0x40; //�趨��ʱ����ʼ����   � � 
    TH1 = 0xE8;  //11.0592MHz 1200������
    TL1 = 0xE8;
    TI = 1;
    TR1 = 1;     //������ʱ�� 
    
    while(1)
    {
        printf ("Hello World!\n"); //��ʾHello World
    }
}
