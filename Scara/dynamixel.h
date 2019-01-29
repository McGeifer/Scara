// dynamixel.h

#ifndef _DYNAMIXEL_h
#define _DYNAMIXEL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
/* Basic configuration of the Dynamixel servos */
void InitDynamixel(void);

/* Handler function for Dynamixel specific error codes */
void DynamixelError(uint8_t errorBit, uint8_t id);

/* Cyclic function for updating all actual positions in the object dictionary */
void UpdatePos(void);

/* Cyclic function for detecting and executing movements of Dynamixel servos */
void HandleMove(void);

#endif