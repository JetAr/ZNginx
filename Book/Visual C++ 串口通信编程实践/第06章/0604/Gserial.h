/*---------------------------------------------------------------------
   FILENAME: GSERIAL.H
   This file is used to define const and class for GSERIAL.CPP
   此文件用于定义常量和serial类的变量
--------------------------------------------------------------------*/

#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>


//Define Serial port Const 定义串口逻辑名常量
#define COM1            1
#define COM2            2
#define COM3            3
#define COM4            4
#define COM5            5
#define COM6            6

#define COM1BASE        0x3F8   /*  Base port address for COM1  */
#define COM2BASE        0x2F8   /*  Base port address for COM2  */
#define COM3BASE        0x3E8   /*  Base port address for COM3  */
#define COM4BASE        0x2E8   /*  Base port address for COM4  */
#define COM5BASE        0x3A8   /*  Base port address for COM5  */
#define COM6BASE        0x2A8   /*  Base port address for COM6  */

/*
    The 8250 UART has 10 registers accessible through 7 port addresses.
    Here are their addresses relative to COM1BASE and COM2BASE. Note
    that the baud rate registers, (DLL) and (DLH) are active only when
    the Divisor-Latch Access-Bit (DLAB除数锁存器访问位) is on. The (DLAB) 
	is bit 7 of the (LCR--Line Control Register线路控制寄存器).

	* TXR Output data to the serial port.
	* RXR Input data from the serial port.
	* LCR Initialize the serial port.
	* IER Controls interrupt generation.
	* IIR Identifies interrupts.
	* MCR Send contorl signals to the modem.
	* LSR Monitor the status of the serial port.
	* MSR Receive status of the modem.
	* DLL Low byte of baud rate divisor.
	* DHH High byte of baud rate divisor.
*/

/*The following is the adress of the registers               DLAB status	*/
#define TXR             0       /*  Transmit register (WRITE)   0			*/
#define RXR             0       /*  Receive register  (READ)	0			*/
#define IER             1       /*  Interrupt Enable			x			*/
#define IIR             2       /*  Interrupt ID				x			*/
#define LCR             3       /*  Line control				x			*/
#define MCR             4       /*  Modem control				x			*/
#define LSR             5       /*  Line Status					x			*/
#define MSR             6       /*  Modem Status				x			*/

#define DLL             0       /*  Divisor Latch Low			1			*/
#define DLH             1       /*  Divisor latch High			1			*/


/*-------------------------------------------------------------------*
  Bit values held in the Line Control Register (LCR).
	bit		meaning
	---		-------
	0-1		00=5 bits, 01=6 bits, 10=7 bits, 11=8 bits.	/word length select bit
	2		Stop bits.									/	
	3		0=parity off, 1=parity on.					/Parity Enable=1
	4		0=parity odd, 1=parity even.				/Odd or Even select
	5		Sticky parity.								/
	6		Set break.
	7		Toggle port addresses.						/1:access 
 *-------------------------------------------------------------------*/
#define LCR_NO_PARITY       0x00
#define LCR_EVEN_PARITY     0x18
#define LCR_ODD_PARITY      0x08



/*-------------------------------------------------------------------*
  Bit values held in the Line Status Register (LSR).
	bit		meaning
	---		-------
	0		Data ready.
	1		Overrun error - Data register overwritten.
	2		Parity error - bad transmission.
	3		Framing error - No stop bit was found.
	4		Break detect - End to transmission requested.
	5		Transmitter holding register is empty.
	6		Transmitter shift register is empty.
	7               Time out - off line.
 *-------------------------------------------------------------------*/
#define LSR_RCVRDY          0x01
#define LSR_OVRERR          0x02
#define LSR_PRTYERR         0x04
#define LSR_FRMERR          0x08
#define LSR_BRKERR          0x10
#define LSR_XMTRDY          0x20
#define LSR_XMTRSR          0x40
#define LSR_TIMEOUT			0x80

/*-------------------------------------------------------------------*
  Bit values held in the Modem Output Control Register (MCR).
	bit     	meaning
	---		-------
	0		Data Terminal Ready. Computer ready to go.
	1		Request To Send. Computer wants to send data.
	2		auxillary output #1.
	3		auxillary output #2.(Note: This bit must be
			set to allow the communications card to send
			interrupts to the system)
	4		UART ouput looped back as input.
	5-7		not used.
 *------------------------------------------------------------------*/
#define MCR_DTR             0x01
#define MCR_RTS             0x02
#define MCR_INT				0x08


/*------------------------------------------------------------------*
  Bit values held in the Modem Input Status Register (MSR).
	bit		meaning
	---		-------
	0		delta Clear To Send.
	1		delta Data Set Ready.
	2		delta Ring Indicator.
	3		delta Data Carrier Detect.
	4		Clear To Send.
	5		Data Set Ready.
	6		Ring Indicator.
	7		Data Carrier Detect.
 *------------------------------------------------------------------*/
#define MSR_CTS             0x10
#define MSR_DSR             0x20


/*------------------------------------------------------------------*
  Bit values held in the Interrupt Enable Register (IER).
	bit		meaning
	---		-------
	0		Interrupt when data received.
	1		Interrupt when transmitter holding reg. empty.
	2		Interrupt when data reception error.
	3		Interrupt when change in modem status register.
	4-7		Not used.
 *------------------------------------------------------------------*/
#define IER_RX_INT          0x01
#define IER_TX_INT          0x02


/*------------------------------------------------------------------*
  Bit values held in the Interrupt Identification Register (IIR).
	bit		meaning
	---		-------
	0		Interrupt pending
	1-2             Interrupt ID code
			00=Change in modem status register,
			01=Transmitter holding register empty,
			10=Data received,
			11=reception error, or break encountered.
	3-7		Not used.
 *------------------------------------------------------------------*/
#define IIR_MS_ID           0x00   // Modem status
#define IIR_RX_ID           0x04   // Rceived data OK,and to read the receive buffer
#define IIR_TX_ID           0x02   // Transmitter holding register empty
#define IIR_MASK			0x07   // Get the low 3 bits of IIR 


/*
	These are the port addresses of the 8259 Programmable Interrupt
    Controller (PIC).
*/
#define PIC8259_IMR             0x21   /* Interrupt Mask Register port */
#define PIC8259_ICR             0x20   /* Interrupt Control Port       */


/*
    An end of interrupt needs to be sent to the Control Port of
    the 8259 when a hardware interrupt ends.
*/
#define PIC8259_EOI            0x20   /* End Of Interrupt */




/*
	The (IMR) tells the (PIC) to service an interrupt only if it
	is not masked (FALSE).
*/
#define IRQ0            0xFE  // COM? 1111 1110
#define IRQ1            0xFD  // COM? 1111 1101
#define IRQ2            0xFB  // COM? 1111 1011
#define IRQ3            0xF7  // COM2 1111 0111  /* COM2 */
#define IRQ4            0xEF  // COM1 1110 1111  /* COM1 */
#define IRQ5            0xDF  // COM? 1101 1111 
#define IRQ6            0xBF  // COM? 1011 1111
#define IRQ7            0x7F  // COM? 0111 1111

#define IRQ8            0xFE  // COM? 1111 1110
#define IRQ9            0xFD  // COM? 1111 1101
#define IRQ10           0xFB  // COM? 1111 1011
#define IRQ11           0xF7  // COM? 1111 0111
#define IRQ12           0xEF  // COM? 1110 1111
#define IRQ13           0xDF  // COM? 1101 1111 
#define IRQ14           0xBF  // COM? 1011 1111
#define IRQ15           0x7F  // COM? 0111 1111


#define FALSE				0
#define TRUE				1

#define ESC					0x1B    /* ASCII Escape character */
#define ASCII				0x007F  /* Mask ASCII characters  */


#define NO_ERROR			0       /* 无错误No error               */

#define BUF_OVFL			1       /* 缓冲区溢出Buffer overflowed      */

#define SBUFSIZ				512 	/* Serial buffer size     */
#define IBUF_LEN			2048    // 接收缓冲区Incoming buffer
#define OBUF_LEN			1024    // 发送缓冲区Outgoing buffer

	unsigned int PortBaseAddr[6]= {COM1BASE,COM2BASE,COM3BASE,COM4BASE,COM5BASE,COM6BASE};

	// 70-8  71-9  72-10  73-11  74-12  75-13 76-14 77-15
	int InterruptNo[6]=  { 0x0C, 0x0B, 0x0D,  0x72,  0x73,  0x77};//4,3,5,10,11,15
	int ComIRQ[6] =     { IRQ4, IRQ3, IRQ5,  IRQ10, IRQ11, IRQ15};

///////////////////////
 class GSerial{
	int flag;

public:

	unsigned int   m_unPortNo;
	unsigned int   m_unPortBase;
	GSerial(void);
	~GSerial(void);

	int  InitSerialPort(int Port, int Speed, int Parity, int Bits, int StopBit);
	void CloseSerialPort(void);
	int  SetDataFormat(int Parity, int Bits, int StopBit);
	int  SetSpeed(int Speed);
	int  SetPortBaseAddr(int Port);
	void CommOn(void);
	void CommOff(void);

	int ReadStatus(void);
	void SendChar(unsigned char unCh);
	void SendString(int nStrlen, unsigned char *unChBuf);
	//char ReadChar(void);

	void interrupt(*OldVects)(...);

	void SetVects(void interrupt(*New_Int)(...));
	void ResetVects(void);

};


