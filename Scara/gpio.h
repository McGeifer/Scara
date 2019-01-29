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
#define INTERRUPT_PIN	3	/* Interrupt pin for counting the position impulses of the z-axis */

/* Setup and configuration of the digital in- and outputs and interrupts */
void InitGPIO(void);

/* Detection of the transmission protocol and choice of operating mode  */
void InitOperationMode(void);

/* Interrupt service routine of the optical measuring system */
void LightBarrierISR(void);

#endif