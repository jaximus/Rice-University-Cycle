

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"



#include <xdc/runtime/timestamp.h>

Types_FreqHz freq;
#define HIGH 1
#define LOW 0
typedef struct MotorObject {
	 int direction;            // Direction of rotation
	    int speed;                // Speed in RPMs
	    unsigned long step_delay; // delay between steps, in ms, based on speed
	    int number_of_steps;      // total number of steps this motor can take
	    int pin_count;            // how many pins are in use.
	    int step_number;          // which step the motor is on
	    uint32_t ui32Port;
	    // motor pin numbers:
		uint8_t motor_pin_1;
	    uint8_t motor_pin_2;
	    uint8_t motor_pin_3;
	    uint8_t motor_pin_4;

	    unsigned long last_step_time; // time stamp in us of when the last step was taken
} MotorObject;
void stepMotor(int thisStep,MotorObject* motorHandle);
void Motor_init(unsigned int MotorIndex,MotorObject* motorHandle,int number_of_steps,Types_FreqHz f );
void setSpeed(long whatSpeed,MotorObject* motorHandle);


void Motor_init(unsigned int MotorIndex,MotorObject* motorHandle,int number_of_steps,Types_FreqHz f ){

	motorHandle->step_number = 0;    // which step the motor is on
	motorHandle->speed = 0;          // the motor speed, in revolutions per minute
	motorHandle->direction = 0;      // motor direction
	motorHandle->last_step_time = 0; // time stamp in us of the last step taken
	motorHandle->number_of_steps = number_of_steps; // total number of steps for this motor
	uint8_t pins=motorHandle->motor_pin_1|motorHandle->motor_pin_2|motorHandle->motor_pin_4|motorHandle->motor_pin_3;
	GPIOPinTypeGPIOOutput(motorHandle->ui32Port,pins);
	freq=f;

	//ENable the port of the motor
	// SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}


/*
 * Sets the speed in revs per minute
 */
void setSpeed(long whatSpeed,MotorObject* motorHandle)
{
	motorHandle->step_delay = 2;//100000L;//1000L /( motorHandle->number_of_steps * whatSpeed);
}

void step(int steps_to_move,MotorObject* motorHandle)
{
  int steps_left = abs(steps_to_move);  // how many steps to take

  // determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move > 0) { motorHandle->direction = 1; }
  if (steps_to_move < 0) { motorHandle->direction = 0; }

  int olda=0;

  // decrement the number of steps, moving one step each time:
  while (steps_left > 0)
  {
		Types_Timestamp64 result;

	  Timestamp_get64(&result);

	  unsigned long now=  (result.lo/(  freq.lo/10000 ))+(530000)*result.hi;
//    unsigned long now =((Timestamp_get32()	*(1000*100)	)/freq.lo);
   int  a=now;//(Timestamp_get32()*100	)/freq.lo;
//    				if(a-(olda)	>=1){
//    		    		System_printf("\n stepper time %d",a);
//
//    					System_flush();
//    					olda=a;
//    				}


    // move only if the appropriate delay has passed:
    if (now - motorHandle->last_step_time >= motorHandle->step_delay)
    {
      // get the timeStamp of when you stepped:
    	motorHandle->last_step_time = now;
      // increment or decrement the step number,
      // depending on direction:
      if (motorHandle->direction == 1)
      {
    	  motorHandle->step_number=motorHandle->step_number+1;
        if (motorHandle->step_number == motorHandle->number_of_steps) {
        	motorHandle->step_number = 0;
        }
      }
      else
      {
        if (motorHandle->step_number == 0) {
        	motorHandle->step_number = motorHandle->number_of_steps;
        }
        motorHandle->step_number--;
      }
      // decrement the steps left:
      steps_left--;
      // step the motor to step number 0, 1, ..., {3 or 10}
      if (motorHandle->pin_count == 5)
        stepMotor(motorHandle->step_number % 10,motorHandle);
      else
        stepMotor(motorHandle->step_number % 4,motorHandle);
    }
  }
}
void stepMotor(int thisStep,MotorObject* motorHandle)
{


    switch (thisStep) {
      case 0:  // 1010
    	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_1,motorHandle->motor_pin_1);
    	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_2,LOW);
    	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_3,motorHandle->motor_pin_3);
    	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_4,LOW);
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, LOW);
//        digitalWrite(motor_pin_3, HIGH);
//        digitalWrite(motor_pin_4, LOW);
      break;
      case 1:  // 0110

   	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_1,LOW);
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_2,motorHandle->motor_pin_2);
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_3,motorHandle->motor_pin_3);
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_4,LOW);
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, HIGH);
//        digitalWrite(motor_pin_3, HIGH);
//        digitalWrite(motor_pin_4, LOW);
      break;
      case 2:  //0101
	  GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_1,LOW);
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_2,motorHandle->motor_pin_2);
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_3,LOW);
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_4,motorHandle->motor_pin_4);
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, HIGH);
//        digitalWrite(motor_pin_3, LOW);
//        digitalWrite(motor_pin_4, HIGH);
      break;
      case 3:  //1001
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_1,motorHandle->motor_pin_1);
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_2,LOW);
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_3,LOW);
	   GPIOPinWrite(motorHandle->ui32Port, motorHandle->motor_pin_4,motorHandle->motor_pin_4);
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, LOW);
//        digitalWrite(motor_pin_3, LOW);
//        digitalWrite(motor_pin_4, HIGH);
      break;

  }



}
