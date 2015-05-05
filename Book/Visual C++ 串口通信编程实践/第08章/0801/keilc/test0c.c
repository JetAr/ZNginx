#include <reg51.h>
#include <stdio.h>

void main(void)
{
    SCON = 0x50; //´®¿Ú·½Ê½1,ÔÊÐí½ÓÊÕ
    TMOD = 0x20; //¶¨Ê±Æ÷1¶¨Ê±·½Ê½ 2
    TCON = 0x40; //Éè¶¨¶¨Ê±Æ÷¿ªÊ¼¼ÆÊý   Æ Ê 
    TH1 = 0xE8;  //11.0592MHz 1200²¨ÌØÂÊ
    TL1 = 0xE8;
    TI = 1;
    TR1 = 1;     //Æô¶¯¶¨Ê±Æ÷ 
    
    while(1)
    {
        printf ("Hello World!\n"); //ÏÔÊ¾Hello World
    }
}
