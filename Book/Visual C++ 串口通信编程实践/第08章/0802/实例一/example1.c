#include <reg51.h>
#define uchar unsigned char
uchar xdata rt_buf[32];
uchar r_in,t_out;
bit r_full,t_empty;

serial() interrupt4  //串口中断程序
{
	if(RI && r_full)
	{
		rt_buf[r_in]=SBUF;
		RI=0;
	   if(rt_buf[r_in]==0x24)
	   {
		 r_full=1;
		 SBUF=tr_buf[t_out];
		 t_empty=0;
	   }
	   /*接收字符为$，则接收结束；设置接收结束标志，开始发送*/
	   r_in=++r_in;
	}
	else if(TI && t_empty)
	{
		TI=0;
		t_out=++t_out;
		SBUF=rt_buf[t_out];
		if(t_out==r_in)
			t_empty=1;
		/*t_out=r_in则发送完，设发送完标志t_empty*/
	} 
}

main()
{
	uchar a:
/*设置定时器T1工作于方式2，计数常数为0xfdH */
    TMOD=0x20;
	TL1=0xfd;
	TH1=0xfd;
	SCON=0x50;//在11.0592MHz下，设置串行口波特率为9600，方式1
	PCON=0xD0;
	IE=0x10;
	TR1=1;
	EA=1;
	r_in=t_out=0;
	t_empty=1;
	r_full=0;
	for(;;)
	{
　　　　　//相应的任务处理功能函数
	}
}
