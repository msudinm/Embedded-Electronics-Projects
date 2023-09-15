/* ----------------------------------Password Protected Notice Board------------------------------------ */
/* Created by: Sudin Mhatre */
/* Institute: Embedded Technosolutions */
/* Uploaded On: Github.com */

/* In this project, we are interfacing 4x3 matrix keypad, 16x2 LCD display in 4-Bit mode, Bluetooth module HC-05 and EEPROM */
/* Keypad is used to enter password before displaying message on notice board */
/* 16x2 LCD display used as notice board which has user interface to enter password and display message */
/* We will require Bluetooth module connected to smartphone and its TX-RX pins connected to UART pins of LPC 2148 */
/* We have to download any Bluetooth terminal app in smartphone, so that we can type message on it, which will be displayed on 
	 notice board */
/* EEPROM is used to store password */
/*-----------------------------------------------------------------------------------------------------------*/
#include <lpc214x.h>
#include "matrix_keypad.h"
#include "i2c_write_read.h"
#include <stdio.h>
#include <string.h>

#define R1 (1<<6) //pin no. 30 define row 1
#define R2 (1<<7) //pin no. 31 define row 2
#define R3 (1<<8) //pin no. 33 define row 3
#define R4 (1<<9) //pin no. 34 define row 4
#define C1 (1<<19) //pin no. 4 define column 1  /* use internal pull-up pins for columns, only port 1 has internal pull-up */
#define C2 (1<<17) //pin no. 12 define column 2
#define C3 (1<<18) //pin no. 8 define column 3

char pass[5], new_pass[5]; //variable to store password entered through keypad
char msg[100]; //variable to store message string sent through Bluetooth terminal app
char default_pass[] = "4916";  //default password
char* check; //variable to read password from memory

int i=0;

void rx_str(void); //function to receive message string through bluetooth ie UART
void lcd_notice(char* n); //function to display message on notice board
int checkspace(char*, int); //function to check if a word can be occupied on a lcd display line
/*-----------------------------------------------------------------------------------------------------------*/
int main()
{
	char key, option, confirm, flush;
	int flag = 0;
	PINSEL0 = 0x00000055; //set P0.0 & P0.1 as UART, P0.2 & P0.3 as I2C and other pins as GPIO
	PINSEL2 = 0x00000000; // set Port pins as GPIO (for columns pins and lcd control pins)
	IODIR0 = R1 | R2 | R3 | R4; // set rows as output
	IOSET0 = R1 | R2 | R3 | R4; //set all rows to HIGH
	
	IODIR0  |= (1<<10)|(1<<11)|(1<<12)|(1<<13); //P0.10 to P0.13 as output (lcd data pins) (on-board lcd)
	/* P0.10 = D4, P0.11 = D5, P0.12 = D6, P0.13 = D7 */
	
	IODIR1 = 0x01C00000; //P1.22 to P1.24 as output (lcd control pins)
	/* P1.22 = EN, P1.23 = R/W, P1.24 = RS */
	
	pll_setup(); //initialize PLL
	uart_setup(); //initialize UART
	T0PR = 14999; //set timer prescaler for milli-seconds
	lcd_init(); //initialize LCD setup
	delay(10);
	i2c_init(); //initialize I2C
	         
	check = i2c_mem_read(); //read password from memory
								/*------------------------------------------------------------------------------*/
	recheck:
	i=0;
	lcd_cmd(0x01);
	lcd_cmd(0x80);
	lcd_display("enter pswrd:"); //asked to enter password using keypad
	lcd_cmd(0xC0);
	
	while(i<4) //since we have set 4-digit password, this loop scans key-pressing action 4-times
	{
		key = keypad(R1,R2,R3,R4,C1,C2,C3); /* 'keypad' function from 'matrix_keypad.h' header file which scans keys pressed and
		                                      returns corresponding value */
		if((key=='0')||(key=='1')||(key=='2')||(key=='3')||(key=='4')||(key=='5')||(key=='6')||(key=='7')||(key=='8')||(key=='9'))
			/* here we checked whether only 0-9 keys are pressed or not */
		{
			pass[i] = key; //pressed key value stored in 'pass' variable one by one
			lcd_data('*'); //here we ensure that we got pressed key while masking its value
			i++;
		}
	}
					/*------------------------------------------------------------------------------*/
	lcd_cmd(0x01);
	lcd_cmd(0x80);
					
	if((strcmp(check,pass)) && (strcmp(default_pass,pass))) //to check whether entered password and stored password are same or not
	{
		lcd_display("wrong pswrd");
		delay(1000);
		goto recheck; //if password is wrong, repeat above steps
	}
	else
	{
		/* if password is correct, continue further steps */
		lcd_display("pswrd matched");
		delay(500);
		lcd_cmd(0xC0);
		lcd_display("select option"); //ask to choose any of two actions
		delay(1000);
	}
						/*------------------------------------------------------------------------------*/
	redirect:
	lcd_cmd(0x01);
	lcd_cmd(0x80);
	lcd_display("1.set new pswrd"); //press '1' for setting new password
	lcd_cmd(0xC0);
	lcd_display("2.show notice  "); //press '2' for sending message
	flag=0;
	while(flag == 0)
	{
		option = keypad(R1,R2,R3,R4,C1,C2,C3); //get value of key pressed
		if((option=='0')||(option=='1')||(option=='2')||(option=='3')||(option=='4')||(option=='5')||(option=='6')||(option=='7')||(option=='8')||(option=='9'))
		{
			flag++;
		}
	}
	lcd_data(option);  //it will show which option is pressed 1 or 2
	delay(500);
						/*------------------------------------------------------------------------------*/
	switch(option)  //based on option selected, further action takes place
	{
		case '1': //new password 
			lcd_cmd(0x01);
			lcd_cmd(0x80);
			lcd_display("set new pswrd");	
			lcd_cmd(0xC0);
		
			i=0;
			while(i<4) 
			{
				key = keypad(R1,R2,R3,R4,C1,C2,C3);
				if((key=='0')||(key=='1')||(key=='2')||(key=='3')||(key=='4')||(key=='5')||(key=='6')||(key=='7')||(key=='8')||(key=='9'))
				{
					new_pass[i] = key; 
					lcd_data('*'); 
					i++;
				}
			}
			pass_confirm:
			tx_line("New password entered is "); //asking confirmation through bluetooth terminal
			tx_line(new_pass);
			tx_line("\n");
			tx_line("Are you sure? Send y/n"); //to continue send 'y' or to go back send 'n'
			tx_line("\n");
			confirm = rx();
			flush = rx();
			flush = rx();
			if(confirm == 'y') //to continue with new password
			{
				I2C0CONSET = en;
				I2C0CONCLR = start | si | ack | stop; //clear all other bits
				i2c_mem_write(new_pass); //write new password in memory
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				lcd_display("Pswrd changed");
				lcd_cmd(0xC0);
				delay(500);
				strcpy(check,new_pass);
				
				goto recheck; //go back to main screen
			}
			else if(confirm == 'n') //redirect to select option
			{
				goto redirect;
			}
			else
			{
				tx_line("Please select y or n...");
				tx_line("\n");
				goto pass_confirm;
			}
					/*------------------------------------------------------------------------------*/
		case '2':  //send message
			lcd_cmd(0x01);
			lcd_cmd(0x80);
			lcd_display("send message...");
			break;
	} 
	rx_str(); //take input from UART and store all message in 'msg' variable
	lcd_cmd(0x01);
	lcd_cmd(0x80);
	
	while(1) //once message is received it is displayed continously
	{
		lcd_notice(msg); //function to display message
		delay(5000); //wait to allow read the message before it gets cleared
		lcd_cmd(0x01);
		lcd_cmd(0x80);
	}
}
/*-----------------------------------------------------------------------------------------------------------*/
void rx_str() //receive message through Bluetooth ie UART
{
	char t;
	int i=0;
	do
	{
		msg[i] = rx(); //store all message characters one by one
//		tx(msg[i]);
		i++;
	}
	while(msg[i-1] != 0x0D && msg[i-1] != 0x0A && msg[i-1] != '\0'); /*stop when end of message string occurs ie null
	                                                                   character or return feed, etc. */
	t = rx(); //empty UART receive buffer register
	msg[i-1] = '\0'; //add null character at the end of message string
}
/*-----------------------------------------------------------------------------------------------------------*/
void lcd_notice(char* n) //display message on LCD display
{
  int j=0; //position of lcd cursor
	int gap;
  while(*n!='\0') //check string untill null character
	{
		if(j==16) //if end of 1st line, shift cursor to 2nd line
		{
			lcd_cmd(0xC0);
		}
		if(j==32) //if end of 2nd line, then clear display and show remaiming part on fresh screen
		{
			delay(5000);
			lcd_cmd(0x01);
			lcd_cmd(0x80);
			j=0;
		}
		lcd_data(*n); //show all characters one by one
		
		if(*n == ' ')  //check for space to find a word
		{
			if(j<16)
				gap = checkspace((n+1), 16-(j+1)); //if 1st line is ongoing, check if a word can be occupied in that line 
			else if(j<32)
				gap = checkspace((n+1), 32-(j+1));  //if 2nd line is ongoing, check if a word can be occupied in that line
		}
		else
			gap = 1; 

		if(gap == 0)  //it means word cannot be occupied on current line
		{
			if(j<16) //shift the word to 2nd line
			{
				lcd_cmd(0xC0);
				j = 16;
				gap = 1;
			}
			else if(j<32)  //if word already at 2nd line, shift to fresh screen
			{
				delay(5000);
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				j=0;
				gap = 1;
			}
		}
		else if(gap == 1)  //no lne shifting required
		{
			j++;
		}
		
		n++;
	}
}
/*-----------------------------------------------------------------------------------------------------------*/
int checkspace(char* first_letter, int rem_space)  //function to determine whether a word can be occupied on same line
{
	int count = 0, result;
	while(*first_letter != ' ' && *first_letter != '\0')  /* determine word length ie count between two spaces or 
																													between space and end of line */
	{
		count++;
		first_letter++;
	}
	
	if(count > rem_space)  //check if word length is greater than remaining positions on the line
		result = 0;
	else
		result = 1;
	return result;
}
/*-----------------------------------------------------------------------------------------------------------*/
