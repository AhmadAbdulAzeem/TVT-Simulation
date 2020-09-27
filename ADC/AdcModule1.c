#include "tm4c123gh6pm.h"
#include "AdcModule1.h"

void ADC_init(void)
{
	volatile uint32_t delay;
	
	/****************************clock initialization*******************************************/
	SYSCTL_RCGCGPIO_R |= 0x10;  		//enable clock
	SYSCTL_RCGCADC_R |= 2;      		//enable clock to ADC0
	
	
	
	/************************Initialize PE2 for AIN0 input**************************************/
	delay=0; 												//dummy var
	GPIO_PORTE_AFSEL_R |= 4;    		//enable alternate function
	GPIO_PORTE_DEN_R &= ~4;					//disable digital function
	GPIO_PORTE_AMSEL_R |= 4;				//enable analog function
	
	
	/*********************************Intialize ADC**********************************************/
	ADC1_ACTSS_R &= ~4;							//disable SS3 during configuration
	ADC1_EMUX_R |= 0X0000;					//software trigger conversion
	ADC1_SSMUX2_R = 1;							//get input from channel 0
	ADC1_SSCTL2_R |= 6;							//take one sample at a time, set flag at 1st sample
	ADC1_ACTSS_R |= 4;							//enable ADC1 sequencer 2
}


uint16_t ADC_readChannel(void)
{
	volatile uint16_t result;
	ADC1_PSSI_R |= 4;								//start a conversion sequence 2
	while((ADC1_RIS_R & 4) == 0);		//wait for conversion complete
	result = ADC1_SSFIFO2_R;				//read conversion result
	ADC1_ISC_R |= 4;									//clear completion flag
	return result;
}