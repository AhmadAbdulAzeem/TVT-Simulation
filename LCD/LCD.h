#ifndef __LCD__H__
#define __LCD__H__


#include "tm4c123gh6pm.h"
//#include "std_types.h"
//#include "IO_ports.h"
#include "stdint.h"

#define LCD_CTRL_PORT GPIO_PORTA_DATA_R
#define LCD_DATA_PORT GPIO_PORTB_DATA_R       


#define RS 0x20 /* PORTA BIT5 mask PA5 */
#define RW 0x40 /* PORTA BIT6 mask PA6 */
#define EN 0x80 /* PORTA BIT7 mask PA7 */




void delayMs(int n);
void delayUs(int n);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_displayString(const char *Str);
char intgerToString(uint8_t num);
void LCD_clearScreen(void);
void LCD_vidWriteNumber(int Number);
void LCD_vidSetPosition(uint8_t row,uint8_t column);


#endif


