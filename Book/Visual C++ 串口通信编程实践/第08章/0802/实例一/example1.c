#include <reg51.h>
#define uchar unsigned char
uchar xdata rt_buf[32];
uchar r_in,t_out;
bit r_full,t_empty;

serial() interrupt4  //�����жϳ���
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
	   /*�����ַ�Ϊ$������ս��������ý��ս�����־����ʼ����*/
	   r_in=++r_in;
	}
	else if(TI && t_empty)
	{
		TI=0;
		t_out=++t_out;
		SBUF=rt_buf[t_out];
		if(t_out==r_in)
			t_empty=1;
		/*t_out=r_in�����꣬�跢�����־t_empty*/
	} 
}

main()
{
	uchar a:
/*���ö�ʱ��T1�����ڷ�ʽ2����������Ϊ0xfdH */
    TMOD=0x20;
	TL1=0xfd;
	TH1=0xfd;
	SCON=0x50;//��11.0592MHz�£����ô��пڲ�����Ϊ9600����ʽ1
	PCON=0xD0;
	IE=0x10;
	TR1=1;
	EA=1;
	r_in=t_out=0;
	t_empty=1;
	r_full=0;
	for(;;)
	{
����������//��Ӧ���������ܺ���
	}
}
