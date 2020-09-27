#include "LCD.h"
#include "Adc.h"
#include "Port.h"
#include "Dio.h"
#include "AdcModule1.h"
#include "Uart.h"

#define CONTROL_CABINET PF0  // control cabinet and cooling water, compressed air on same pin
#define V1_AND_V2       PF1    
#define P1_AND_P2       PF2	 
#define V5              PF3
#define V12_AND_V13     PA2
#define P7_AND_P8       PD7
#define HEATER          PD2 
#define DOOR            PF4
#define V10_AND_V11     PD3
#define V16							PD6
#define P3_AND_P4       PA4
#define P6							PE0
#define V3_AND_V4 			PE1
#define COOLING_LED 		PA3
#define DOOR_LED				PE4

#define HEATING_TIME 9000

void SystemInit(){}
void PORTE_init(void);
void init();

/* Adc configuration */
AdcChannel channels[1] = {PE3};
Adc_ValueGroupType data[1];
Adc_ConfigType configPtr = {ADC_MODULE0, Adc_SS3, 1, 1, ADC_TRIGG_SRC_SW, NONE, ADC_HW_TRIG_RISING_EDGE, ADC_ACCESS_MODE_SINGLE, 0, ADC_CONV_MODE_CONTINUOUS, ADC_STREAM_BUFFER_LINEAR, 1, ADC_GROUP_REPL_ABORT_RESTART, channels};
	
/* Port configuration */
PortPin pins[15] = {{PORT_PIN_OUT, TRUE, CONTROL_CABINET, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTF, FALSE},
								   {PORT_PIN_OUT, TRUE, V1_AND_V2, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTF, FALSE},
									 {PORT_PIN_OUT, TRUE, P1_AND_P2, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTF, FALSE},
									 {PORT_PIN_OUT, TRUE, V5, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTF, FALSE},
									 {PORT_PIN_OUT, TRUE, V12_AND_V13, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTA, FALSE},
									 {PORT_PIN_OUT, TRUE, P7_AND_P8, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTD, FALSE},
									 {PORT_PIN_OUT, TRUE, HEATER, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTD, FALSE},
									 {PORT_PIN_IN, TRUE, DOOR, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTF, FALSE},
									 {PORT_PIN_OUT, TRUE, V10_AND_V11, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTD, FALSE},
									 {PORT_PIN_OUT, TRUE, V16, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTD, FALSE},
									 {PORT_PIN_OUT, TRUE, P3_AND_P4, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTA, FALSE},
									 {PORT_PIN_OUT, TRUE, P6, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTE, FALSE},
									 {PORT_PIN_OUT, TRUE, V3_AND_V4, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTE, FALSE},
									 {PORT_PIN_OUT, TRUE, COOLING_LED, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTA, FALSE},
									 {PORT_PIN_OUT, TRUE, DOOR_LED, PORT_PIN_MODE_DIO, PORT_PIN_LEVEL_LOW, FALSE, PORTE, FALSE}
									 
};
Port_ConfigType port = {pins, 15};

int main()
{
	Adc_StatusType status;
	Std_ReturnType read;
	Dio_LevelType level;
	int temp;
	uint16_t AdcData;
	uint32_t pressure;
	// intialize LCD 
	LCD_init();
	
	// Intialize port 
  	Port_Init(&port);
	
	// Intialize ADC PE3
	Adc_Init(&configPtr);
	Adc_StartGroupConversion(1);
	status = Adc_SetupResultBuffer(1, data);
	status = Adc_GetGroupStatus(1);
	read = Adc_ReadGroup(1, data);
	
	// Intialize ADC PE2
	ADC_init();
	
	//Inatialize UART
	UART_Init();
	
	// Intialy door is closed 
	LCD_displayString("Door open!");
	UART_OutString("Door open! \n");
	delayMs(500);
	
	while(Dio_ReadChannel(DOOR) == STD_HIGH);
	LCD_clearScreen();
	LCD_displayString("Door is closed");
	UART_OutString("Door is closed \n");
	delayMs(3000);
	Dio_WriteChannel(DOOR_LED, STD_HIGH);
	delayMs(2000);
	LCD_clearScreen();
	LCD_displayString("Switch on heaters");
	UART_OutString("Switch on heaters \n");
	delayMs(3000);
	Dio_WriteChannel(HEATER, STD_HIGH);
	delayMs(2000);
	
	
	// Reading temperature 
	while(1)
	{
		status = Adc_GetGroupStatus(1);
		read = Adc_ReadGroup(1, data);
		temp = (data[0] * 3.3 * 100) / 4096;
		LCD_clearScreen();
		LCD_displayString("Temperature: ");
		UART_OutString("Temperature: ");
		delayMs(500);
		LCD_vidWriteNumber(temp);
		UART_OutNumber(temp);
		UART_OutString("\n");
		delayMs(500);
		if(temp > 40 )
			break;
	}
	delayMs(1000);
	
	LCD_clearScreen();
	LCD_displayString("Waiting 4 hours");
	UART_OutString("Waiting 4 hours \n");
	delayMs(HEATING_TIME);
	
	LCD_clearScreen();
	LCD_displayString("Switch off Heater");
	UART_OutString("Switch off Heater \n");
	delayMs(3000);
	Dio_WriteChannel(HEATER, STD_LOW);
	delayMs(2000);
	
	LCD_clearScreen();
	LCD_displayString("Cooling, compressing");
	UART_OutString("Cooling, compressing and cabinet on \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and cabinet on");
	delayMs(3000);
	Dio_WriteChannel(CONTROL_CABINET, STD_HIGH);
	delayMs(2000);
	
	LCD_clearScreen();
	LCD_displayString("Switch on P7,P8");
	UART_OutString("Switch on P7,P8 \n");
	delayMs(3000);
	Dio_WriteChannel(P7_AND_P8, STD_HIGH);// Turn on P7 and P8
	delayMs(2000);	                      // wait 5 seconds
	
	LCD_clearScreen();
	LCD_displayString("Switch on v5");
	UART_OutString("Switch on v5 \n");
	delayMs(3000);
	Dio_WriteChannel(V5, STD_HIGH);
	delayMs(3000);
	LCD_clearScreen();
	LCD_displayString("Switch on v12");
	UART_OutString("Switch on v12 and v13 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and v13");
	delayMs(3000);
	Dio_WriteChannel(V12_AND_V13, STD_HIGH); 	// open V12 and V13
	delayMs(2000);  // wait 3 seconds
	
	// Reading pressure
	while(1)
	{
		AdcData = ((ADC_readChannel() * 3.3 ) / 4096) * 10;
		if(AdcData < 35 && AdcData > 25)
		{
			pressure = AdcData * 20 - 500;
			LCD_clearScreen();
			LCD_displayString("Pressure: ");
			UART_OutString("Pressure: ");
			delayMs(250);
			LCD_vidWriteNumber(pressure);
			UART_OutNumber(pressure);
			delayMs(250);
			LCD_displayString("KPa");
			UART_OutString("KPa \n");
			delayMs(250);
		}
		if(AdcData <= 20)  //Stage 1
		{
			break;
		}
		if((AdcData < 25 && AdcData > 20))
		{
			pressure = AdcData * 200 - 4000;
			LCD_clearScreen();
			LCD_displayString("Pressure: ");
			UART_OutString("Pressure: ");
			delayMs(250);
			LCD_vidWriteNumber(pressure);
			UART_OutNumber(pressure);
			delayMs(250);
			LCD_displayString("Pa");
			UART_OutString("Pa \n");
			delayMs(250);
		}
	}
	
	// close V5, V12, and V13
	LCD_clearScreen();
	LCD_displayString("switch off v5");
	UART_OutString("switch off v5 \n");
	delayMs(3000);
	Dio_WriteChannel(V5, STD_LOW);
	delayMs(2000);
	LCD_clearScreen();
	LCD_displayString("Switch off v12");
	UART_OutString("Switch off v12 and v13 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and v13");
	delayMs(3000);
	Dio_WriteChannel(V12_AND_V13, STD_LOW); 
	delayMs(2000);
	LCD_clearScreen();
	LCD_displayString("P7,P8 will be");
	UART_OutString("P7,P8 will be off after 30m \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("off after 30m");
	delayMs(3000);
	
	//Turn on P3, P4, and P6
	LCD_clearScreen();
	LCD_displayString("Switch on p3");
	UART_OutString("Switch on p3 and p4 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and p4");
	delayMs(3000);
	Dio_WriteChannel(P3_AND_P4, STD_HIGH);
	delayMs(2000);
	LCD_clearScreen();
	LCD_displayString("Switch on p6");
	UART_OutString("Switch on p6 \n");
	delayMs(3000);
	Dio_WriteChannel(P6, STD_HIGH);
	// wait 5 seconds
	delayMs(2000);
	
	
	// open V10 and V11
	LCD_clearScreen();
	LCD_displayString("Switch on v10");
	UART_OutString("Switch on v10 and v11 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and v11");
	delayMs(3000);
	Dio_WriteChannel(V10_AND_V11, STD_HIGH);	
	delayMs(2000);
	// open V16
	LCD_clearScreen();
	LCD_displayString("Switch on v16");
	UART_OutString("Switch on v16 \n");
	delayMs(3000);
	Dio_WriteChannel(V16, STD_HIGH);
	delayMs(2000);
	//turn on cooling led (if available)
	LCD_clearScreen();
	LCD_displayString("Cooling cryopump");
	UART_OutString("Cooling cryopump \n");
	delayMs(3000);
	Dio_WriteChannel(COOLING_LED, STD_HIGH);	
	//wait 5 seconds
	delayMs(5000);
			
			

	//Turn off P7 and P8
	LCD_clearScreen();
	LCD_displayString("Switch off p7");
	UART_OutString("Switch off p7 and p8 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and p8");
	delayMs(3000);
	Dio_WriteChannel(P7_AND_P8, STD_LOW);
	delayMs(2000);
	
	//open V3 and V4
	LCD_clearScreen();
	LCD_displayString("Switch on v3");
	UART_OutString("Switch on v3 and v4\n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and v4");
	delayMs(3000);
	Dio_WriteChannel(V3_AND_V4, STD_HIGH);
	delayMs(2000);

			/* this block will read the prssure through ADC and display it through LCD and if it equals 0.1Pa we will move to next step */
	while(1)
	{
		AdcData = ((ADC_readChannel() * 3.3 ) / 4096) * 10;
		if(AdcData < 20 && AdcData > 10)
		{
			pressure = AdcData * 100 - 1000;
			LCD_clearScreen();
			LCD_displayString("Pressure: ");
			UART_OutString("Pressure: ");
			delayMs(250);
			LCD_vidWriteNumber(pressure);
			UART_OutNumber(pressure);
			delayMs(250);
			LCD_displayString("mPa");
			UART_OutString("mPa \n");
			delayMs(250);
		}
		if(pressure <= 100)  //Stage 2
		{
			break;
		}
		
	}
	
	//close V3 and V4
	LCD_clearScreen();
	LCD_displayString("Switch off v3");
	UART_OutString("Switch off v3 and v4 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and v4");
	delayMs(3000);
	Dio_WriteChannel(V3_AND_V4, STD_LOW);
	delayMs(2000);
		
	//Close V10, V11, and V16
	LCD_clearScreen();
	LCD_displayString("Switch off v10");
	UART_OutString("Switch off v10 and v11, v16 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and v11, v16");
	delayMs(3000);
	Dio_WriteChannel(V10_AND_V11, STD_LOW);
	delayMs(1000);
	Dio_WriteChannel(V16, STD_LOW);
	delayMs(3000);
			
	//Turn off P3, P4, and P6
	LCD_clearScreen();
	LCD_displayString("Switch off p3");
	UART_OutString("Switch off p3 and p4 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and p4");
	delayMs(3000);
	Dio_WriteChannel(P3_AND_P4, STD_LOW);
	delayMs(2000);
	LCD_clearScreen();
	LCD_displayString("Switch off p6");
	UART_OutString("Switch off p6 \n");
	delayMs(3000);
	Dio_WriteChannel(P6, STD_LOW);
	delayMs(2000);
			
			
			
			
	//Turn on P1 and P2
	LCD_clearScreen();
	LCD_displayString("Switch on p1");
	UART_OutString("Switch on p1 and p2 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and p2");
	delayMs(3000);
	Dio_WriteChannel(P1_AND_P2, STD_HIGH);
	delayMs(2000);
	
			
	// open V1 and V2
	LCD_clearScreen();
	LCD_displayString("Switch on v1");
	UART_OutString("Switch on v1 and v2 \n");
	LCD_vidSetPosition(2, 0);
	LCD_displayString("and v2");
	delayMs(3000);
	Dio_WriteChannel(V1_AND_V2, STD_HIGH);
	// wait 5 seconds
  delayMs(2000);
		   
			

			/* this block will read the prssure through ADC and display it through LCD and if it equals 10uPa we will move to next step */
	while(1)
	{     
		AdcData = ((ADC_readChannel() * 3.3 ) / 4096) * 10;
		if(AdcData <= 20 && AdcData >= 0)
		{
			pressure = AdcData * 100;
			LCD_clearScreen();
			LCD_displayString("Pressure: ");
			UART_OutString("Pressure: ");
			delayMs(250);
			LCD_vidWriteNumber(pressure);
			UART_OutNumber(pressure);
			delayMs(250);
			LCD_displayString("uPa");
			UART_OutString("uPa \n");
			delayMs(250);
		}
		if(pressure <= 10)  //Stage 3
		{
			break;
		}
	}
	
	
	LCD_clearScreen();
	LCD_displayString("Ready for test");
	UART_OutString("Ready for test \n");
	while(1);
}

void PORTE_init(void)
{
	GPIO_PORTE_DIR_R |= 0x13;				// PE0 and PE1 are outputs
	GPIO_PORTE_AFSEL_R &= 0xEC;				// PE0 and PE1 are GPIOs
	GPIO_PORTE_AMSEL_R &= 0xEC;				// PE0 and PE1 are digital
	GPIO_PORTE_DEN_R |= 0x13;				// PE0 and PE1 are digital
	GPIO_PORTE_DATA_R |= 0x13;
}


void init()
{
	GPIO_PORTF_CR_R=0XFF;
	GPIO_PORTF_AFSEL_R=0;
	GPIO_PORTF_PCTL_R=0;
	GPIO_PORTF_AMSEL_R=0;
	GPIO_PORTF_DIR_R &= ~0X10;
	GPIO_PORTF_DEN_R |= 0X10;
	GPIO_PORTF_PUR_R |= 0X10;
}