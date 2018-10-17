// sio.h

#ifndef _SIO_h
#define _SIO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void InitSio();
void HandleSIO();
static void Modbus();
static void Rapid();
static void Scara();

#endif