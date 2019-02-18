/* sio.h */

#ifndef _SIO_h
#define _SIO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif



/*
 * 
 */
void initSio(void);

/*
 *
 */
void handleSIO(void);

/*
 *
 */
void handleModbusData(void);

/*
 *
 */
void handleRapidString(void);

/*
 *
 */
void handleScaraData(void);

#endif