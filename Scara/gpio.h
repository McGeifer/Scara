// gpio.h

#ifndef _GPIO_h
#define _GPIO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define MODBUS_PIN		21	/* Pin to select the Modbus operation mode */
#define RAPID_PIN		20	/* Pin to select the Rapid operation mode */
#define MAGNET_PIN		13	/* Pin for switching the magnet gripper */
#define INTERRUPT_PIN	3	/* Interrupt pin for counting the position impulses of the z-axis */
#define DIRECTION_PIN	2	/* Pin for switching the tristate buffer */

/* Setup and configuration of the digital in/ outputs and interrupts */
void InitGPIO(void);

/* Detection of the transmission protocol and choice of operating mode  */
void InitOperationMode(void);

/* Interrupt service routine of the optical measuring system */
void LightBarrierISR(void);

#endif