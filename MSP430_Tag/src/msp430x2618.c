//! MSP430F2618 clock and I/O definitions

// Included by other 2xx ports, such as the 2274.

#include "platform.h"

#include <signal.h>
#include <io.h>
#include <iomacros.h>
#include "timer.h"
#include  <msp430x22x4.h>
#include <stdlib.h>

char flash_read(char *p)
{
	unsigned char read = 1;
	read = *p;
	sleep_jiffies (200* TIMER1_JIFFIES_PER_MS);
	return read;
}

u_int16_t flash_read16(char *p)
{
	unsigned char read1 = 1;
	unsigned char read2 = 1;
	unsigned char read3 = 1;
	unsigned char read4 = 1;
	read1 = *p++;
	read2 = *p++;
	read3 = *p++;
	read4 = *p;
	sleep_jiffies (50* TIMER1_JIFFIES_PER_MS);
	return ((((u_int16_t)read1)<<12)|(((u_int16_t)read2)<<8))|(((u_int16_t)(read3)<<4)|(read4));
}

void flash_store(unsigned char fb[8])
{
	FCTL2 = FWKEY + FSSEL1 + FN1;     // MCLK/3 for Flash Timing Generator
	flash_clear();            // Block must always be cleared before writing
	
	char *Flash_ptr;
	Flash_ptr = (char *) 0x1001;      //Point to Info Flash segment D
	
	FCTL3 = FWKEY;                            // Clear Lock bit
	FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
	
	*Flash_ptr++ = fb[0];  //OID 
	*Flash_ptr++ = fb[1];
	*Flash_ptr++ = fb[2];
	*Flash_ptr++ = fb[3];  //OID
	*Flash_ptr++ = fb[4];  //CodeBlock
	*Flash_ptr++ = fb[5];
	*Flash_ptr++ = fb[6];
	*Flash_ptr++ = fb[7];  //CodeBlock
	
	FCTL1 = FWKEY;                            // Clear WRT bit
	FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
	sleep_jiffies (100* TIMER1_JIFFIES_PER_MS);
}

void flash_clear(void)
{
	int *p;
	p = (int *)0x1000;    //Point to Info Flash segment D
	
	FCTL3 = FWKEY;        // Clear Lock bit
	FCTL1 = FWKEY + ERASE;    // Set Erase bit
	*p = 0;       // Dummy write to erase Flash segment D
	FCTL1 = FWKEY;        // Clear WRT bit
	FCTL3 = FWKEY + LOCK;     // Set LOCK bit
}


//! Receive a byte.
unsigned char serial_rx(){
  char c;
  
  while(!(IFG2&UCA0RXIFG));//wait for a byte
  c = UCA0RXBUF;
  IFG2&=~UCA0RXIFG;
  
  //UCA0CTL1 &= ~UCA0RXSE;
  return c;
}

//! Receive a byte.
unsigned char serial1_rx(){
  //TODO
  return 00;
}

//! Transmit a byte.
void serial_tx(unsigned char x){
  while ((IFG2 & UCA0TXIFG) == 0); //loop until buffer is free
  UCA0TXBUF = x;	/* send the character */
  while(!(IFG2 & UCA0TXIFG));
}
//! Transmit a byte.
void serial_tx_old(unsigned char x){
  while ((IFG2 & UCA0TXIFG) == 0); //loop until buffer is free
  UCA0TXBUF = x;	/* send the character */
  while(!(IFG2 & UCA0TXIFG));
}

//! Transmit a byte on the second UART.
void serial1_tx(unsigned char x){

}

//! Set the baud rate.
void setbaud(unsigned char rate){
  
  //Table 15-4, page 481 of 2xx Family Guide
  switch(rate){
  case 1://9600 baud
    UCA0BR1 = 0x06;
    UCA0BR0 = 0x82;
    break;
  case 2://19200 baud
    UCA0BR1 = 0x03;
    UCA0BR0 = 0x41;
    break;
  case 3://38400 baud
    UCA0BR1 = 0xa0;
    UCA0BR0 = 0x01;
    break;
  case 4://57600 baud
    UCA0BR1 = 0x1d;
    UCA0BR0 = 0x01;
    break;
  default:
  case 5://115200 baud
    UCA0BR0 = 0x8a;
    UCA0BR1 = 0x00;
    break;
  }
}

//! Set the baud rate of the second uart.
void setbaud1(unsigned char rate){
  
  //http://mspgcc.sourceforge.net/baudrate.html
  switch(rate){
  case 1://9600 baud
    
    break;
  case 2://19200 baud
    break;
  case 3://38400 baud
    
    break;
  case 4://57600 baud
    
    break;
  default:
  case 5://115200 baud
    
    break;
  }
}

#define BAUD0EN 0x41
#define BAUD1EN 0x03

void msp430_init_uart(){

  // Serial on P3.4, P3.5
  P3SEL |= BIT4 + BIT5;
  P3DIR |= BIT4;

  //UCA0CTL1 |= UCSWRST;                    /* disable UART */

  UCA0CTL0 = 0x00;
  //UCA0CTL0 |= UCMSB ;

  UCA0CTL1 |= UCSSEL_2;                     // SMCLK

  //UCA0BR0 = BAUD0EN;                        // 115200
  //UCA0BR1 = BAUD1EN;
  setbaud(5);//default baud, 115200

  UCA0MCTL = 0;                             // Modulation UCBRSx = 5
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**


  //Leave this commented!
  //Interrupt is handled by target code, not by bootloader.
  //IE2 |= UCA0RXIE;
}

/* Info Flash Values
CALDCO_16MHZ 0xdc CALBC1_16MHZ 0x8e   2274-000.txt
CALDCO_16MHZ 0x74 CALBC1_16MHZ 0x8f   2618-000.txt
CALDCO_16MHZ 0x6c CALBC1_16MHZ 0x8f   2618-001.txt
CALDCO_16MHZ 0x97 CALBC1_16MHZ 0x8f   2618-002.txt
CALDCO_16MHZ 0x6c CALBC1_16MHZ 0x8f   2618-003.txt
CALDCO_16MHZ 0x95 CALBC1_16MHZ 0x8f   2618-004.txt
CALDCO_16MHZ 0xcc CALBC1_16MHZ 0x8e   2618-005.txt
CALDCO_16MHZ 0x87 CALBC1_16MHZ 0x8f   2618-006.txt
CALDCO_16MHZ 0x96 CALBC1_16MHZ 0x8f   2619-001.txt
*/

//! Initialize the MSP430 clock.
void msp430_init_dco() {
  if(CALBC1_16MHZ!=0xFF){
    //Clear DCL for BCL12
    DCOCTL = 0x00;
    //Info is intact, use it.
    BCSCTL1 = CALBC1_16MHZ;
    DCOCTL = CALDCO_16MHZ;
  }else{
    //Info is missing, guess at a good value.
    BCSCTL1 = 0x8f;   //CALBC1_16MHZ at 0x10f9
    DCOCTL = 0x7f;    //CALDCO_16MHZ at 0x10f8
  }
  
  return;
}

