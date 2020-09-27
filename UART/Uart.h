/*
 * Uart.h
 *
 *  Created on: Sep 21, 2020
 *      Author: Yousef
 */

#ifndef UART_H_
#define UART_H_

void UART_Init(void);
int UART_InChar(void);
void UART_OutChar(int data);
void UART_OutString(char data[]);
void UART_OutNumber(int Number);



#endif /* UART_H_ */
