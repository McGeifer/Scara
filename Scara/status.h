// status.h

#ifndef _STATUS_h
#define _STATUS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

 // test
void SendStatus(char* optionalDebugMessage, char* message, uint8_t statusType);
void SystemStatus();
	 

#endif