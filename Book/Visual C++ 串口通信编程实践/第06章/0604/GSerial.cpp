
  /*------------------------------------------------------------------*
	  GSERIAL.CPP 
	  Edited by Gong jianwei  http://www.gjwtech.com
	  For asynchronous serial port communications
	  适用于DOS环境下异步串口通信编程 多串口控制程序 
      ATTENTION: Compile this program with Test Stack Overflow OFF.
	在Turbo C++3.0中选项设置 Options/Compiler/Entry中关闭Test Stack Overflow

  *------------------------------------------------------------------*/

#include "GSerial.h"

struct COMPORT_VAR{
	char  inbuf[IBUF_LEN];		// in buffer
	char  outbuf[OBUF_LEN];		// out buffer

	unsigned int   startbuf ;
	unsigned int   endbuf  ;
	unsigned int   inhead  ;
	unsigned int   intail   ;
	unsigned int   outhead  ;
	unsigned int   outtail  ;
	unsigned int   PortBase ;
};

COMPORT_VAR comport1,comport2;




//////////////////////////////////GSerial////////
GSerial::GSerial()
{
}

GSerial::~GSerial()
{
}

//*  get status of the port  */
int GSerial::ReadStatus(void)
{
  return(inp(m_unPortBase+5));
}

/*  send one valid char from the port  */
void GSerial::SendChar(unsigned char unCh)
{
   while ((ReadStatus() & 0x40) == 0);
   outportb(m_unPortBase,unCh);
}

/*  send one string from the port  */
void GSerial::SendString(int nStrlen, unsigned char *unChBuf)
{
	int k=0;	
	do {
		SendChar(*(unChBuf + k));	
		k++;
	} while ((k < nStrlen));
}


/* Install our functions to handle communications */
void GSerial::SetVects(void interrupt(*New_Int)(...))
{
	disable();
	OldVects = getvect(InterruptNo[m_unPortNo-1]);
	setvect(InterruptNo[m_unPortNo-1], New_Int);
	enable();
}

/* Uninstall our vectors before exiting the program */
void GSerial::ResetVects(void)
{
	setvect(InterruptNo[m_unPortNo-1], OldVects);
}



/* Tell modem that we're ready to go */
void GSerial::CommOn(void)
{
	int temp;
	disable();
	//temp = inportb(m_unPortBase + MCR) | MCR_INT;
	//outportb(m_unPortBase + MCR, temp);
	outportb(m_unPortBase + MCR, MCR_INT);
	//temp = inportb(m_unPortBase + MCR) | MCR_DTR | MCR_RTS;
	//outportb(m_unPortBase + MCR, temp);
	temp = (inportb(m_unPortBase + IER)) | IER_RX_INT;//|IER_TX_INT;
	outportb(m_unPortBase + IER, temp);
	temp = inportb(PIC8259_IMR) & ComIRQ[m_unPortNo-1];
	outportb(PIC8259_IMR, temp);
	enable();
}

/* Go off-line */
void GSerial::CommOff(void)
{
	 int  temp;

	disable();
	temp = inportb(PIC8259_IMR) | ~ComIRQ[m_unPortNo-1];
	outportb(PIC8259_IMR, temp);
	outportb(m_unPortBase + IER, 0);
	outportb(m_unPortBase + MCR, 0);
	enable();
}



/* Set the port number to use */
int GSerial::SetPortBaseAddr(int Port)
{
	if((Port<1)||(Port>6))
		return(-1);
	m_unPortNo = Port;
	m_unPortBase = PortBaseAddr[m_unPortNo-1];
	return (0);
}

/* This routine sets the speed; will accept funny baud rates. */
/* Setting the speed requires that the DLAB be set on.        */
int GSerial::SetSpeed(int Speed)
{
	char	c;
	int		divisor;

	if (Speed == 0)            /* Avoid divide by zero */
		return (-1);
	else
		divisor = (int) (115200L/Speed);

	if (m_unPortBase == 0)
		return (-1);

	disable();
	c = inportb(m_unPortBase + LCR);
	outportb(m_unPortBase + LCR, (c | 0x80)); /* Set DLAB */
	outportb(m_unPortBase + DLL, (divisor & 0x00FF));
	outportb(m_unPortBase + DLH, ((divisor >> 8) & 0x00FF));
	outportb(m_unPortBase + LCR, c);          /* Reset DLAB */
	enable();

	return (0);
}

/* Set other DATA Format communications parameters */
int GSerial::SetDataFormat(int Parity, int Bits, int StopBit)
{
	int  setting;

	if (m_unPortBase == 0)
		return (-1);
	if (Bits < 5 || Bits > 8)
		return (-1);
	if (StopBit != 1 && StopBit != 2)
		return (-1);
	if (Parity != LCR_NO_PARITY && Parity != LCR_ODD_PARITY && Parity != LCR_EVEN_PARITY)
		return (-1);

	setting  = Bits-5;
	setting |= ((StopBit == 1) ? 0x00 : 0x04);
	setting |= Parity;

	disable();
	outportb(m_unPortBase + LCR, setting);
	enable();

	return (0);
}


void GSerial::CloseSerialPort(void)
{
	CommOff();
	ResetVects();
}

/* Set up the port */
int GSerial::InitSerialPort(int Port, int Speed, int Parity, int Bits, int StopBit)
{
	int flag = 0;
	if (SetPortBaseAddr(Port))
	  flag = -1;
	if (SetSpeed(Speed))
	  flag = -1;
	if (SetDataFormat(Parity, Bits, StopBit))
	  flag = -1;
	return(flag);
}



////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////COM1//////////////////////////////////////////////////////////
void interrupt ComIntServ_comport1(...)
{
	int temp;
	disable();
	temp = (inportb(comport1.PortBase+IIR)) & IIR_MASK;				// why interrupt was called
	switch(temp)
	{
		case 0x00:  // modem status changed
			inportb(comport1.PortBase+MSR);   // read in useless char
			break;
		case 0x02:  // Request To Send char
			if (comport1.outhead != comport1.outtail)							// there's a char to send
			{
				outportb(comport1.PortBase+TXR,comport1.outbuf[comport1.outhead++]);	// send the character
				if (comport1.outhead == OBUF_LEN)
					comport1.outhead=0;								// if at end of buffer, reset pointer
			}
			break;
		case 0x04:  // character ready to be read in
			//inbuf[inhead++] = inportb(m_unPortBase+RXR);// read character into inbuffer
			comport1.inbuf[comport1.inhead] = inportb(comport1.PortBase+RXR);// read character into inbuffer
			comport1.inhead++;
			if (comport1.inhead == IBUF_LEN) // if at end of buffer
				comport1.inhead=0;           // reset pointer
			break;
		case 0x06:  // line status has changed
			inportb(comport1.PortBase+LSR);     // read in useless char
			break;
		default:
			break;

	}

	outportb(PIC8259_ICR, PIC8259_EOI);	// Signal end of hardware interrupt
	enable();							// reenable interrupts at the end of the handler

}


//COM1 串口1
char ReadChar_comport1(void)
{
	char ch;
	if (comport1.inhead != comport1.intail)     // there is a character
	{
		disable();                          // disable irqs while getting char
		ch = comport1.inbuf[comport1.intail++];               // get character from buffer
		if (comport1.intail == IBUF_LEN)				// if at end of in buffer
			comport1.intail=0;						// reset pointer
		enable();                           // re-enable interrupt
		return(ch);                      // return the char
	}
	ch = -1;
	return(ch);                          // return nothing
}


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////COM2//////////////////////////////////////////////////////////

void interrupt ComIntServ_comport2(...)
{
	int temp;
	disable();
	temp = (inportb(comport2.PortBase+IIR)) & IIR_MASK;				// why interrupt was called
	switch(temp)
	{
		case 0x00:  // modem status changed
			inportb(comport2.PortBase+MSR);   // read in useless char
			break;
		case 0x02:  // Request To Send char
			if (comport2.outhead != comport2.outtail)							// there's a char to send
			{
				outportb(comport2.PortBase+TXR,comport2.outbuf[comport2.outhead++]);	// send the character
				if (comport2.outhead == OBUF_LEN)
					comport2.outhead=0;								// if at end of buffer, reset pointer
			}
			break;
		case 0x04:  // character ready to be read in
			//inbuf[inhead++] = inportb(m_unPortBase+RXR);// read character into inbuffer
			comport2.inbuf[comport2.inhead] = inportb(comport2.PortBase+RXR);// read character into inbuffer
			comport2.inhead++;
			if (comport2.inhead == IBUF_LEN) // if at end of buffer
				comport2.inhead=0;           // reset pointer
			break;
		case 0x06:  // line status has changed
			inportb(comport2.PortBase+LSR);     // read in useless char
			break;
		default:
			break;

	}

	outportb(PIC8259_ICR, PIC8259_EOI);	// Signal end of hardware interrupt
	enable();							// reenable interrupts at the end of the handler

}


//COM2 串口2
char ReadChar_comport2(void)
{
	char ch;
	if (comport2.inhead != comport2.intail)     // there is a character
	{
		disable();                          // disable irqs while getting char
		ch = comport2.inbuf[comport2.intail++];               // get character from buffer
		if (comport2.intail == IBUF_LEN)				// if at end of in buffer
			comport2.intail=0;						// reset pointer
		enable();                           // re-enable interrupt
		return(ch);                      // return the char
	}
	ch = -1;
	return(ch);                          // return nothing
}

///////
main()
{
	/* Communications parameters */

	comport1.startbuf =0;
	comport1.endbuf   =0;
	comport1.inhead   =0;
	comport1.intail   =0;
	comport1.outhead  =0;
	comport1.outtail  =0;
	comport1.PortBase =0;

	comport2.startbuf =0;
	comport2.endbuf   =0;
	comport2.inhead   =0;
	comport2.intail   =0;
	comport2.outhead  =0;
	comport2.outtail  =0;
	comport2.PortBase =0;


	int   port     = COM1;
	int   speed    = 9600;
	int   parity   = LCR_NO_PARITY;
	int   bits     = 8;
	int   stopbits = 1;
	int   done  = FALSE;
	char  c;
	int  temp;
	int  SError=0;

	GSerial gsCOM1, gsCOM2;  //定义两个GSerial对象

	//初始化COM1，串口1
	if (!gsCOM1.InitSerialPort(port, speed, parity, bits, stopbits))
	{
		comport1.PortBase = PortBaseAddr[port-1];
		gsCOM1.SetVects(ComIntServ_comport1);
		gsCOM1.CommOn();
	}
	else
	   SError=2;

	//初始化COM2，串口2
	port  =  COM2;
	if (!gsCOM2.InitSerialPort(port, speed, parity, bits, stopbits))
	{
		comport2.PortBase = PortBaseAddr[port-1];
		gsCOM2.SetVects(ComIntServ_comport2);
		gsCOM2.CommOn();
	}
	else
	   SError=2;


	//打印串口地址及中断向量地址
	fprintf(stdout, "\nCOM%d, PortBase=0X%x, IntVect=0X%x\n\n",
		gsCOM1.m_unPortNo,gsCOM1.m_unPortBase,ComIRQ[gsCOM1.m_unPortNo-1]);

	fprintf(stdout, "\nCOM%d, PortBase=0X%x, IntVect=0X%x\n\n",
		gsCOM2.m_unPortNo,gsCOM2.m_unPortBase,ComIRQ[gsCOM2.m_unPortNo-1]);

	fprintf(stdout, "Now we are ready to go: \n\n");



	do {
		if (kbhit())
		{
			c = getch();
			/* Look for an Escape key */
			switch (c)
			{
				case ESC:
					done = TRUE;  /* Exit program */
					break;
			}
			if (!done)
			{
			   gsCOM1.SendChar( c );
			   fprintf(stdout,"\n\n[COM1:TX]: %c\n",c);
			}
		}
		c = ReadChar_comport1();
		if (c != -1)     //'-1' is the END signal of a string
		{
			fprintf(stdout,"[COM1:RX]: %c\n",c);
		}

		delay(50);

		c = ReadChar_comport2();
		if (c != -1)     //'-1' is the END signal of a string
		{
			fprintf(stdout,"[COM2:RX]:  %c\n",c);
			gsCOM2.SendChar( c );
			fprintf(stdout,"[COM2:TX]:  %c\n",c);
		}

	} while ((!done) && (!SError));

	//关闭打开的串口
	gsCOM1.CloseSerialPort();
	gsCOM2.CloseSerialPort();

	/* Check for errors */
	switch (SError)
	{
		case NO_ERROR: fprintf(stderr, "\nbye.\n");
					  return (0);

		case BUF_OVFL: fprintf(stderr, "\nBuffer Overflow.\n");
					  return (99);
		case 2:   fprintf(stderr,"\n Cannot init serial port");
					   return(2);
		default:      fprintf(stderr, "\nUnknown Error, SError = %d\n",
							  SError);
					  return (99);
	}
}


