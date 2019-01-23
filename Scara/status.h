// status.h

#ifndef _STATUS_h
#define _STATUS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/* Printing of user defined messages via console. The 'optionalDebugMessage'
 * will only be printed when debug mode is active, use "NULL" if no optional 
 * message should be printed. The 'message' is required and will always be
 * printed if it's 'statusType' is not "DEBUG". The 'statusType' defines the
 * visibillity and prefix for the printed staus message. The availabile
 * types are 'NOTYPE', 'INFO', 'WARNING', 'ERROR' and 'DEBUG'. Status
 * messages with the 'DEBUG" statusType will only be printed if the system
 * is in debug mode
 */
void SendStatus(char* optionalDebugMessage, char* message, uint8_t statusType);

#endif