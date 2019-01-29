// 
// 
// 

#include "gpio.h"
#include "objdir.h"
#include "status.h"

void InitGPIO() {

	/* set pin modes */
	pinMode(MODBUS_PIN, INPUT);		/* pin to enable Modbus communication */
	pinMode(RAPID_PIN, INPUT);		/* pin to enable "Rapid" mode */
	pinMode(MAGNET_PIN, OUTPUT);	/* pin to switch the lifting magnet on/off */
	pinMode(INTERRUPT_PIN, INPUT);	/* interrupt pin for the light barrier */

	/* interrupts */
	attachInterrupt(digitalPinToInterrupt(3), LightBarrierISR, CHANGE);	/* attache interrupt for light barrier impulse counter */
}

void InitOperationMode(void) {

	if (digitalRead(RAPID_PIN) == LOW && digitalRead(MODBUS_PIN) == HIGH) {
		if (SetObjData(OBJ_IDX_OP_MODE, OP_MODE_MODBUS) == 0) {
			/* modbus protocol selected */
			return;
		}
	}
	else if (digitalRead(RAPID_PIN) == HIGH && digitalRead(MODBUS_PIN) == LOW) {
		if (SetObjData(OBJ_IDX_OP_MODE, OP_MODE_RAPID) == 0) {
			SendStatus("InitOperationMode(): ", "Rapid protocoll selected", STATUS_TYPE_INFO);
			/* rapid protocol selected */
			return;
		}
	}
	else {
		if (SetObjData(OBJ_IDX_OP_MODE, OP_MODE_SCARA) == 0) {
			SendStatus("InitOperationMode(): ", "Scara protocoll selected", STATUS_TYPE_INFO);
			/* scara protocol selected */
			return;
		}
	}
	SendStatus("InitOperationMode(): ", "error selecting protocoll", STATUS_TYPE_ERROR);
}

void LightBarrierISR(void) {

	if (GetObjData(OBJ_IDX_Z_ACTUAL_POS) > GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS)) {
		SetObjData(OBJ_IDX_Z_POS_COUNT, GetObjData(OBJ_IDX_Z_POS_COUNT) - 1);
	}
	else {
		SetObjData(OBJ_IDX_Z_POS_COUNT, GetObjData(OBJ_IDX_Z_POS_COUNT) + 1);
	}
}