// gpio.h

#ifndef _GPIO_h
#define _GPIO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define MODBUS_PIN		21
#define RAPID_PIN		20
#define MAGNET_PIN		13
#define WATCH_PIN		3	// Interrupt pin for counting the position impulses of the z-axis (digital pin 3)
#define INTERRUPT_PIN	1	// Interrupt pin 1 is digital pin 3


void InitGPIO();
void InterruptRoutine();

#endif