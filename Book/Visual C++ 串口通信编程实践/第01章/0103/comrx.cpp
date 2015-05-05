/////////////////////////////////////////////////////////////
//COMRX.CPP  for asyn serial communication (only RX)
//edited by Xiong Guangming and Gong Jianwei 
//Turbo C++3.0
/////////////////////////////////////////////////////////////
#include <stdio.h>
#include <dos.h>
#include <conio.h>

#define BUFFLEN 1024

void InitCOM();  //初始化串口
void OpenPort();  //打开串口
void ClosePort(); //关闭串口，释放串口资源
//新的中断函数，注意在TC2.0下，下面函数的...要去掉
void interrupt far  asyncint(...);
//中断向量：用于保存中断现场
void interrupt(*asyncoldvect)(...);

unsigned char Buffer[BUFFLEN];
int buffin=0;
int buffout=0;
//unsigned char ch;

//COM1产生的硬件中断号为IQR4，对应的中断向量为为0CH
//打开COM1
void OpenPort()
{
	unsigned char ucTemp;
	InitCOM();  //初始化串口

	//读入由参数给定的中断向量值，并将它作为中断函数的远地址
	asyncoldvect=getvect(0x0c);
	disable();       //关中断
	inportb(0x3f8);
	inportb(0x3fe);
	inportb(0x3fb);
	inportb(0x3fa);
	outportb(0x3fc,0x08|0x0b);
	outportb(0x3f9,0x01);
	ucTemp=inportb(0x21)&0xef;
	outportb(0x21,ucTemp);
	setvect(0x0c,asyncint);
	enable();       //开中断
}

//中断服务程序，从COM1接收数据
//注意在TC2.0下，下面函数的...要去掉
void interrupt far asyncint(...)
{
	//unsigned char ch;
	Buffer[buffin++] = inportb(0x3f8);// 读字符到缓冲区
	if (buffin >= BUFFLEN)  // 缓冲区满
		buffin=0;           // 指针复位
	outportb(0x20,0x20);
}

void ClosePort(void) //关闭中断
{
	disable();
	outportb(0x3f9,0x00);
	outportb(0x3fc,0x00);
	outportb(0x21,inportb(0x21)&0x10);
	enable();
	setvect(0x0c,asyncoldvect);
}

void InitCOM()// 对COM1串口初始化，设置串口参数
{

	outportb(0x3fb,0x80);  //将设置波特率
	/* 设置波特率，低位在前、高位在后；(部分波特率器参数如下)
     波特率    分频器H   分频器L
	 ...
	 4800       00        18H
	 7200       00        10H
	 9600       00        0CH
	 ....	 
  */
	outportb(0x3f8,0x0C);   //波特率为9600bps
	outportb(0x3f9,0x00);

	/*设置停止位、奇偶校验位、等
	D7：为1表示设置波特率；为0，其他；
	D6：为1，是，强迫在数据线上输出逻辑0；为0，则否；
	D5：1，校验位可变；0，不变；
	D4D3：×0，无校验位；01，奇校验位；11，偶校验位；
	D2：0,1个停止位；1，1.5个停止位；
	D1D0：00，5位数据位；01，6位数据位；10，7位数据位；11，8位数据位；
	*/
	outportb(0x3fb,0x03);   //8个数据位，1个停止位、无奇偶校验

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

//以下为主函数
void main()
{
	unsigned char unChar;
	short bExit_Flag=0;

	OpenPort(); //打开串口

	fprintf(stdout, "\n\nReady to Receive DATA\n"
			"press [ESC] to quit...\n\n");

	do {
		if (kbhit())
		{
			unChar=getch();
			/* Look for an ESC key */
			switch (unChar)
			{
			case 0x1B:   //ESC的ASCII值为27
				bExit_Flag = 1;  /* Exit program */
				break;
			//You may want to handle other keys here
			}
		}
		unChar = read_char();  //从缓冲区中读数
		if (unChar != 0xff)
		{
			fprintf(stdout,"%c",unChar);
		}
	} while (!bExit_Flag);

	ClosePort(); //关闭串口
}
