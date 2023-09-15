#include <lpc214x.h>

/* R1, R2, R3, R4 are row pins of matrix keypad */
/* C1, C2, C3 are column pins of matrix keypad */

char keypad(R1,R2,R3,R4,C1,C2,C3)
{
		char key; //variable to store value of pressed key
	
		IOCLR0 = R1; //set 1st row to LOW, R1=0
		IOSET0 = R2 | R3 | R4; // set remaining rows to HIGH
/*------------------------------------------------------------------------------*/	
		if(!(IOPIN1 & C1)) //if R1=0, C1=0, then key='1' and same further
		{
			key = '1';
			while(!(IOPIN1 & C1));
			goto jump;
		}
		if(!(IOPIN1 & C2))
		{
			key = '2';
			while(!(IOPIN1 & C2));
			goto jump;
		}
		if(!(IOPIN1 & C3))
		{
			key = '3';
			while(!(IOPIN1 & C3));
			goto jump;
		}
/*------------------------------------------------------------------------------*/	
		IOCLR0 = R2; //set 2nd row to LOW, R2=0
		IOSET0 = R1 | R3 | R4; // set remaining rows to HIGH
		
		if(!(IOPIN1 & C1))
		{
			key = '4';
			while(!(IOPIN1 & C1));
			goto jump;
		}
		if(!(IOPIN1 & C2))
		{
			key = '5';
			while(!(IOPIN1 & C2));
			goto jump;
		}
		if(!(IOPIN1 & C3))
		{
			key = '6';
			while(!(IOPIN1 & C3));
			goto jump;
		}
/*------------------------------------------------------------------------------*/		
		IOCLR0 = R3; //set 3rd row to LOW, R3=0
		IOSET0 = R1 | R2 | R4; // set remaining rows to HIGH
		
		if(!(IOPIN1 & C1))
		{
			key = '7';
			while(!(IOPIN1 & C1));
			goto jump;
		}
		if(!(IOPIN1 & C2))
		{
			key = '8';
			while(!(IOPIN1 & C2));
			goto jump;
		}
		if(!(IOPIN1 & C3))
		{
			key = '9';
			while(!(IOPIN1 & C3));
			goto jump;
		}
/*------------------------------------------------------------------------------*/		
		IOCLR0 = R4; //set 4th row to LOW, R4=0
		IOSET0 = R1 | R2 | R3; // set remaining rows to HIGH
		
		if(!(IOPIN1 & C2))
		{
			key = '0';
			while(!(IOPIN1 & C2));
			goto jump;
		}
		/* we can add same operations for '*' and '#' keys also */
/*------------------------------------------------------------------------------*/		
		jump:
		return key;
}

