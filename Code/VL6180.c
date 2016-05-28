/*
 * VL6180.c
 *
 *  Created on: Jun 13, 2015
 *      Author: rskip
 */


#include "VL6180.h"
#include <xdc/runtime/Error.h>

static VL6180X_Object prx_object[VL6180X_COUNT] = {0};

VL6180X_Handle VL6180_Create(uint8_t sensorNumber, uint8_t address,I2C_Handle i2c_handle)
{

	prx_object[sensorNumber]._address = address;
	prx_object[sensorNumber]._handle_i2c=i2c_handle;
	return 	&(prx_object[sensorNumber]);
}


bool VL6180_Init(VL6180X_Handle handle)
{

	bool status;
	uint8_t	date_lo,
			date_hi,
			time_lo,
			time_hi;
	uint8_t reset;
    Error_Block     eb;


    handle->dataAccess = GateMutex_create(NULL, &eb);
    if (!handle->dataAccess) {
    	return (NULL);
    }


	status = VL6180_Read_Byte(handle,VL6180X_SYSTEM__FRESH_OUT_OF_RESET,&reset);

		if( reset == 1)
		{
		// Mandatory : private registers
		VL6180_Write_Byte(handle,0x0207,0x01);
		VL6180_Write_Byte(handle,0x0208,0x01);
		VL6180_Write_Byte(handle,0x0096,0x00);
		VL6180_Write_Byte(handle,0x0097,0xfd);
		VL6180_Write_Byte(handle,0x00e3,0x00);
		VL6180_Write_Byte(handle,0x00e4,0x04);
		VL6180_Write_Byte(handle,0x00e5,0x02);
		VL6180_Write_Byte(handle,0x00e6,0x01);
		VL6180_Write_Byte(handle,0x00e7,0x03);
		VL6180_Write_Byte(handle,0x00f5,0x02);
		VL6180_Write_Byte(handle,0x00d9,0x05);
		VL6180_Write_Byte(handle,0x00db,0xce);
		VL6180_Write_Byte(handle,0x00dc,0x03);
		VL6180_Write_Byte(handle,0x00dd,0xf8);
		VL6180_Write_Byte(handle,0x009f,0x00);
		VL6180_Write_Byte(handle,0x00a3,0x3c);
		VL6180_Write_Byte(handle,0x00b7,0x00);
		VL6180_Write_Byte(handle,0x00bb,0x3c);
		VL6180_Write_Byte(handle,0x00b2,0x09);
		VL6180_Write_Byte(handle,0x00ca,0x09);
		VL6180_Write_Byte(handle,0x0198,0x01);
		VL6180_Write_Byte(handle,0x01b0,0x17);
		VL6180_Write_Byte(handle,0x01ad,0x00);
		VL6180_Write_Byte(handle,0x00ff,0x05);
		VL6180_Write_Byte(handle,0x0100,0x05);
		VL6180_Write_Byte(handle,0x0199,0x05);
		VL6180_Write_Byte(handle,0x01a6,0x1b);
		VL6180_Write_Byte(handle,0x01ac,0x3e);
		VL6180_Write_Byte(handle,0x01a7,0x1f);
		VL6180_Write_Byte(handle,0x0030,0x00);

		// Recommended : Public registers - See data sheet for more detail
		VL6180_Write_Byte(handle,VL6180X_SYSTEM__MODE_GPIO1, 0x10); // Enables polling for ‘New Sample ready’
												// when measurement completes
		VL6180_Write_Byte(handle,VL6180X_READOUT__AVERAGING_SAMPLE_PERIOD, 0x30); // Set the averaging sample period
												// (compromise between lower noise and
												// increased execution time)
		VL6180_Write_Byte(handle,VL6180X_SYSALS__ANALOGUE_GAIN, 0x46); // Sets the light and dark gain (upper
												// nibble). Dark gain should not be
												// changed.
		VL6180_Write_Byte(handle,VL6180X_SYSRANGE__VHV_REPEAT_RATE, 0xFF); // sets the # of range measurements after
												// which auto calibration of system is
												// performed
		VL6180_Write_Byte(handle,VL6180X_SYSALS__INTEGRATION_PERIOD, 0x63); // Set ALS integration time to 100ms
		VL6180_Write_Byte(handle,VL6180X_SYSRANGE__VHV_RECALIBRATE, 0x01); // perform a single temperature calibration
												// of the ranging handle
		//Optional: Public registers - See data sheet for more detail
		VL6180_Write_Byte(handle,VL6180X_SYSRANGE__INTERMEASUREMENT_PERIOD, 0x06); // Set default ranging inter-measurement
												// period to 70ms
		VL6180_Write_Byte(handle,VL6180X_SYSALS__INTERMEASUREMENT_PERIOD, 0x31); // Set default ALS inter-measurement period
												// to 500ms
		VL6180_Write_Byte(handle,VL6180X_SYSTEM__INTERRUPT_CONFIG_GPIO, 0x24); // Configures interrupt on ‘New Sample
												// Ready threshold event’
		VL6180_Write_Byte(handle,VL6180X_SYSTEM__FRESH_OUT_OF_RESET,0x00); //change fresh out of set status to 0
	//	VL6180_Write_Byte(handle,VL6180X_SYSRANGE__START,0x01); //Make sure range scanning disabled
		}



	status  = VL6180_Read_Byte(handle,VL6180X_IDENTIFICATION__MODEL_ID,&(handle->_id.Model_Id));
	status &= VL6180_Read_Byte(handle,VL6180X_IDENTIFICATION__MODEL_REV_MAJOR,&(handle->_id.Model_Rev_Major));
	status &= VL6180_Read_Byte(handle,VL6180X_IDENTIFICATION__MODEL_REV_MINOR,&(handle->_id.Model_Rev_Minor));
	status &= VL6180_Read_Byte(handle,VL6180X_IDENTIFICATION__MODULE_REV_MAJOR,&(handle->_id.Module_Rev_Major));
	status &= VL6180_Read_Byte(handle,VL6180X_IDENTIFICATION__MODULE_REV_MINOR,&(handle->_id.Module_Rev_Minor));
	status &= VL6180_Read_Byte(handle,VL6180X_IDENTIFICATION__DATE_HI,&date_hi);
	status &= VL6180_Read_Byte(handle,VL6180X_IDENTIFICATION__DATE_LO,&date_lo);
	status &= VL6180_Read_Byte(handle,VL6180X_IDENTIFICATION__TIME_HI,&time_hi);
	status &= VL6180_Read_Byte(handle,VL6180X_IDENTIFICATION__TIME_LO,&time_lo);

	if(status == true)
	{
		handle->_id.Date = (date_hi<<8) + date_lo;
		handle->_id.Time = (time_hi<<8) + time_lo;
	}

	return status;
}



bool VL6180_Read_Byte(VL6180X_Handle sensor, uint16_t reg_addr, uint8_t *data)
{
	I2C_Transaction  i2cTransaction;
	unsigned char	 writeBuffer[2];
	bool status;

	writeBuffer[0]=(reg_addr >> 8) & 0xFF;
	writeBuffer[1]=reg_addr  & 0xFF;

	i2cTransaction.slaveAddress = sensor->_address;     /* 7-bit peripheral slave address */
	i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
	i2cTransaction.writeCount = 2;          /* Number of bytes to be written */
	i2cTransaction.readBuf = data;    /* Buffer to be read */
	i2cTransaction.readCount = 1;           /* Number of bytes to be read */

	status =  I2C_transfer(sensor->_handle_i2c, &i2cTransaction); /* Perform I2C transfer */
	return status;
}

bool VL6180_Write_Byte(VL6180X_Handle sensor, uint16_t reg_addr, uint8_t data)
{
	I2C_Transaction  i2cTransaction;
	unsigned char	 writeBuffer[3];
	bool status;

	writeBuffer[0]=(reg_addr >> 8) & 0xFF;
	writeBuffer[1]=reg_addr  & 0xFF;
	writeBuffer[2]=data  & 0xFF;

	i2cTransaction.slaveAddress = sensor->_address;     /* 7-bit peripheral slave address */
	i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
	i2cTransaction.writeCount = 3;          /* Number of bytes to be written */
	i2cTransaction.readBuf = NULL;    /* Buffer to be read */
	i2cTransaction.readCount = 0;           /* Number of bytes to be read */

	status = I2C_transfer(sensor->_handle_i2c, &i2cTransaction); /* Perform I2C transfer */

	return status;
}



bool VL6180_Write(VL6180X_Handle sensor, uint16_t reg_addr, uint8_t *writeBuffer,uint16_t writeCount)
{
	I2C_Transaction  i2cTransaction;

	writeBuffer[0]=(reg_addr >> 8) & 0xFF;
	writeBuffer[1]=reg_addr  & 0xFF;

	i2cTransaction.slaveAddress = sensor->_address;     /* 7-bit peripheral slave address */
	i2cTransaction.writeBuf = writeBuffer;  /* Buffer to be written */
	i2cTransaction.writeCount = writeCount;          /* Number of bytes to be written */
	i2cTransaction.readBuf = NULL;    /* Buffer to be read */
	i2cTransaction.readCount = 0;           /* Number of bytes to be read */

	return I2C_transfer(sensor->_handle_i2c, &i2cTransaction); /* Perform I2C transfer */
}



bool VL6180_StopContinuous(VL6180X_Handle sensor)
{
	return VL6180_Write_Byte(sensor,VL6180X_SYSRANGE__START,0);
}


bool VL6180_StartContinuous(VL6180X_Handle sensor)
{
	return VL6180_Write_Byte(sensor,VL6180X_SYSRANGE__START,VL6180X_CONTINUOUS);
}


bool VL6180_StartSingleShot(VL6180X_Handle sensor)
{
	return VL6180_Write_Byte(sensor,VL6180X_SYSRANGE__START,VL6180X_SINGLE);
}


///////////////////////////////////////////////////////////////////
// poll for new sample ready ready//
/////////////////////////////////////////////////////////////////
bool VL6180X_Poll_Range(VL6180X_Handle sensor,uint8_t *range_status)
{
	uint8_t status;

	// check the status
	VL6180_Read_Byte(sensor,VL6180X_RESULT__INTERRUPT_STATUS_GPIO,&status);
	*range_status =status & 0x07;

	return 0;
}

bool VL6180X_Read_Range(VL6180X_Handle sensor,uint8_t *data)
{
	unsigned int key;

	bool status;
//	key = GateMutex_enter(sensor->dataAccess);
	status=VL6180_Read_Byte(sensor,0x062,&(sensor->data));
    *data=sensor->data;
   // GateMutex_leave(sensor->dataAccess, key);


	status&=VL6180X_Clear_Interrupts(sensor);

	return status;
}

bool VL6180X_Clear_Interrupts(VL6180X_Handle sensor)
{
	return VL6180_Write_Byte(sensor,0x015,0x07);
}

bool VL6180X_Set_Addr(VL6180X_Handle sensor, uint8_t new_addr)
{
	uint8_t ID;

	VL6180_Write_Byte(sensor, VL6180X_I2C_SLAVE__DEVICE_ADDRESS,new_addr);

	sensor->_address = new_addr;
	VL6180_Read_Byte(sensor,VL6180X_IDENTIFICATION__MODEL_ID,&ID);
	return (ID == 0xB4);

}


bool VL6180X_Check_Addr(VL6180X_Handle sensor)
{

	uint8_t ID;

	VL6180_Read_Byte(sensor,VL6180X_IDENTIFICATION__MODEL_ID,&ID);
	return (ID == 0xB4);
}
