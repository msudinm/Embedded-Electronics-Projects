#include <LPC214x.h>
#include <stdio.h>
/*------------------------------------------------------------------------------*/
void timer_init()
{
	//initialize timer//
	T0CTCR = 0x00; //select timer mode
	T0TCR = 0x02; //reset timer
}
/*------------------------------------------------------------------------------*/
void delay(int i) //function for delay
{
	T0TCR = 0x02; //reset timer
	T0TCR = 0x01; //enable timer
	while(T0TC < i); //wait for required delay
	T0TC = 0x00; //reset timer counter to 0
}
/*------------------------------------------------------------------------------*/
void pll_setup() //initialize PLL
{
	PLL0CON = 0x01; //enable PLL
	PLL0CFG = 0x24; //calculated for Fosc=12mhz, CCLK=60mhz
	PLL0FEED = 0xAA; //feed sequence
	PLL0FEED = 0x55; //feed sequence
	
	while((PLL0STAT & (1<<10))==0); //wait PLL to lock
	
	PLL0CON = 0x03; //connect PLL
	PLL0FEED = 0xAA; //feed sequence
	PLL0FEED = 0x55; //feed sequence
	
	VPBDIV = 0x00; //pclk=1/4 of cclk =15mhz
}
/*------------------------------------------------------------------------------*/
void uart_setup() //initialize UART
{
	U0LCR = 0x83; /* 8 bits, no Parity, 1 Stop bit | DLAB set to 1 */
	U0DLL = 0x61;
	U0DLM = 0x00; //dll and dlm calculated for pclk=15mhz as PLL and baud rate=9600
	U0FDR = 0xF0; //mulval=15 and divaddval=0
	U0LCR &= 0x7F; // Set DLAB=0 to lock MULVAL and DIVADDVAL
  //BaudRate is now ~9600 and we are ready for UART communication! 
}
/*------------------------------------------------------------------------------*/
void tx(char ch) //function to transmit single byte data over UART
{
	while((U0LSR & (1<<5))==0); //wait for U0THR is empty
	U0THR = ch;
}
/*------------------------------------------------------------------------------*/
void tx_line(char* str) //function to transmit string data over UART
{
	while(*str != 0x00)
	{
		tx(*str);
		str++;
	}
}
/*------------------------------------------------------------------------------*/
char rx() //function to receive single byte data through UART
{
	while((U0LSR & (1<<0))==0); //wait for U0RBR contains data
	return U0RBR;
}
/*------------------------------------------------------------------------------*/
void lcd_delay(int t)
{
	int i,j;
	for(i=0;i<=100*t;i++)
	{
		for(j=0;j<=1275;j++);
	}
}
/*------------------------------------------------------------------------------*/
void lcd_cmd(char cmd) //send command to lcd
{
	IOCLR0 = 0x00003C00; //clear all data pins
	IOCLR1 = 0x01C00000;
	IOPIN0 = ((cmd>>4 & 0x0F)<<10); //load MSB to data pins
	IOCLR1 = (1<<24); //set 0 to RS pin for command mode
	IOCLR1 = (1<<23); //set 0 ro RW pin for write mode
	IOSET1 = (1<<22); //latch EN pin
	lcd_delay(1);
	IOCLR1 = (1<<22); //clear EN pin
	lcd_delay(1);
	
	IOCLR0 = 0x00003C00;
	IOCLR1 = 0x01C00000;
	IOPIN0 = ((cmd & 0x0F)<<10); //load LSB to data pins
	IOCLR1 = (1<<24);
	IOCLR1 = (1<<23);
	IOSET1 = (1<<22);
	lcd_delay(1);
	IOCLR1 = (1<<22);
	lcd_delay(1);
}
/*------------------------------------------------------------------------------*/
void lcd_init() //initialize lcd
{
	lcd_cmd(0x28);  //4-bit mode
	lcd_cmd(0x01);  //clear lcd
	lcd_cmd(0x02);	//return home
	lcd_cmd(0x06);  //entry mode
	lcd_cmd(0x0F);  //cursor blinking
	lcd_cmd(0x80);  //cursor to beginning of 1st line
}
/*------------------------------------------------------------------------------*/
void lcd_data(char d) //send data to lcd
{
	IOCLR0 = 0x00003C00; //clear all data pins
	IOCLR1 = 0x01C00000;
	IOPIN0 = ((d>>4 & 0x0F)<<10); //load MSB to data pins
	IOSET1 = (1<<24); //set 1 to RS pin for data mode
	IOCLR1 = (1<<23); //set 0 ro RW pin for write mode
	IOSET1 = (1<<22); //latch EN pin
	lcd_delay(1);
	IOCLR1 = (1<<22); //clear EN pin
	lcd_delay(1);
	
	IOCLR0 = 0x00003C00;
	IOCLR1 = 0x01C00000;
	IOPIN0 = ((d & 0x0F)<<10); //load LSB to data pins
	IOSET1 = (1<<24);
	IOCLR1 = (1<<23);
	IOSET1 = (1<<22);
	lcd_delay(1);
	IOCLR1 = (1<<22);
	lcd_delay(1);
}
/*------------------------------------------------------------------------------*/
void lcd_display(char *p) //send data string to lcd
{
	int i;
	for(i=0;*p!='\0';i++)
	{
		lcd_data(*p++);
	}
}
/*------------------------------------------------------------------------------*/
unsigned int adc_read() //function to read analog data
{
	unsigned int a;
	AD0CR = 0x01200308; //laod all adc settings
	while((AD0GDR & (1<<31))==0); //wait till adc conversion is complete
	a = AD0GDR; //load adc data register into 'a'
	AD0GDR = 0x00; //clear adc data register
	a = a>>6; //right shift result by 6 bits to get actual data at LSB 
	a = a & 0x000003FF; //mask result with all 1's to clear any bits other than actual data
	return a; 
}
/*------------------------------------------------------------------------------*/



