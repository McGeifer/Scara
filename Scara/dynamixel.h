// dynamixel.h

#ifndef _DYNAMIXEL_h
#define _DYNAMIXEL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void InitDynamixel();
void DynamixelError(uint8_t errorBit, uint8_t id);
void UpdatePos(void);
void HandleMove(void);

#endif