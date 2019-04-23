/* sio.h */

#ifndef _SIO_h
#define _SIO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


static void handleModbusData(void);
static void handleRapidString(void);
static void handleScaraData(void);
/*
	
 */
void initSio(void);
void handleSIO(void);
#endif