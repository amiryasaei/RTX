#include<LPC17xx.h>
#include "stdio.h"
#include <cmsis_os2.h>
#include "uart.h"
#include "MPU9250.h" //IMU library
#include "sensor_fusion.h"//file with sensor fusion algorithm

osMutexId_t mutex1; //defining a mutex_id


const osMutexAttr_t Thread_Mutex_attr = {
	"myButtonMutex", // human readable mutex name
	0, // attr_bits
	NULL, // memory for control block
	0U // size for control block
};

void IMU(void* args)
{
	//Reading the values of gyroscope, accelerometer, and magnetometer
	while(1)
	{
		osMutexAcquire(mutex1, osWaitForever); //acquiring mutex
		MPU9250_read_gyro(); //gyro value
		MPU9250_read_acc(); //acceleration value
		if (MPU9250_st_value & 16) //cheking the 4th bit
				MPU9250_read_mag();//magnetic value
		osMutexRelease(mutex1); // releasing mutex
	}
}
void sensorFusionRead(void* args)
{
	while(1)
	{
		osMutexAcquire(mutex1, osWaitForever); //acquiring mutex
		
		//updating the value of IMU using the sensor fusion algorithm
		sensor_fusion_update(MPU9250_gyro_data[0],MPU9250_gyro_data[1], MPU9250_gyro_data[2], MPU9250_accel_data[0], MPU9250_accel_data[1], MPU9250_accel_data[2], MPU9250_mag_data[0], MPU9250_mag_data[1], MPU9250_mag_data[2]);
		
		osMutexRelease(mutex1);//releasing mutex
	}
}
void print(void* args)	
{	
	while(1)
	{
		osMutexAcquire(mutex1, osWaitForever); //acquiring mutex
		//pringting the values read from IMU
		printf("%f, %f, %f \n",sensor_fusion_getPitch(), sensor_fusion_getYaw(), sensor_fusion_getRoll());
		
		osMutexRelease(mutex1);//releasing mutex
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
	MPU9250_init(1,1); //initializing IMu sensor
	
	if ((MPU9250_whoami() & 1)) //setting 2^0 bit
		LPC_GPIO2->FIOSET |= 1<<6;	
	
	if ((MPU9250_whoami() & 2)) //setting 2^1 bit
		LPC_GPIO2->FIOSET |= 1<<5;
	
	if ((MPU9250_whoami() & 4)) //setting 2^2 bit
		LPC_GPIO2->FIOSET |= 1<<4;

	if ((MPU9250_whoami() & 8))//setting 2^3 bit
		LPC_GPIO2->FIOSET |= 1<<3;
		
	if ((MPU9250_whoami() & 16))//setting 2^4 bit
		LPC_GPIO2->FIOSET |= 1<<2;
				
	if ((MPU9250_whoami() & 32))//setting 2^5 bit
		LPC_GPIO1->FIOSET |= 1<<31;
					
	if ((MPU9250_whoami() & 64))//setting 2^6 bit
		LPC_GPIO1->FIOSET |= 1<<29;
						
	if ((MPU9250_whoami() & 128))//setting 2^7 bit
		LPC_GPIO1->FIOSET |= 1<<28;

	
	sensor_fusion_init();
	sensor_fusion_begin(512); //setting freaquency to 512Hz
	
	
	mutex1 = osMutexNew(&Thread_Mutex_attr); //defining the mutex
	
	osKernelInitialize();
	
	osThreadNew(IMU,NULL,NULL);
	osThreadNew(sensorFusionRead,NULL,NULL);
	osThreadNew(print,NULL,NULL);
	
	osKernelStart();
	while(1);
}