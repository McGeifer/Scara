// move.h

#ifndef _MOVE_h
#define _MOVE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

int8_t UpdatePos(void);
void HandleMove(void);

#endif

