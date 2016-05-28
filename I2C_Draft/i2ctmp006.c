
#include "headers.h"
//#include "driverlib/gpio.c"
//*******Callbacks

void gpioMPU9150DataReady(void)
{
	GPIO_clearInt(MPU9150_INT_PIN);
	Semaphore_post(imuSemaphore);
}
void PrxDataReady(void)
{
	GPIO_clearInt(VL6180X_INT_PIN1);
	//Semaphore_post(imuSemaphore);
}

char j=0;
// can't go lower than 0
int currentPos = 800;
int DesPos = 800;
int direction=0;




FIL logFile;

/********************************************************
 * Declarations
 *
 */
//Version
#define VMAJOR 0
#define VMINOR 1
#define Board_I2C_MPU9150 0
#define I2C_MPU9150_ADDR0 0x68
#define I2C_MPU9150_ADDR1 0x69

/* Drive number used for FatFs */
#define DRIVE_NUM           0
/* String conversion macro */
#define STR_(n)             #n
#define STR(n)              STR_(n)
#define PWM_FREQUENCY 100

//extern const Semaphore_Handle imuSemaphore;
//extern const Semaphore_Handle proximitySemaphore;
//extern const Semaphore_Handle sdSemaphore;
//extern const Queue_Handle proximityQueue;
//extern const Queue_Handle sdQueue;
//GateMutex_Handle cBufAccess;

//IMU Handles
MPU9150_Handle mpu0;
MPU9150_Handle mpu1;
//GLOBAL FLAGS
int ACCER = 0;
int SD = 0;
int PROX = 0;
int BNO = 1;
int ENCODER = 0;
int SEND_TO_CABLE = 0;
int angle = 0;
int p_angle[4];
int oldavg=0;
int newavg=0;
int CalibrationOffset = 0;
int Velocity = 0;//Holds speed in quarter turns
int speed = 1557;//speed offset used for ESC speed

vector degs;
int dir_angle = 0;
//I2C Handles
I2C_Handle	handle_i2c0; //IMU I2C
I2C_Handle	handle_i2c1; //IMU I2C


/// PWM VARIABLES
uint32_t ui32Load;
uint32_t ui32PWMClock;

//Proximity Sensor Handles
VL6180X_Handle ProximitySensor0;
VL6180X_Handle ProximitySensor1;
MotorObject motor;

//UART Handles
UART_Handle 	 handle_uart0;	// XBee
UART_Handle      handle_uart1;	//usb cable


BNO55_Handle bno;
// buffers
cir_buffer cBuf;
cir_buffer cBuf2;
int lastAngle;
void gpioButtonFxn0(void)
{
    /* Clear the GPIO interrupt and toggle an LED */
    GPIO_toggle(Board_LED0);
    	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,0);
    	CalibrationOffset =lastAngle;
    	speed-=3;
    	if(speed<=1560){
    		speed=1560;
    	}
//    	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, 1000 * ui32Load / 10000);
//    	PWM1_BASE, PWM_OUT_0
    	Task_sleep((UInt)200);

    direction = 1;

}



//middle button on pcb
/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON1.
 *  This may not be used for all boards.
 */
void gpioButtonFxn1(void)
{
    /* Clear the GPIO interrupt and toggle an LED */
    GPIO_toggle(Board_LED1);
	GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_PIN_2);
	speed+=3;
	if(speed>=1600){
		speed=1600;
	}
//	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, 2000 * ui32Load / 10000);
	Task_sleep((UInt)200);

	direction = 0;



}

void BNOTaskFxn(UArg arg0, UArg arg1){

	while(!BNO){
//		System_printf("MPU9150_0 could not be initialized");
//		System_flush();
		Task_sleep((UInt)200);
	}

	bno =  BNO55_Create( 0,  0x28, handle_i2c0);

	char output[100];				//Output string
	int len;



	while(BNO55_init(bno)==0){
		Task_sleep((UInt)100);

	};
//	setExtCrystalUse(bno,true);
//	Task_sleep((UInt)150);
//	setMode( bno, OPERATION_MODE_IMUPLUS);
	Task_sleep((UInt)15);

	degs.x=1;
	degs.y=2;
	degs.z=3;
	short t=2;
	int id;
	int cal=0;

	while(1){
		uint8_t config=135;
		getVector(bno,&degs,0x1A);
		if(cal==0&& degs.z !=0){
			CalibrationOffset = degs.z;
			cal=1;
		}

		if(SEND_TO_CABLE){
			len = System_sprintf(output,"Orientation: %d.0 %d.0 %d.0\n",degs.x,degs.z,degs.y);

			UART_write(handle_uart1,output,len);


		}

	angle = degs.z- CalibrationOffset;
	lastAngle = angle;
	System_printf("Orientation: %d %d %d \n",degs.x,angle,degs.y);
	System_flush();
	p_angle[0] =p_angle[1];
	p_angle[1] =p_angle[2];
	oldavg = newavg;
//	Task_sleep((UInt)50);+ p_angle[0]  + p_angle[1]


	//	angle

	p_angle[2] =p_angle[3];
	p_angle[3] =angle;

	newavg = (p_angle[3]) + p_angle[2])/2;



	}
}


/*
 * Encoder Task that keeps track of wheel rotations
 */
void EncoderFxn(UArg arg0, UArg arg1)
{

	while(!ENCODER){
//		System_printf("encoder could not be initialized");
//		System_flush();
		Task_sleep((UInt)5);
	}
	//Get frequency
	Types_FreqHz freq;
	Timestamp_getFreq(&freq);
	int a=0;
	int i=0;

		int  POS=QEIPositionGet(QEI1_BASE);
	 int PrevPos=0;
		 int CurrPos =0;
		 Types_Timestamp64 result;
	 while(1){

		int o=QEIPositionGet(QEI1_BASE);
		PrevPos =CurrPos;
		CurrPos =QEIPositionGet(QEI1_BASE);
		Velocity = (CurrPos-PrevPos)/2; //number of quarter turns since charlie addded magnets


		i=(Timestamp_get32()*1000)		/freq.lo;
		  Timestamp_get64(&result);

		  unsigned long now=  (result.lo/(  freq.lo/1000 ))+(53000)*result.hi;
		  System_printf("Time %d, Steps:%d Corrected Velocity(Ticks per Sec):%d \n", now, CurrPos,Velocity);
		POS=o;
		if(SEND_TO_CABLE){
			char output[50];
		 int len = System_sprintf(output,"Speed in Quarter Turns: %d\n",Velocity);

			UART_write(handle_uart1,output,len);

		}

		Task_sleep((UInt)1000);
		  Timestamp_get64(&result);

		now=  (result.lo/(  freq.lo/1000 ))+(53000)*result.hi;
		  System_printf("2ND Time %d, Steps:%d Corrected Velocity(Ticks per Sec):%d \n", now, CurrPos,Velocity);
	  		System_flush();

		i=(i-(Timestamp_get32()*1000)		/freq.lo);


	}
}

/*
* Interrupt handler for calibration of counter weight system
*/
void CalibrationHandler(void) {

   GPIO_toggle(Board_LED0);
   currentPos = 800;

}




//
double k1=.7711;
double k2=-.0302;
//int angle[10]={12,10,8,4,2,9,-4,3,2,5};
//double roll[]={2/.1,2/.1,4/.1,7/.1,13/.1,7/.1,1/.1,3/.1};

int index=0;
int jk=0;
int limit = 15;
int pos = GPIO_PIN_2;

/*
 * Calculates a new position for the counter weight system change period in cfg file.
 *
 * */
void CalcNewPos(){

	//	angle
//	if(p_angle[3]>=3)
//		DesPos= (16)*(p_angle[3])+800;
//	if(p_angle[3]<=-3)
//		DesPos = (16)*(p_angle[3])+800;
////		p_angle[3] =p_angle[1];
	if(DesPos>=1600){
		DesPos=1600;
	}
	if(DesPos<=0){
		DesPos=0;
	}


//		p_angle[1] =p_angle[2];
//		p_angle[2] =p_angle[3];
//		p_angle[3] =array2[(jk++)%16];
//		p_angle[0] = 0;
//		p_angle[1] = 1;
//		p_angle[2] = 4;
//		oldavg = newavg;
//		newPos = ((1600)* p_angle[0])/90;
//		newavg = (p_angle[0] + p_angle[1] + p_angle[2] + p_angle[3])/4;
//	double newPos =( -k1*p_angle[3] - (k2/2)*(oldavg-newavg))*157.14;
//	DesPos =((int) newPos)+900;
//	index = ((index+1)%10);
}


char output[10];
/*
 * Task used to change the speed of the ESC for the bike
 * changes the speed depending on characters read from XBEE or
 * from a button
 *
 */
void EscControl(){
//	int len = UART_readPolling(handle_uart1,output,1);
//		output[len]='\0';
//		System_printf(output);
//		System_flush();
//	if(output[0]=='f')
//		{
//			PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, speed * ui32Load / 10000);
//			speed+=3;
//	}
//	else if(output[0]=='s'){
//			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, speed * ui32Load / 10000);
//			speed-=3;
//	}
//			System_printf(" PWMed  \n\r");
//			System_flush();
			Task_sleep((UInt)1000);
//
}

/*
* Task used to move stepper motor of Counter weight system
* Period has to be changed in cfg file
*/
void myTimerFunc(){

	//angle is from -90 to 90 map to 0 and
//	DesPos = (angle+90)*9;
	if(DesPos<=currentPos){
		direction  = 0;
	}
	else {
		direction  = 1;
	}
//	if(currentPos==1600){
//		direction =0;
//		DesPos=0;
//	}
//	else if (currentPos==0){
//		DesPos=1600;
//		direction=1;
//	}


	MotorObject* motorHandle = &motor;
	  if (direction == 1)
		{
		  if(currentPos!=DesPos )
			  currentPos++;

		}
		else
		{
			if(currentPos!=DesPos)
				currentPos--;

		  motorHandle->step_number--;
		}

	switch (currentPos%4) {
	   case 0:  // 1010
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_1,motorHandle->motor_pin_1);
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_2,LOW);
	  GPIOPinWrite(GPIO_PORTD_BASE, motorHandle->motor_pin_3,motorHandle->motor_pin_3);
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_4,LOW);
	  break;
	  case 1:  // 0110
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_1,LOW);
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_2,motorHandle->motor_pin_2);
	  GPIOPinWrite(GPIO_PORTD_BASE, motorHandle->motor_pin_3,motorHandle->motor_pin_3);
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_4,LOW);
	  break;
	  case 2:  //0101
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_1,LOW);
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_2,motorHandle->motor_pin_2);
	  GPIOPinWrite(GPIO_PORTD_BASE, motorHandle->motor_pin_3,LOW);
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_4,motorHandle->motor_pin_4);
	  break;
	  case 3:  //1001
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_1,motorHandle->motor_pin_1);
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_2,LOW);
	  GPIOPinWrite(GPIO_PORTD_BASE, motorHandle->motor_pin_3,LOW);
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_4,motorHandle->motor_pin_4);
	  break;
  }

}


//Task used for writing to SD card.
// Period of writing can be modified by changing sleep duration
void SDTaskFxn(UArg arg0, UArg arg1){

	while(!SD){
			Task_sleep((UInt)50);
		}
		FRESULT fresult;
		SDSPI_Handle sdspiHandle;
		SDSPI_Params sdspiParams;
		int i,len;
		char	fileName[20];			//Filename of logfile
		sdDataObj sdMsg;				//Temporary sdMessage storage
		char output[100];				//Output string
		unsigned int bytesWritten = 0;	//Bytes written to sd card
		int key;						//Buffer access key
		bool hasData;

		// Mount and register the SD Card
		SDSPI_Params_init(&sdspiParams);
		sdspiHandle = SDSPI_open(Board_SDSPI0, DRIVE_NUM, &sdspiParams);
		if (sdspiHandle == NULL){
			System_abort("Error starting the SD card\n");
		}
		else{
			System_printf("Drive %u is mounted\n", DRIVE_NUM);
		}

//		 Try to open the source file
		for(i=100;i<5000;i++)
		{
			System_sprintf(fileName,"Dat_%d.txt",i);
			fresult = f_open(&logFile, fileName, FA_CREATE_NEW | FA_WRITE);
			if( fresult == FR_OK )
				break;;
		}
//
//		//Print version header
		len=System_sprintf(output,"MotoSense V%d.%d\n",VMAJOR,VMINOR);
		f_write(&logFile, output, len, &bytesWritten);
		f_sync(&logFile);
		i=0;
	while(1){


		hasData=true;
		while(hasData)
		{
			//key = GateMutex_enter(cBufAccess);	//Lock buffer
			hasData = circ_rem(&cBuf, &sdMsg);	//Read Data
			//GateMutex_leave(cBufAccess, key);	//Release Buffer
			//System_printf("\r\nSD task");
			//Transfer data to second buffer
			if(hasData)
			{
				circ_add(&cBuf2,sdMsg);
//				switch(sdMsg.src)
//				{
//				case prx0:
//					len=System_printf("prx0,%d,%d\n",sdMsg.ts,	sdMsg.dat[0]);
//					break;
//				case prx1:
//					len=System_printf("prx1,%d,%d\n",sdMsg.ts,	sdMsg.dat[0]);
//					break;
//				case imu0:
//					len=System_printf("imu0,%d,%d,%d,%d,%d,%d,%d\n",(short)sdMsg.ts,
//							(short)sdMsg.dat[0],(short)sdMsg.dat[1],(short)sdMsg.dat[2],
//							(short)sdMsg.dat[3],(short)sdMsg.dat[4],(short)sdMsg.dat[5]);
//					break;
//				case imu1:
//					len=System_printf("imu1,%d,%d,%d,%d,%d,%d,%d\n",(short)sdMsg.ts,
//							(short)sdMsg.dat[0],(short)sdMsg.dat[1],(short)sdMsg.dat[2],
//							(short)sdMsg.dat[3],(short)sdMsg.dat[4],(short)sdMsg.dat[5]);
//					break;
//				default:
//					break;
//				}
				//UART_write(handle_uart3,output,len);
				 // System_flush();
			}
			if(circ_size(&cBuf2)==128)
			{
				f_write(&logFile, (char*) &cBuf2.sdBuffer, sizeof(sdDataObj)*128 , &bytesWritten);
				f_sync(&logFile);
				circ_init(&cBuf2);

			}



		  Task_sleep((UInt)2);
		}
	}
}

double conv = .439822;// distance m per revolution
/*
 *  Main.
 *  Sets up pins and settings for all other tasks.
 *  */
int main(void)
{
	int i;				//Loop counter
		uint64_t cnt=0;		//Used for sck toggle
		Error_Block eb;		//Error block
    /* Call board init functions */
	Board_initGeneral();
	Board_initGPIO();
	Board_initSDSPI();
	Board_initUART();
	Board_initI2C();

//	Task_Params taskParams;
//	 Task_Handle myTsk0, myTski;
//	 Clock_Params clockParams;
//	 Clock_Handle myClk0, myClk1;
//	 Error_Block eb;
//	 UInt i;

    /* Turn on user LED */

    System_printf("Starting MAIN \n");
    System_flush();

	/****************************************/
	/*Initialize I2c */
	/****************************************/
	I2C_Params	params_i2c;
	I2C_Params_init(&params_i2c);
	params_i2c.bitRate=I2C_400kHz;
	params_i2c.transferMode=I2C_MODE_BLOCKING;
//
	handle_i2c0 = I2C_open(0, &params_i2c);
	if (handle_i2c0 == NULL) {
		System_printf("I2C0 (IMU) did not open");
	}
	//Initialize circular buffers and access mutex
    circ_init(&cBuf);
    circ_init(&cBuf2);
    cBufAccess = GateMutex_create(NULL, &eb);
//
	handle_i2c1 = I2C_open(1, &params_i2c);
	if (handle_i2c1 == NULL) {
		System_printf("I2C1 (Proximity) did not open");
	}
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();
    /* install Button callback */
    GPIO_setCallback(Board_BUTTON0, gpioButtonFxn0);
    GPIO_setCallback(Board_BUTTON1, gpioButtonFxn1);

    /* Enable interrupts */
    GPIO_enableInt(Board_BUTTON0);
    GPIO_enableInt(Board_BUTTON1);

    /****************************************/
   	/* Reset Pin
   	/****************************************/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_4,GPIO_PIN_4);


	 /****************************************/
	/*Initialize Motor Driver Pins */
	/****************************************/

    	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3);
    	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
        GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);

    	motor.ui32Port = GPIO_PORTE_BASE;
    	motor.motor_pin_1=GPIO_PIN_0;
    	motor.motor_pin_2=GPIO_PIN_1;
    	motor.motor_pin_3=GPIO_PIN_1;//PD1
    	motor.motor_pin_4=GPIO_PIN_3;
    	Motor_init(0,&motor,400,freq);
    	setSpeed(1,&motor);

    	 /****************************************/
		/*Initialize Calibration Pin */
		/****************************************/

    	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
        GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_7);
        GPIO_setCallback(Board_CalibartionPIN, CalibrationHandler);
          /* Enable interrupts */
          GPIO_enableInt(Board_CalibartionPIN);

	  /****************************************/
		/*Initialize UART */
		/****************************************/
		/*Initialize UART1*/
		UART_Params      params_uart;
		UART_Params_init(&params_uart);
		params_uart.baudRate  = 9600;//115200;
		params_uart.writeDataMode = UART_DATA_TEXT;
		params_uart.readDataMode = UART_DATA_TEXT;
		params_uart.readReturnMode = UART_RETURN_FULL;
		params_uart.readEcho = UART_ECHO_OFF;

		handle_uart1 = UART_open(EK_TM4C123GXL_UART1, &params_uart);
		//UART1 main
		if (!handle_uart1) {
			System_printf("UART did not open");
		}
		/*Initialize UART0*/
		UART_Params uartParams;
		/* Create a UART with data processing off. */
		UART_Params_init(&uartParams);
		uartParams.writeDataMode = UART_DATA_TEXT;
		uartParams.readDataMode = UART_DATA_TEXT;
		uartParams.readReturnMode = UART_RETURN_FULL;
		uartParams.readEcho = UART_ECHO_OFF;
		uartParams.baudRate = 9600;
//		handle_uart0 = UART_open(EK_TM4C123GXL_UART0, &uartParams);
//		if (handle_uart0 == NULL) {
//		   System_abort("Error opening the UART");
//		}
		char output[100];
		int 	len = System_sprintf(output,"Uart Start \n\r");

//		/UART_write(handle_uart1,output,len);
//		UART_write(handle_uart0,output,len);

		 /****************************************/
		/*Initialize PWM */
		/****************************************/
		/*PWM0 MODULE3 STEERING*/
		SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

		SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_5);
		GPIOPinConfigure(GPIO_PB5_M0PWM3);

		ui32PWMClock = SysCtlClockGet() / 64;
		ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
		PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
		PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, ui32Load);

		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, 1500 * ui32Load / 10000);
		PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);
		PWMGenEnable(PWM0_BASE, PWM_GEN_1);

		/*PWM1 MODULE0 ESC */
		SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
		GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
		GPIOPinConfigure(GPIO_PD0_M1PWM0);
		PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
		PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);
//		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, 1542 * ui32Load / 10000);
				PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, 1542 * ui32Load / 10000);

		PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
		PWMGenEnable(PWM1_BASE, PWM_GEN_0);
		 /****************************************/
		/*Initialize QEI */
		/****************************************/
		// C4,C5,C6
		SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI1);
		// Set up Alternate function of pins
		GPIOPinConfigure(GPIO_PC4_IDX1);// -GND
		GPIOPinConfigure(GPIO_PC5_PHA1),//-PULLDOWN
		GPIOPinConfigure(GPIO_PC6_PHB1);//- GND
		GPIODirModeSet(GPIO_PORTC_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6,GPIO_DIR_MODE_HW);
		// 4 and 6 pull down
		GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_6, GPIO_STRENGTH_2MA,
					  GPIO_PIN_TYPE_STD_WPD);
		//pin 5 pull up since that is what worked before
		GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA,
					  GPIO_PIN_TYPE_STD_WPU);
		//Set up pins as correct config
		//	 GPIOPinTypeQEI(GPIO_PORTC_BASE,GPIO_PIN_5);
		QEIConfigure(QEI1_BASE, (QEI_CONFIG_CAPTURE_A | QEI_CONFIG_NO_RESET |
		QEI_CONFIG_CLOCK_DIR | QEI_CONFIG_NO_SWAP), 65535);
		QEIEnable(QEI1_BASE);
		QEIPositionGet(QEI1_BASE);

		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_2);
		GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_PIN_2);
		QEIVelocityEnable(QEI1_BASE);
		//100 us clock tics. 1 sec  =
		QEIVelocityConfigure(QEI1_BASE,  QEI_VELDIV_1, 80000000);//.1 sec



    /* Start BIOS */
    BIOS_start();
    System_printf("afte bios \n");
    return (0);
}


