#include "LCD.h"



/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
int i, j;
for(i = 0 ; i < n; i++)
for(j = 0; j < 3180; j++)
{} /* do nothing for 1 ms */
}



/* delay n microseconds (16 MHz CPU clock) */
void delayUs(int n)
{
int i, j;
for(i = 0 ; i < n; i++)
for(j = 0; j < 3; j++)
{
	//asm ("nop");
	
} /* do nothing for 1 us */
}



void LCD_init(void)
{
	
	//PortA_Init(0XE0,0X00);
	//PortB_Init(0XFF,0X00);
	
	
	//intialize PORTA
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000001;     // 1) activate clock for Port A
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	GPIO_PORTA_LOCK_R = 0x4C4F434B;
  GPIO_PORTA_CR_R |= 0xEC;           // allow changes to PA7-0
  GPIO_PORTA_AMSEL_R &= 0x03;        // 3) disable analog on PA
  GPIO_PORTA_PCTL_R &= 0x000000FF;   // 4) PCTL GPIO on PA7-0
  // rest
  GPIO_PORTA_DIR_R |= 0xEC ;     // 5) direction of pins
  GPIO_PORTA_AFSEL_R &= 0x03;        // 6) disable alt funct on PA0-0
  // rest
  //GPIO_PORTA_PUR_R = 0X00;       // enable pull-up
  GPIO_PORTA_DEN_R |= 0xEC;          //7) enable digital I/O on PA7-0
	
	
	//intialize PORTB
	SYSCTL_RCGC2_R |= 0x00000002;     // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_CR_R = 0xFF;           // allow changes to PB7-0
  GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog on PB
  GPIO_PORTB_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PB7-0
  // rest
  GPIO_PORTB_DIR_R = 0XFF;     // 5) direction of pins
  GPIO_PORTB_AFSEL_R = 0x00;        // 6) disable alt funct on PB0-0
  // rest
  //GPIO_PORTB_PUR_R = pull_up;       // enable pull-up
  GPIO_PORTB_DEN_R = 0xFF;          //7) enable digital I/O on PB7-0
	
	
	
	
	delayMs(20); /* initialization sequence */
  LCD_command(0x30);  //wakeup
	LCD_command(0x30);  //wakeup
	LCD_command(0x30); 
	LCD_command(0x38); /* set 8-bit data, 2-line, 5x7 font */
  LCD_command(0x06); /* move cursor right */
  LCD_command(0x01); /* clear screen, move cursor to home */
  LCD_command(0x0F); /* turn on display, cursor blinking */
}

void LCD_command(unsigned char command)
{
	LCD_CTRL_PORT &= (~0x60);   /* RS = 0, R/W = 0 */
	LCD_DATA_PORT=command;
	LCD_CTRL_PORT|=EN;
	delayMs(2);
	LCD_CTRL_PORT&=~EN;
	/*if (command < 4)
			delayMs(2); 
			else
			delayUs(40);*/
}


void LCD_data(unsigned char data)
{
	LCD_CTRL_PORT |= RS;
	LCD_CTRL_PORT &= (~RW);
	LCD_DATA_PORT=data;
	LCD_CTRL_PORT|= (EN | RS);
	delayMs(2);
	LCD_CTRL_PORT &= (~EN);
	delayUs(40);
}

void LCD_displayString(const char *Str)
{
	uint8_t i=0;
	while(Str[i] != '\0')
	{
		LCD_data(Str[i]);
		i++;
	}
}


char intgerToString(uint8_t num)
{
	return (char) num+'0';
}


void LCD_clearScreen(void)
{
	LCD_command(0x01); //clear display
}


void LCD_vidWriteNumber(int Number)
{
	int reverse_Number=1;
	if(Number < 0)
	{
		Number *= -1;
		LCD_data('-');
	}
	if(Number==0)
	{
		LCD_data('0');
	}

	while(Number!=0)
	{
		reverse_Number=Number%10+reverse_Number*10;
		Number/=10;
	}
	while(reverse_Number!=1)
	{
		LCD_data(reverse_Number%10+48);//+'0'
		reverse_Number/=10;
	}

}

void LCD_vidSetPosition(uint8_t row,uint8_t column)
{

	if(row==1)
	{
		LCD_command(128+column);
	}
	else if(row==2)
	{
		LCD_command(128+64+column);
	}
}



