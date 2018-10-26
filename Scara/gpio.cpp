// 
// 
// 

#include "gpio.h"
#include "objdir.h"
#include "status.h"

void InitGPIO() {

	// set pin modes
	pinMode(MODBUS_PIN, INPUT);		// pin to enable Modbus communication
	pinMode(RAPID_PIN, INPUT);		// pin to enable "Rapid" mode
	pinMode(MAGNET_PIN, OUTPUT);	// pin to switch the lifting magnet on/off
	
	pinMode(WATCH_PIN, INPUT);		// interrupt pin for the light barrier
	attachInterrupt(INTERRUPT_PIN, InterruptRoutine, CHANGE);	// Attaches interrupt for counting the impulses
}

void InitOperationMode() {

	if (digitalRead(RAPID_PIN) == LOW && digitalRead(MODBUS_PIN) == HIGH) {
		SetObjStructData(0xFE, OP_MODE_MODBUS);
	}
	else if (digitalRead(RAPID_PIN) == HIGH && digitalRead(MODBUS_PIN) == LOW) {
		SetObjStructData(0xFE, OP_MODE_RAPID);
		SendStatus("Rapid protocoll selected", STATUS_TYPE_INFO);
	}
	else {
		SetObjStructData(0xFE, OP_MODE_SCARA);
		SendStatus("Scara protocoll selected", STATUS_TYPE_INFO);
	}
}

static void InterruptRoutine() {

	noInterrupts();
	// blub
	interrupts();
}