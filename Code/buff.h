
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/fatfs/ff.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>

typedef enum srcName
{	prx0 =0,
	prx1,
	imu0,
	imu1,
	info
} srcName;

typedef struct DataObj {
//	Queue_Elem elem;   //Queue req
	uint16_t buf;
	srcName src;       //sensor
	uint32_t ts;       //timestamp
	uint16_t dat[6];   //data array
//	uint8_t datSize; 	//data length
} sdDataObj,proximityDataObj,xbeeDataObj;


//Processor frequency
Types_FreqHz freq;

#define BUFFER_SIZE 256


typedef struct circular_buffer
{
  sdDataObj sdBuffer[BUFFER_SIZE];
  volatile uint16_t begin;
  volatile uint16_t end;
} cir_buffer;


void circ_init(cir_buffer* cBuf)
{
	cBuf->begin=0;
	cBuf->end=0;

}
void circ_add(cir_buffer* cBuf, sdDataObj in)
{
	cBuf->sdBuffer[cBuf->end] = in;
	cBuf->end=(++(cBuf->end))%BUFFER_SIZE;
	if(cBuf->end==cBuf->begin)
		cBuf->begin=(++(cBuf->begin))%BUFFER_SIZE;
}

bool circ_rem(cir_buffer* cBuf, sdDataObj *out)
{
	if(cBuf->end == cBuf->begin)
		return false;

	*out = cBuf->sdBuffer[cBuf->begin];
	cBuf->begin=(++(cBuf->begin))%BUFFER_SIZE;
	return true;
}

int circ_size(cir_buffer* cBuf)
{
	if(cBuf->end<cBuf->begin)
		return 	(cBuf->end + BUFFER_SIZE- cBuf->begin)%BUFFER_SIZE;
	return 	(cBuf->end - cBuf->begin)%BUFFER_SIZE;


}



