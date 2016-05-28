/*
 * BNO55.c
 *
 *  Created on: Jan 17, 2016
 *      Author: raul martinez
 */

#include "BNO55.h"
//#include "headers.h"
//#include "driverlib/gpio.c"
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"

//#include "driverlib/gpio.c"

#include <xdc/runtime/timestamp.h>
#include "xdc/runtime/System.h"
#include "ti/sysbios/knl/Task.h"
static BNO55_Object ori_object[1] = {0};
BNO55_Handle BNO55_Create(uint8_t sensorNumber, uint8_t address,I2C_Handle i2c_handle)
{

	ori_object[sensorNumber]._address = address;
	ori_object[sensorNumber]._handle_i2c = i2c_handle;
	return 	&(ori_object[sensorNumber]);
}
bool BNO55_init(BNO55_Handle handle){

	bool i=true;
	//CHECK TO SEE THAT SENSOR IS CONNECTED
	while(getID(handle)!=0xA0){
//	    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_4,GPIO_PIN_4);
//	    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_4,0);
//		Task_sleep((UInt)15000);
//	    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_4,GPIO_PIN_4);



		}


	  /* Switch to config mode (just in case since this is the default) */
	  i&=setMode(handle,OPERATION_MODE_CONFIG);

	  /* Reset */
	  i&=Write_Byte(handle,BNO055_SYS_TRIGGER_ADDR, 0x20);

		Task_sleep((UInt)15000);
		while (getID(handle)!= 0xA0)
		  {	Task_sleep((UInt)150);
		  }

		/* Set to normal power mode */
		i&= Write_Byte(handle,BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);


		i&= Write_Byte(handle,BNO055_PAGE_ID_ADDR, 0);

		i&=Write_Byte(handle,BNO055_SYS_TRIGGER_ADDR, 0x0);

	  /* Set the requested operating mode (see section 3.3) */
		i&= setMode(handle,OPERATION_MODE_NDOF);
		return i;


}

bool Read_Byte(BNO55_Handle sensor, uint8_t reg_addr, uint8_t *data)
{
	I2C_Transaction  i2cTransaction;
	unsigned char	 writeBuffer[1];
	bool status;

	//writeBuffer[0]=(reg_addr >> 8) & 0xFF;
	writeBuffer[0]=reg_addr  & 0xFF;

	i2cTransaction.slaveAddress = sensor->_address;     /* 7-bit peripheral slave address */
	i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
	i2cTransaction.writeCount = 1;          /* Number of bytes to be written */
	i2cTransaction.readBuf = data;    /* Buffer to be read */
	i2cTransaction.readCount = 1;           /* Number of bytes to be read */

	status =  I2C_transfer(sensor->_handle_i2c, &i2cTransaction); /* Perform I2C transfer */
	return status;
}

bool Read_len(BNO55_Handle sensor, uint8_t reg_addr, uint8_t *data,int count)
{
	I2C_Transaction  i2cTransaction;
	unsigned char	 writeBuffer[1];
	bool status;

	//writeBuffer[0]=(reg_addr >> 8) & 0xFF;
	writeBuffer[0]=reg_addr  & 0xFF;

	i2cTransaction.slaveAddress = sensor->_address;     /* 7-bit peripheral slave address */
	i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
	i2cTransaction.writeCount = 1;          /* Number of bytes to be written */
	i2cTransaction.readBuf = data;    /* Buffer to be read */
	i2cTransaction.readCount = count;           /* Number of bytes to be read */

	status =  I2C_transfer(sensor->_handle_i2c, &i2cTransaction); /* Perform I2C transfer */
	return status;
}

bool Write_Byte(BNO55_Handle sensor, uint8_t reg_addr, uint8_t data)
{
	I2C_Transaction  i2cTransaction;
	unsigned char	 writeBuffer[2];
	bool status;

//	writeBuffer[0]=(reg_addr >> 8) & 0xFF;
	writeBuffer[0]=reg_addr  & 0xFF;
	writeBuffer[1]=data  & 0xFF;

	i2cTransaction.slaveAddress = sensor->_address;     /* 7-bit peripheral slave address */
	i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
	i2cTransaction.writeCount = 2;          /* Number of bytes to be written */
	i2cTransaction.readBuf = NULL;    /* Buffer to be read */
	i2cTransaction.readCount = 0;           /* Number of bytes to be read */

	status = I2C_transfer(sensor->_handle_i2c, &i2cTransaction); /* Perform I2C transfer */

	if(status==0){

		Task_sleep((UInt)10);

		status = I2C_transfer(sensor->_handle_i2c, &i2cTransaction); /* Perform I2C transfer */
	    System_printf("misfire \n");
	    System_flush();

	}
	return status;
}
/**************************************************************************/
/*!
    @brief  Use the external 32.768KHz crystal SENSOR Should be in config mode already
*/
/**************************************************************************/
void setExtCrystalUse(BNO55_Handle sensor,bool usextal)
{


  /* Switch to config mode (just in case since this is the default) */
  setMode(sensor,OPERATION_MODE_CONFIG);

  Write_Byte(sensor,BNO055_PAGE_ID_ADDR, 0);
  if (usextal) {
	  Write_Byte(sensor,BNO055_SYS_TRIGGER_ADDR, 0x80);
  } else {
	  Write_Byte(sensor,BNO055_SYS_TRIGGER_ADDR, 0x00);
  }

}

/**************************************************************************/
/*!
    @brief  Puts the chip in the specified operating mode
*/
/**************************************************************************/
bool setMode(BNO55_Handle sensor,uint8_t mode)
{

	return Write_Byte(sensor,BNO055_OPR_MODE_ADDR, mode);

}


void getSystemStatus(BNO55_Handle sensor,uint8_t *system_status, uint8_t *self_test_result, uint8_t *system_error)
{
	Write_Byte(sensor, BNO055_PAGE_ID_ADDR, 0);

  /* System Status (see section 4.3.58)
     ---------------------------------
     0 = Idle
     1 = System Error
     2 = Initializing Peripherals
     3 = System Iniitalization
     4 = Executing Self-Test
     5 = Sensor fusio algorithm running
     6 = System running without fusion algorithms */

  if (system_status != 0)
     Read_Byte(sensor,BNO055_SYS_STAT_ADDR, system_status);


  /* Self Test Results (see section )
     --------------------------------
     1 = test passed, 0 = test failed

     Bit 0 = Accelerometer self test
     Bit 1 = Magnetometer self test
     Bit 2 = Gyroscope self test
     Bit 3 = MCU self test

     0x0F = all good! */

  if (self_test_result != 0)
	  Read_Byte(sensor,BNO055_SELFTEST_RESULT_ADDR, self_test_result);


  /* System Error (see section 4.3.59)
     ---------------------------------
     0 = No error
     1 = Peripheral initialization error
     2 = System initialization error
     3 = Self test result failed
     4 = Register map value out of range
     5 = Register map address out of range
     6 = Register map write error
     7 = BNO low power mode not available for selected operat ion mode
     8 = Accelerometer power mode not available
     9 = Fusion algorithm configuration error
     A = Sensor configuration error */

  if (system_error != 0)
	  Read_Byte(sensor,BNO055_SYS_ERR_ADDR, system_error);



}
/**************************************************************************/
/*!
    @brief  Gets current calibration state.  Each value should be a uint8_t
            pointer and it will be set to 0 if not calibrated and 3 if
            fully calibrated.
*/
/**************************************************************************/
void getCalibration(BNO55_Handle sensor,uint8_t* sys, uint8_t* gyro, uint8_t* accel, uint8_t* mag) {



  uint8_t calData;
  Read_Byte(sensor,BNO055_CALIB_STAT_ADDR, &calData);

  if (sys != NULL) {
    *sys = (calData >> 6) & 0x03;
  }
  if (gyro != NULL) {
    *gyro = (calData >> 4) & 0x03;
  }
  if (accel != NULL) {
    *accel = (calData >> 2) & 0x03;
  }
  if (mag != NULL) {
    *mag = calData & 0x03;
  }


}

/**************************************************************************/
/*!
    @brief  Gets the temperature in degrees celsius
*/
/**************************************************************************/

int8_t getTemp(BNO55_Handle sensor){
	uint8_t temp;
	Read_Byte( sensor,  BNO055_TEMP_ADDR, &temp);
	return temp;
}

void getVector(BNO55_Handle sensor,vector* vect,uint8_t vect_type)
{

  uint8_t buffer[6];
  uint16_t* b =( uint16_t*) buffer;
  b[0]=0;
  b[1]=0;
  b[2]=0;


  int16_t* xyz=(int16_t*)vect;
  int16_t x, y, z;
  x = y = z = 0;

  /* Read vector data (6 bytes) */
  Read_len(sensor,(uint8_t)vect_type, buffer, 6);

  x = ((int16_t)buffer[0]) | (((int16_t)buffer[1]) << 8);
  y = ((int16_t)buffer[2]) | (((int16_t)buffer[3]) << 8);
  z = ((int16_t)buffer[4]) | (((int16_t)buffer[5]) << 8);

  /* Convert the value to an appropriate range (section 3.6.4) */
  /* and assign the value to the Vector type */
  switch(vect_type)
  {
    case 0x0E:
      /* 1uT = 16 LSB */
      xyz[0] = ((int16_t)x)/16.0;
      xyz[1] = ((int16_t)y)/16.0;
      xyz[2] = ((int16_t)z)/16.0;
      break;
    case 0x14:
      /* 1rps = 900 LSB */
      xyz[0] = ((int16_t)x)/900.0;
      xyz[1] = ((int16_t)y)/900.0;
      xyz[2] = ((int16_t)z)/900.0;
      break;
    case 0x1A:
      /* 1 degree = 16 LSB */
      xyz[0] = ((int16_t)x)/16.0;
      xyz[1] = ((int16_t)y)/16.0;
      xyz[2] = ((int16_t)z)/16.0;
      break;
    case 0x08:
    case 0x28:
    case 0x2E:
      /* 1m/s^2 = 100 LSB */
      xyz[0] = ((int16_t)x)/100.0;
      xyz[1] = ((int16_t)y)/100.0;
      xyz[2] = ((int16_t)z)/100.0;
      break;
  }


}



uint8_t getID(BNO55_Handle sensor){
	uint8_t temp=0;
	Read_Byte( sensor,  BNO055_CHIP_ID_ADDR, &temp);
	return temp;
}
