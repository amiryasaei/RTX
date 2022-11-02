#include<LPC17xx.h>
#include "stdio.h"
#include "uart.h"

#include "GLCD.h" //exercise 4

 #include <cmsis_os2.h> //exercise 5

void ledSwitch(unsigned int num)
{
	//turn on all LEDs
	LPC_GPIO1->FIOSET |= 1<<28;
	LPC_GPIO1->FIOSET |= 1<<29;
	LPC_GPIO1->FIOSET |= 1<<31;
	
	LPC_GPIO2->FIOSET |= 1<<2;
	LPC_GPIO2->FIOSET |= 1<<3;
	LPC_GPIO2->FIOSET |= 1<<4;
	LPC_GPIO2->FIOSET |= 1<<5;
	LPC_GPIO2->FIOSET |= 1<<6;

	if ((num & 1) == 0) //clear 2^0 bit
		LPC_GPIO2->FIOCLR |= 1<<6;	
	
	if ((num & 2) == 0) //clear 2^1 bit
		LPC_GPIO2->FIOCLR |= 1<<5;
	
	if ((num & 4) == 0 ) //clear 2^2 bit
		LPC_GPIO2->FIOCLR |= 1<<4;
	
	if ((num & 8) == 0 )//clear 2^3 bit
		LPC_GPIO2->FIOCLR |= 1<<3;
		
	if ((num & 16) == 0 )//clear 2^4 bit
		LPC_GPIO2->FIOCLR |= 1<<2;
				
	if ((num & 32) == 0 )//clear 2^5 bit
		LPC_GPIO1->FIOCLR |= 1<<31;
					
	if ((num & 64) == 0 )//clear 2^6 bit
		LPC_GPIO1->FIOCLR |= 1<<29;
						
	if ((num & 128) == 0 )//clear 2^7 bit
		LPC_GPIO1->FIOCLR |= 1<<28;
}


void joysStick (void)
{
	//Read the pins assigned to each bit
	if(!(LPC_GPIO1->FIOPIN &= (1<<23)))
		printf("LEFT \r\n");
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<24)))
		printf("UP \r\n");
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<26)))
		printf("DOWN \r\n");
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<25)))
		printf("RIGHT \r\n");
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<20)))
		printf("JOYSTICK_PUSHED \r\n");
	
	else if(!(LPC_GPIO2->FIOPIN &= (1<<10)))
		printf("PUSHBUTTON_PUSHED \r\n");
	
}

void analog(void)
{
	LPC_PINCON->PINSEL1 &= !(1<<18); //clear bit 18
	LPC_PINCON->PINSEL1 &= !(1<<19); //clear bit 19
	
	LPC_PINCON->PINSEL1 |= 1<<18; //set bit 18
	
	LPC_SC->PCONP |= 1 << 12; //set bit 12
	
	LPC_ADC->ADCR = (1<<2)|(4<<8)|(1<<21); //reading analog values
	
	
	while (TRUE)
	{
		LPC_ADC->ADCR |= 1 << 24; //start conversion
		while((LPC_ADC->ADGDR & 0x80000000) == 0) //check the 31st bit to be 0
		{
			printf("%x\n", (LPC_ADC->ADGDR & 0xFFF0) >> 4); //extract bits 4 to 15
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
	GLCD_ClearLn(0, 1);//clear the first line
	GLCD_ClearLn(2, 1);//clear the second line
	
	//define char arrays for each string message
	unsigned char up[10] = "UP";
	unsigned char down[10] = "DOWN";
	unsigned char left[10] = "LEFT";
	unsigned char right[10] = "RIGHT";
	unsigned char no_dir[10] = "NO DIR";
	unsigned char pressed[10] = "PRESSED";
	unsigned char not_pressed[15] = "NOT PRESSED";
	
	//define a pointer to point to the char arrays
	unsigned char *ptr;
	
	while(1)
	{
	if(!(LPC_GPIO1->FIOPIN &= (1<<23)))//left input
	{
		GLCD_ClearLn(0, 1);
		ptr = left;
		GLCD_DisplayString(0, 0, 1, ptr);
		osDelay(50);
	}
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<24)))//up input
	{
		GLCD_ClearLn(0, 1);
		ptr = up;
		GLCD_DisplayString(0, 0, 1, ptr);
		osDelay(50);
	}
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<26)))//down input
	{
		GLCD_ClearLn(0, 1);
		ptr = down;
		GLCD_DisplayString(0, 0, 1, ptr);
		osDelay(50);
	}
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<25)))//right input
	{
		GLCD_ClearLn(0, 1);
		ptr = right;
		GLCD_DisplayString(0, 0, 1, ptr);
		osDelay(50);
	}
	
	else if(!(LPC_GPIO1->FIOPIN &= (1<<20)))//joyStick press input
	{
		GLCD_ClearLn(2, 1);
		ptr = pressed;
		GLCD_DisplayString(2, 0, 1, ptr);
		osDelay(50);
	}
	else
	{ 
		ptr = no_dir;//no direction is given
		GLCD_DisplayString(0, 0, 1, ptr);
		ptr = not_pressed;//joystick is not pressed
		GLCD_DisplayString(2, 0, 1, ptr);
		osDelay(50);
	}
	osThreadYield();
}
}	

void LED_Thread(void *args)
{
	_Bool LED = FALSE; //state of LED
	_Bool previouse_state = FALSE;//previouse state of the button
	while(1)
	{
		if(!(LPC_GPIO2->FIOPIN & (1<<10)))//button is pushed
		{
			if (LED == FALSE && previouse_state == TRUE)
			{
				LPC_GPIO2->FIOSET |= 1<<6; //set led
				LED = TRUE;
				previouse_state = FALSE;
			}
		  else if (LED == TRUE && previouse_state == TRUE)
			{
				LPC_GPIO2->FIOCLR |= 1<<6; //clear led
				LED = FALSE;
				previouse_state = FALSE;
			}
		}
		else //Update previouse state when not pressed
				previouse_state = TRUE;
		osThreadYield();
	}
}

int main(void)
{
	//defining LED pins  
	LPC_GPIO1->FIODIR |= 1<<28;
	LPC_GPIO1->FIODIR |= 1<<29;
	LPC_GPIO1->FIODIR |= 1<<31;
	
	LPC_GPIO2->FIODIR |= 1<<2;
	LPC_GPIO2->FIODIR |= 1<<3;
	LPC_GPIO2->FIODIR |= 1<<4;
	LPC_GPIO2->FIODIR |= 1<<5;
	LPC_GPIO2->FIODIR |= 1<<6;
	
	SystemInit();
	
	GLCD_Init();
	//exercise 4
	//unsigned char string[20] = "Hello World!";
	//unsigned char *s;
	//s = string;
	//GLCD_ClearLn(0, 1); //clear line 1
	//GLCD_ClearLn(2, 1); //clear line 2
	//GLCD_Clear(0x07E0); //change LCD background
	//GLCD_SetBackColor(0x001F); //setting the background of the text
	//GLCD_DisplayString(0, 0, 1, s); //display string

	
	//printf("world \n");
	//ledSwitch(123);
	//analog(); //read analog values
	

	//while(1)
	//{
	//	joysStick(); //read joyStick
	//}
	
	osKernelInitialize();
	
		GLCD_Clear(0x07E0); //clear LCD
		osThreadNew(analog_Thread,NULL,NULL);
		osThreadNew(joyStick_Thread,NULL,NULL);
		osThreadNew(LED_Thread,NULL,NULL);
	
	osKernelStart();
	
	while(1);
}