/*
 * prox.c
 *
 *  Created on: Apr 3, 2016
 *      Author: u400
 */





//void proximityFxn(UArg arg0, UArg arg1)
//{
//	while(!PROX){
//			Task_sleep((UInt)5);
//		}
//
//
//	uint64_t ts;				//Timestamp
//	uint8_t range=0;			//Range reading
//	proximityDataObj proximityMsg;	//Message to be communicated
//	Error_Block eb;				//Error block
//	uint8_t range_status;		//status of sensor
//	int old_priority;
//	bool newDat=false;
//	int key;
//	int cnt=0;
//
//
//	System_printf(" Started Proximity Task \n\r");
//	System_flush();
////	old_priority = Task_setPri(proximityTask,4);
////	key = Hwi_disable();
//
//	//Disable sensor 2
////	GPIO_write(VL6180X_EN, GPIO_CFG_OUT_LOW);
//	Task_sleep((UInt)5);
//	//Create proximity handles
//	ProximitySensor0 =  VL6180_Create(0,0x29,handle_i2c0);
//	ProximitySensor1 =  VL6180_Create(1,0x29,handle_i2c1);
//
//	//Check sensor 1 address (0x2A) is correct, if not change
////	if(VL6180X_Check_Addr(ProximitySensor0)==false)
////	{	//Sensor is still 0x29
////		ProximitySensor0->_address=0x029;
////		VL6180X_Set_Addr(ProximitySensor0,(uint8_t)0x002A);
////	}
//
//	//Enable sensor 2 (0x29)
////	GPIO_write(VL6180X_EN, GPIO_CFG_OUT_HIGH);
////	Task_sleep((UInt)5);
//
//
//	VL6180_StopContinuous(ProximitySensor0);
//	Task_sleep(5);
//	VL6180_StopContinuous(ProximitySensor1);
//	Task_sleep((UInt)5);
//
//
//	//Initialiize sensors
//	VL6180_Init(ProximitySensor0);
//	Task_sleep(5);
//	VL6180_Init(ProximitySensor1);
//	Task_sleep(5);
//	//Start scanning
//	VL6180_StartContinuous(ProximitySensor0);
//	Task_sleep(5);
//	VL6180_StartContinuous(ProximitySensor1);
////	Task_sleep((UInt)5);
////	Task_setPri(proximityTask,old_priority);
//	int i = 0;
//	while (1)
//	{
//		newDat=false;
//		//Poll sensor 1
//		VL6180X_Poll_Range(ProximitySensor0,&range_status);
//		if(range_status ==0x04)
//		{
//			ts=Timestamp_get32();	//get timestamp
//			VL6180X_Read_Range(ProximitySensor0,&range);	//Read value
//
//			//Compose messsage
//			proximityMsg.buf=circ_size(&cBuf);
//			proximityMsg.ts=((ts ) * 1000)/freq.lo;
//			proximityMsg.dat[0]=range;
//			proximityMsg.src = prx0;
//			System_printf("PX0 Range %d  \n",range);
//		    System_flush();
//			//Place message on queue
//			//Added to SD buffer
////			key = GateMutex_enter(cBufAccess);		//Lock buffer
//			circ_add(&cBuf,proximityMsg);			//Insert
////		    GateMutex_leave(cBufAccess, key);		//Release buffer
//
//			newDat |= true;
//			i++;
//		}
//		else{
//			System_printf("re Init Prox0  \n");
//					    System_flush();
//			VL6180_StopContinuous(ProximitySensor0);
//			Task_sleep((UInt)50);
//			VL6180_Init(ProximitySensor0);
//			Task_sleep((UInt)50);
//			VL6180_StartContinuous(ProximitySensor0);
//		}
//
//		//Poll sensor 2
//		VL6180X_Poll_Range(ProximitySensor1,&range_status);
//		if(range_status ==0x04)
//		{
//			GPIO_write(LED_RED,false);
//			ts=Timestamp_get32();	//get timestamp
//			VL6180X_Read_Range(ProximitySensor1,&range);	//Read value
//
//			//Compose messsage
//			proximityMsg.buf=circ_size(&cBuf);
//			proximityMsg.ts=((ts ) * 1000)/freq.lo;
//			proximityMsg.dat[0]=range;
//			proximityMsg.src=prx1;
//			System_printf("PX1 Range %d  \n",range);
//		    System_flush();
//			//Place message on queue
//			//Added to SD buffer
//
//			circ_add(&cBuf,proximityMsg);			//Insert
//
//
//			newDat |= true;
//		}
//		else{
//			System_printf("re Init Prox1  \n");
//					    System_flush();
//			VL6180_StopContinuous(ProximitySensor1);
//			Task_sleep((UInt)50);
//			VL6180_Init(ProximitySensor1);
//			Task_sleep((UInt)50);
//			VL6180_StartContinuous(ProximitySensor1);
//		}
//		Task_sleep((UInt)5);
//		//Signal SD data ready
////		if(newDat)
////			Semaphore_post(sdSemaphore);
////		else
////			cnt++;
////		if(i%300==299){
////		System_printf(" package%d   \n\r",i);
////				    System_flush();
////		}
//	//	 System_flush();
//
//	}
//}
/*
 *  ======== IMU  taskFxn ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
//Void taskFxn(UArg arg0, UArg arg1)
//{
//
//
//	while(!ACCER){
//				Task_sleep((UInt)5);
//			}
//    char input;
//
//
//	uint64_t ts;					//timestamp
//	sdDataObj sdMsg;				//SD message
//	MPU9150_Data 	accData,gyData;	//IMU Data storage
//	int key;						//cBuf access key
//	int cnt=0;
//
//
//    //THIS MIGHT BE THE ISSUE LOOK UP IN DATASHEET~ needed to be added to board file
////	/* Use the 1st instance of MPU9150 */
//		GPIO_setCallback(MPU9150_INT_PIN, gpioMPU9150DataReady);
//		GPIO_enableInt(MPU9150_INT_PIN);
////
////		System_printf("After callback");
////			System_flush();
//////		//Initialize IMUs
//	mpu0 = MPU9150_init(0, handle_i2c1, I2C_MPU9150_ADDR0);
//	mpu1 = MPU9150_init(1, handle_i2c1, I2C_MPU9150_ADDR1);
//
//
//		if (mpu0 == NULL){
//			System_printf("MPU9150_0 could not be initialized");
//		    System_flush();
//
//		}
//		if (mpu1 == NULL){
//			System_printf("MPU9150_1 could not be initialized");
//			System_flush();
//		}
//		//Enable interrrups
//			MPU9150_Int(mpu0);
//			MPU9150_Int(mpu1);
//
//			Hwi_enableInterrupt(16);
//
//		System_printf("\nBefore while");
//				System_flush();
//
//				int i=0;
//			while (1)
//					{
//
////						System_printf("While loop task!\n");
////						  System_flush();
//
//						MPU9150_read(mpu0,ts);					//Record Data
//						MPU9150_getAccelRaw(mpu0, &accData);	//Get Accelerometer Data
////						MPU9150_getGyroRaw(mpu0, &gyData);		//Get Gyro Data
//
//						//Compose SD message
//								sdMsg.buf=circ_size(&cBuf);
//								sdMsg.ts=ts;
//								sdMsg.src=imu0;
//								sdMsg.dat[0]=accData.x;
//								sdMsg.dat[1]=accData.y;
//								sdMsg.dat[2]=accData.z;
//								sdMsg.dat[3]=gyData.x;
//								sdMsg.dat[4]=gyData.y;
//								sdMsg.dat[5]=gyData.z;
//
//								//Added to SD buffer
//							//	key = GateMutex_enter(cBufAccess);		//Lock buffer
//								circ_add(&cBuf,sdMsg);					//Insert
//							  //  GateMutex_leave(cBufAccess, key);		//Release buffer
//				System_printf(" Accer0 %d  %d %d \n", accData.x, accData.y, accData.z);
//				System_flush();
//						MPU9150_read(mpu1,ts);					//Record Data
//						MPU9150_getAccelRaw(mpu1, &accData);	//Get Accelerometer Data
////						MPU9150_getGyroRaw(mpu1, &gyData);		//Get Gyro Data
//
//						//Compose SD message
//								sdMsg.buf=circ_size(&cBuf);
//								sdMsg.ts=ts;
//								sdMsg.src=imu1;
//								sdMsg.dat[0]=accData.x;
//								sdMsg.dat[1]=accData.y;
//								sdMsg.dat[2]=accData.z;
//								sdMsg.dat[3]=gyData.x;
//								sdMsg.dat[4]=gyData.y;
//								sdMsg.dat[5]=gyData.z;
//
//								//Added to SD buffer
//							//	key = GateMutex_enter(cBufAccess);		//Lock buffer
//								circ_add(&cBuf,sdMsg);					//Insert
//							  //  GateMutex_leave(cBufAccess, key);		//Release buffer
//
//					if(i%300==299){
//							System_printf(" package%d   \n\r",i);
//							System_flush();
//						}
//					i++;
//						System_printf(" Accer1 %d  %d %d \n ", accData.x, accData.y, accData.z);
//						System_flush();
//					  Task_sleep((UInt)5);
//
//					}
//
//
////			int newVal =  k1*angle[]
//
//}
