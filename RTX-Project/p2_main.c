#include<LPC17xx.h>
#include "stdio.h"
#include "uart.h"

#include "GLCD.h" //exercise 4

 #include <cmsis_os2.h> //exercise 5

void ledSwitch(unsigned int num)
{
	LPC_GPIO1->FIOSET |= 1<<28;
	LPC_GPIO1->FIOSET |= 1<<29;
	LPC_GPIO1->FIOSET |= 1<<31;
	
	LPC_GPIO2->FIOSET |= 1<<2;
	LPC_GPIO2->FIOSET |= 1<<3;
	LPC_GPIO2->FIOSET |= 1<<4;
	LPC_GPIO2->FIOSET |= 1<<5;
	LPC_GPIO2->FIOSET |= 1<<6;

	if ((num & 1) == 0)
		LPC_GPIO2->FIOCLR |= 1<<6;	
	
	if ((num & 2) == 0)
		LPC_GPIO2->FIOCLR |= 1<<5;
	
	if ((num & 4) == 0 )
		LPC_GPIO2->FIOCLR |= 1<<4;
	
	if ((num & 8) == 0 )
		LPC_GPIO2->FIOCLR |= 1<<3;
		
	if ((num & 16) == 0 )
		LPC_GPIO2->FIOCLR |= 1<<2;
				
	if ((num & 32) == 0 )
		LPC_GPIO1->FIOCLR |= 1<<31;
					
	if ((num & 64) == 0 )
		LPC_GPIO1->FIOCLR |= 1<<29;
						
	if ((num & 128) == 0 )
		LPC_GPIO1->FIOCLR |= 1<<28;
	
}




void joysStick (void)
{
	if(!(LPC_GPIO1->FIOPIN &= (1<<23)))
		printf("LEFT \r\n");
	
	if(!(LPC_GPIO1->FIOPIN &= (1<<24)))
		printf("UP \r\n");
	
	if(!(LPC_GPIO1->FIOPIN &= (1<<26)))
		printf("DOWN \r\n");
	
	if(!(LPC_GPIO1->FIOPIN &= (1<<25)))
		printf("RIGHT \r\n");
	
	if(!(LPC_GPIO1->FIOPIN &= (1<<20)))
		printf("JOYSTICK_PUSHED \r\n");
	
	if(!(LPC_GPIO2->FIOPIN &= (1<<10)))
		printf("PUSHBUTTON_PUSHED \r\n");
	
}

void analog(void)
{
	LPC_PINCON->PINSEL1 &= !(1<<18); //clear bit 18
	LPC_PINCON->PINSEL1 &= !(1<<19); //clear bit 19
	
	LPC_PINCON->PINSEL1 |= 1<<18; //set bit 18
	
	LPC_SC->PCONP |= 1 << 12; //set bit 12
	
	LPC_ADC->ADCR = (1<<2)|(4<<8)|(1<<21);
	
	
	while (TRUE)
	{
		LPC_ADC->ADCR |= 1 << 24; //start conversion
		while((LPC_ADC->ADGDR & 0x80000000) == 0)
		{
			printf("%x\n", (LPC_ADC->ADGDR & 0xFFF0) >> 4);
		}
	}
}





void printHello(void *args)
{
	while(1)
	{
		printf("Hello");
		osThreadYield();
	}
}

void analog_Thread(void *args)
{
	while(1)
	{
		analog();
		osThreadYield();
	}
}	

void joyStick_Thread(void *args)
{
	GLCD_ClearLn(0, 1);
	GLCD_ClearLn(2, 1);
	unsigned char up[10] = "UP";
	unsigned char down[10] = "DOWN";
	unsigned char left[10] = "LEFT";
	unsigned char right[10] = "RIGHT";
	unsigned char no_dir[10] = "NO DIR";
	unsigned char pressed[10] = "PRESSED";
	unsigned char not_pressed[15] = "NOT PRESSED";
	unsigned char *ptr;
	
	while(1)
	{
	if(!(LPC_GPIO1->FIOPIN &= (1<<23)))
	{
		GLCD_ClearLn(0, 1);
		ptr = left;
		GLCD_DisplayString(0, 0, 1, ptr);
	}
	
	if(!(LPC_GPIO1->FIOPIN &= (1<<24)))
	{
		GLCD_ClearLn(0, 1);
		ptr = up;
		GLCD_DisplayString(0, 0, 1, ptr);
	}
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<26)))
	{
		GLCD_ClearLn(0, 1);
		ptr = down;
		GLCD_DisplayString(0, 0, 1, ptr);
	}
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<25)))
	{
		GLCD_ClearLn(0, 1);
		ptr = right;
		GLCD_DisplayString(0, 0, 1, ptr);
	}
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<20)))
	{
		GLCD_ClearLn(2, 1);
		ptr = pressed;
		GLCD_DisplayString(2, 0, 1, ptr);
	}
	else
	{
		ptr = no_dir;
		GLCD_DisplayString(0, 0, 1, ptr);
		ptr = not_pressed;
		GLCD_DisplayString(2, 0, 1, ptr);
	}
		osThreadYield();
	}
}	




void LED_Thread(void *args)
{
	LPC_GPIO2->FIOCLR |= 1<<6;
	while(1)
	{
		while(!(LPC_GPIO2->FIOPIN &= (1<<10)))
		{
			LPC_GPIO2->FIOSET |= 1<<6;	
		}
		osThreadYield();
	}	
}

	//unsigned char string[20] = "Hello World!";
	//unsigned char *s;
	//s = string;
	
	//GLCD_SetBackColor(0x001F);
	//GLCD_DisplayString(0, 0, 1, s);

	//osThreadNew(printHello,NULL,NULL);
int main(void)
{
	SystemInit();
	
	GLCD_Init();
	

	
	LPC_GPIO1->FIODIR |= 1<<28;
	LPC_GPIO1->FIODIR |= 1<<29;
	LPC_GPIO1->FIODIR |= 1<<31;
	
	LPC_GPIO2->FIODIR |= 1<<2;
	LPC_GPIO2->FIODIR |= 1<<3;
	LPC_GPIO2->FIODIR |= 1<<4;
	LPC_GPIO2->FIODIR |= 1<<5;
	LPC_GPIO2->FIODIR |= 1<<6;
	
	//printf("world \n");
	//ledSwitch(123);
	//analog();
	

	//while(1)
	//{
		//joysStick();
	//}
	
	osKernelInitialize();
	

	osThreadNew(analog_Thread,NULL,NULL);
	osThreadNew(joyStick_Thread,NULL,NULL);
	osThreadNew(LED_Thread,NULL,NULL);
	
	osKernelStart();
	
	while(1);
}