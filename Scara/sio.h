// sio.h

#ifndef _SIO_h
#define _SIO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define SCARA_PACKET_LENGTH 4

void InitSio();
void HandleSIO();
void HandleModbusData();
void HandleRapidString();
void HandleScaraData();

#endif