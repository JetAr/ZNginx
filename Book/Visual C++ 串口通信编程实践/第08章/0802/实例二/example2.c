//这是一个单片机C51串口接收（中断）和发送例程，可以用来测试51单片机的中断接收 
//和查询发送，另外我觉得发送没有必要用中断，因为程序的开销是一样的 
#include <reg51.h>
#include <string.h>

#define INBUF_LEN 4   //数据长度

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

//向串口发送一个字符 
void send_char_com(unsigned char ch)  
{
    SBUF=ch;
    while(TI==0);
    TI=0;
}

//向串口发送一个字符串，strlen为该字符串长度 
void send_string_com(unsigned char *str,unsigned int strlen)
{
    unsigned int k=0;
    do 
    {
        send_char_com(*(str + k));
        k++;
    } while(k < strlen);
}


//串口接收中断函数 
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
                read_flag=1;  //如果串口接收的数据达到INBUF_LEN个， 
                            //且校验没错，就置位取数标志 
             }
        }
    }
}

//主程序
main()
{
    init_serialcomm();  //初始化串口 
    while(1)
    {
          if(read_flag)  //如果取数标志已置位，就将读到的数从串口发出 
          {
               read_flag=0; //取数标志清0 
               send_string_com(inbuf1,INBUF_LEN);
          }
    }

}
