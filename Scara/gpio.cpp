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
	pinMode(INTERRUPT_PIN, INPUT);	// interrupt pin for the light barrier

	// interrupts
	attachInterrupt(digitalPinToInterrupt(3), ISR_LightBarrier, CHANGE);	// attache interrupt for light barrier impulse count
}

void InitOperationMode(void) {

	if (digitalRead(RAPID_PIN) == LOW && digitalRead(MODBUS_PIN) == HIGH) {
		if (SetObjStructData(OBJ_IDX_OP_MODE, OP_MODE_MODBUS) == 0) {
			// modbus protocoll selected
			return;
		}
	}
	else if (digitalRead(RAPID_PIN) == HIGH && digitalRead(MODBUS_PIN) == LOW) {
		if (SetObjStructData(OBJ_IDX_OP_MODE, OP_MODE_RAPID) == 0) {
			SendStatus("InitOperationMode(): ", "Rapid protocoll selected", STATUS_TYPE_INFO);
			return;
		}
	}
	else {
		if (SetObjStructData(OBJ_IDX_OP_MODE, OP_MODE_SCARA) == 0) {
			SendStatus("InitOperationMode(): ", "Scara protocoll selected", STATUS_TYPE_INFO);
			return;
		}
	}
	SendStatus("InitOperationMode(): ", "error selecting protocoll", STATUS_TYPE_ERROR);
}

void ISR_LightBarrier(void) {

	if (GetObjStructData(OBJ_IDX_Z_ACTUAL_POS) > GetObjStructData(OBJ_IDX_Z_ACTUAL_TARGET_POS)) {
		SetObjStructData(OBJ_IDX_Z_POS_COUNT, GetObjStructData(OBJ_IDX_Z_POS_COUNT) - 1);
	}
	else {
		SetObjStructData(OBJ_IDX_Z_POS_COUNT, GetObjStructData(OBJ_IDX_Z_POS_COUNT) + 1);
	}
}