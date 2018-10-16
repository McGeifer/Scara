// 
// 
// 

#include "gpio.h"
#include "objdir.h"

void InitGPIO() {

	// set pin modes
	pinMode(MODBUS_PIN, INPUT);		// pin to enable Modbus communication
	pinMode(RAPID_PIN, INPUT);		// pin to enable "Rapid" mode
	pinMode(MAGNET_PIN, OUTPUT);	// pin to switch the lifting magnet on/off
	
	pinMode(WATCH_PIN, INPUT);		// interrupt pin for the light barrier
	attachInterrupt(INTERRUPT_PIN, InterruptRoutine, CHANGE);	// Attaches interrupt for counting the impulses
}

void SetOperationMode() {

	if (MODBUS_PIN == HIGH && RAPID_PIN == LOW) {
		
		SetObjStructData(0xFE, OP_MODE_MODBUS);
	}
	else if (RAPID_PIN == HIGH && MODBUS_PIN == LOW) {

		SetObjStructData(0xFE, OP_MODE_RAPID);
	}
	else {

		SetObjStructData(0xFE, OP_MODE_SCARA);
	}
}

void InterruptRoutine() {

	noInterrupts();
	// blub
	interrupts();
}