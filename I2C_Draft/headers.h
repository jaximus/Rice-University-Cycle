/*
 * headers.h
 *
 *  Created on: Nov 5, 2015
 *      Author: raul
 */

#ifndef HEADERS_H_
#define HEADERS_H_




#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h> //for Log_info() calls when UIA is added
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/timestamp.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/fatfs/ff.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include <ti/drivers/GPIO.h>
//#include "inc/hw_memmap.h"
#include "driverlib/gpio.c"

//#include "driverlib/gpio/GPIOTiva.c"
#include "driverlib/i2c.c"
#include "driverlib/uart.c"
#include "driverlib/ssi.c"
#include "driverlib/ssi.h"
#include "driverlib/qei.h"
#include "driverlib/qei.c"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.c"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "driverlib/pwm.c"

#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
//#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
/* TI-RTOS Header files */
//#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/SDSPI.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/Watchdog.h>
// #include <ti/drivers/WiFi.h>






/* Example/Board Header files */
#include "Board.h"
#include "Stepper.h"
#include "MPU9150.h"
#include "VL6180.h"
#include "buff.h"
#include "BNO55.h"


#include "hw_mpu9150.h"



#endif /* HEADERS_H_ */
