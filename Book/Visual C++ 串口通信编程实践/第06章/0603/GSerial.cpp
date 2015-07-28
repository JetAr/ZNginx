
/*------------------------------------------------------------------*
  GSERIAL.CPP

  For asynchronous serial port communications
  适用于DOS环境下异步串口通信编程

    ATTENTION: Compile this program with Test Stack Overflow OFF.
在Turbo C++3.0中选项设置 Options/Compiler/Entry中关闭Test Stack Overflow

*------------------------------------------------------------------*/

#include "GSerial.h"


char  inbuf[IBUF_LEN];		// in buffer
char  outbuf[OBUF_LEN];		// out buffer

unsigned int   startbuf = 0;
unsigned int   endbuf  = 0;
unsigned int   inhead  = 0;
unsigned int   intail   = 0;
unsigned int   outhead  = 0;
unsigned int   outtail  = 0;
unsigned int   PortBase = 0;



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
    do
    {
        SendChar(*(unChBuf + k));
        k++;
    }
    while ((k < nStrlen));
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


void interrupt ComIntServ(...)
{
    int temp;
    disable();
    temp = (inportb(PortBase+IIR)) & IIR_MASK;				// why interrupt was called
    switch(temp)
    {
    case 0x00:  // modem status changed
        inportb(PortBase+MSR);   // read in useless char
        break;
    case 0x02:  // Request To Send char
        if (outhead != outtail)							// there's a char to send
        {
            outportb(PortBase+TXR,outbuf[outhead++]);	// send the character
            if (outhead == OBUF_LEN)
                outhead=0;								// if at end of buffer, reset pointer
        }
        break;
    case 0x04:  // character ready to be read in
        //inbuf[inhead++] = inportb(m_unPortBase+RXR);// read character into inbuffer
        inbuf[inhead] = inportb(PortBase+RXR);// read character into inbuffer
        inhead++;
        if (inhead == IBUF_LEN) // if at end of buffer
            inhead=0;           // reset pointer
        break;
    case 0x06:  // line status has changed
        inportb(PortBase+LSR);     // read in useless char
        break;
    default:
        break;

    }

    outportb(PIC8259_ICR, PIC8259_EOI);	// Signal end of hardware interrupt
    enable();							// reenable interrupts at the end of the handler

}

// Returns either the character to be received from modem if there is one
// waiting in the buffer, or returns a 0 if there is no character waiting.
char ReadChar(void)
{
    char ch;
    if (inhead != intail)     // there is a character
    {
        disable();                          // disable irqs while getting char
        ch = inbuf[intail++];               // get character from buffer
        if (intail == IBUF_LEN)				// if at end of in buffer
            intail=0;						// reset pointer
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
    int        port     = COM1;
    int        speed    = 9600;
    int        parity   = LCR_NO_PARITY;
    int        bits     = 8;
    int        stopbits = 1;
    int        done  = FALSE;
    char       c;
    int temp;
    int SError=0;

    GSerial gs;

    if (!gs.InitSerialPort(port, speed, parity, bits, stopbits))
    {
        PortBase = PortBaseAddr[port-1];
        gs.SetVects(ComIntServ);
        gs.CommOn();
    }
    else
        SError=2;

    fprintf(stdout, "\nCOM%d, PortBase=0X%x, IntVect=0X%x\n\n",gs.m_unPortNo,gs.m_unPortBase,ComIRQ[gs.m_unPortNo-1]);


    fprintf(stdout, "TURBO C TERMINAL\n"
            "...You're now in terminal mode, "
            "press [ESC] to quit...\n\n");

    /*
       The main loop a MSR_CTS as a dumb terminal. We repeatedly
       check the keyboard buffer, and communications buffer.
    */
    do
    {
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
                gs.SendChar( c );
                fprintf(stdout,"%c",c);
            }
        }
        c = ReadChar();
        if (c != -1)     //'-1' is the END signal of a string
        {
            fprintf(stdout,"%c",c);
        }
        //	fprintf(stdout,"%d",testtemp);

    }
    while ((!done) && (!SError));

    gs.CloseSerialPort();

    /* Check for errors */
    switch (SError)
    {
    case NO_ERROR:
        fprintf(stderr, "\nbye.\n");
        return (0);

    case BUF_OVFL:
        fprintf(stderr, "\nBuffer Overflow.\n");
        return (99);
    case 2:
        fprintf(stderr,"\n Cannot init serial port");
        return(2);
    default:
        fprintf(stderr, "\nUnknown Error, SError = %d\n",
                SError);
        return (99);
    }
}


