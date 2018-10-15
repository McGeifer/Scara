// 
// 
// 

#include "gpio.h"


void InitGPIO() {

	// set pin modes
	pinMode(MODBUS_PIN, INPUT);		// pin to enable Modbus communication
	pinMode(RAPID_PIN, INPUT);		// pin to enable "Rapid" mode
	pinMode(MAGNET_PIN, OUTPUT);	// pin to switch the lifting magnet on/off
	
	pinMode(WATCH_PIN, INPUT);		// interrupt pin for the light barrier
	attachInterrupt(INTERRUPT_PIN, InterruptRoutine, CHANGE);	// Attaches interrupt for counting the impulses
}

void InterruptRoutine()
{
	noInterrupts();

	interrupts();
}