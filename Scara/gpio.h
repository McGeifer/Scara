/* gpio.h */

#ifndef _GPIO_h
#define _GPIO_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "arduino.h"
#else
    #include "WProgram.h"
#endif

#define GPIO_PIN_MODBUS     21  /* Pin to select the Modbus operation mode */
#define GPIO_PIN_RAPID      20  /* Pin to select the Rapid operation mode */
#define GPIO_PIN_MAGNET     13  /* Pin for switching the magnet gripper */
#define GPIO_PIN_INTERRUPT  3   /* Interrupt pin for counting the position impulses of the z-axis */

/*
    Setup and configuration of the digital inputs, outputs and interrupts
 */
void initGPIO(void);

/*
    Detection of the transmission protocol and choice of operating mode
 */
void initOperationMode(void);

/*
    Interrupt service routine for the optical measuring system
 */
void lightBarrierISR(void);

#endif