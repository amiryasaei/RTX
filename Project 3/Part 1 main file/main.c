#include<LPC17xx.h>
#include "stdio.h"
#include <cmsis_os2.h>
#include "uart.h"

int counter = 0; //defining a global variable as resource

osMutexId_t mutex1; //defining mutex_id

//typedef struct osMutexAttr_t{
//	const char* name;
//	uint32_t attr_bits;
//	void* memory;
//	uint32_t size;
//	
//}mutex;

const osMutexAttr_t Thread_Mutex_attr = {
	"myButtonMutex", // human readable mutex name
	0, // attr_bits
	NULL, // memory for control block
	0U // size for control block
};

void increament (void* args)
{
	int previouse = 0; //previouse state of the button
	int current = 1; //curretn state of the button
	
	while(1)
	{
		osMutexAcquire(mutex1, osWaitForever); //acquiring mutex
		if (!(LPC_GPIO2->FIOPIN & (1<<10)))
		{	//if pushed we change the status of the current state and wait for it to be back to previouse state to increament counter
			current = 0;
			previouse = 1;
		}
		else
		{
			if (current == 0 && previouse == 1)
			{
				//back to previouse state, increament the counter
				counter ++;
				current = 1;
				previouse = 0;
			}
		}
			printf("%d\n",counter);
		osMutexRelease(mutex1); //releasing mutex
		
	}
}
void LED_representation(void* args)
{
	while(1)
	{
		osMutexAcquire(mutex1, osWaitForever); //acquiring mutex

		//clear all LEDs 
		LPC_GPIO1->FIOCLR |= 1<<28;
		LPC_GPIO1->FIOCLR |= 1<<29;
		LPC_GPIO1->FIOCLR |= 1<<31;
		
		LPC_GPIO2->FIOCLR |= 1<<2;
		LPC_GPIO2->FIOCLR |= 1<<3;
		LPC_GPIO2->FIOCLR |= 1<<4;
		LPC_GPIO2->FIOCLR |= 1<<5;
		LPC_GPIO2->FIOCLR |= 1<<6;		

		if ((counter & 1)) //set 2^0 bit
			LPC_GPIO2->FIOSET |= 1<<6;	
		
		if ((counter & 2)) //set 2^1 bit
			LPC_GPIO2->FIOSET |= 1<<5;
		
		if ((counter & 4)) //set 2^2 bit
			LPC_GPIO2->FIOSET |= 1<<4;

		if ((counter & 8))//set 2^3 bit
			LPC_GPIO2->FIOSET |= 1<<3;
			
		if ((counter & 16))//set 2^4 bit
			LPC_GPIO2->FIOSET |= 1<<2;
					
		if ((counter & 32))//set 2^5 bit
			LPC_GPIO1->FIOSET |= 1<<31;
						
		if ((counter & 64))//set 2^6 bit
			LPC_GPIO1->FIOSET |= 1<<29;
							
		if ((counter & 128))//set 2^7 bit
			LPC_GPIO1->FIOSET |= 1<<28;
		
		osMutexRelease(mutex1); //releasing mutex
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
			
	mutex1 = osMutexNew(&Thread_Mutex_attr); //defining mutex
		
	osKernelInitialize();

	osThreadNew(increament,NULL,NULL);
	osThreadNew(LED_representation,NULL,NULL);
	
	osKernelStart();
	
	while(1);
}