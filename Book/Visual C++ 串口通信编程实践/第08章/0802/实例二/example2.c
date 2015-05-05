//����һ����Ƭ��C51���ڽ��գ��жϣ��ͷ������̣�������������51��Ƭ�����жϽ��� 
//�Ͳ�ѯ���ͣ������Ҿ��÷���û�б�Ҫ���жϣ���Ϊ����Ŀ�����һ���� 
#include <reg51.h>
#include <string.h>

#define INBUF_LEN 4   //���ݳ���

unsigned char inbuf1[INBUF_LEN];
unsigned char checksum,count3;
bit          read_flag=0;

void init_serialcomm(void)
{
    SCON  = 0x50;       //SCON: serail mode 1, 8-bit UART, enable ucvr 
    TMOD |= 0x20;       //TMOD: timer 1, mode 2, 8-bit reload 
    PCON |= 0x80;       //SMOD=1; 
    TH1   = 0xF4;       //Baud:4800  fosc=11.0592MHz 
    IE   |= 0x90;       //Enable Serial Interrupt 
    TR1   = 1;          // timer 1 run 
   // TI=1; 
}

//�򴮿ڷ���һ���ַ� 
void send_char_com(unsigned char ch)  
{
    SBUF=ch;
    while(TI==0);
    TI=0;
}

//�򴮿ڷ���һ���ַ�����strlenΪ���ַ������� 
void send_string_com(unsigned char *str,unsigned int strlen)
{
    unsigned int k=0;
    do 
    {
        send_char_com(*(str + k));
        k++;
    } while(k < strlen);
}


//���ڽ����жϺ��� 
void serial () interrupt 4 using 3 
{
    if(RI)
    {
        unsigned char ch;
        RI = 0;
        ch=SBUF;
        if(ch>127)
        {
             count3=0;
             inbuf1[count3]=ch;
             checksum= ch-128;
        }
        else 
        {
             count3++;
             inbuf1[count3]=ch;
             checksum ^= ch;
             if( (count3==(INBUF_LEN-1)) && (!checksum) )
             {
                read_flag=1;  //������ڽ��յ����ݴﵽINBUF_LEN���� 
                            //��У��û������λȡ����־ 
             }
        }
    }
}

//������
main()
{
    init_serialcomm();  //��ʼ������ 
    while(1)
    {
          if(read_flag)  //���ȡ����־����λ���ͽ����������Ӵ��ڷ��� 
          {
               read_flag=0; //ȡ����־��0 
               send_string_com(inbuf1,INBUF_LEN);
          }
    }

}
