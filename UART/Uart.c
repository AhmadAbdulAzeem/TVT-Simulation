/*
 * Uart.c
 *
 *  Created on: Sep 21, 2020
 *      Author: Yousef
 */
#include "tm4c123gh6pm.h"
#include "Uart.h"
#include "stdint.h"
void intToArray(int num,char arr[]);

// Assumes a 80 MHz bus clock, creates 115200 baud rate
void UART_Init(void)
{ // should be called only once
    SYSCTL_RCGCUART_R   |= 0x0001; // activate UART0
    SYSCTL_RCGCGPIO_R   |= 0x0001; // activate port A
    UART0_CTL_R         &= ~0x0001; // disable UART
    UART0_IBRD_R        = 8; // IBRD=int(16000000/(16*115,200)) = int(8.680555)
    UART0_FBRD_R        = 45; // FBRD = round(0.68055 * 64+0.5) = 45
    UART0_LCRH_R        = 0x0070; // 8-bit word length, enable FIFO
    UART0_CTL_R         = 0x0301; // enable RXE, TXE and UART
    UART0_CC_R          = 0x05;
    GPIO_PORTA_PCTL_R   = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011; // UART
    GPIO_PORTA_AMSEL_R  &= ~0x03; // disable analog function on PA1-0
    GPIO_PORTA_AFSEL_R  |= 0x03; // enable alt funct on PA1-0
    GPIO_PORTA_DEN_R    |= 0x03; // enable digital I/O on PA1-0
}

//Wait for new input, then return ASCII code
int UART_InChar(void)
{
    while((UART0_FR_R&0x0010) != 0); // wait until RXFE is 0
    return((int)(UART0_DR_R&0xFF));
}

//Wait for buffer to be not full, then output
void UART_OutChar(int data)
{

    while((UART0_FR_R&0x0020) != 0); // wait until TXFF is 0
    UART0_DR_R = data;
}


void UART_OutString(char data[])
{
    int i=0;
    while(data[i] != '\0')
    {
        while((UART0_FR_R&0x0020) != 0); // wait until TXFF is 0
        UART0_DR_R = data[i];
        i++;
    }
}

void UART_OutNumber(int Number)
{
	  int i=0;
    char data[10]={0};
    intToArray(Number , data);
    while(i != 10)
    {
        while((UART0_FR_R&0x0020) != 0); // wait until TXFF is 0
        UART0_DR_R = data[i];
        i++;
    }

}

void intToArray(int num,char arr[])
{
    char c;

    int i=9;
    while(num != 0)
    {
        c = num%10 +'0';
        num = num /10;
        arr[i]=c;
        i--;
    }
}





