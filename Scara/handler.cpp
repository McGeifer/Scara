// 
// 
// 

#include "handler.h"
#include "objdir.h"

int HandleSio() {

	ObjStruct *tmp;
	tmp = LocateObjDir(0xA0, 0, 0);
	uint8_t x = 1;
	uint8_t y = 3;
	uint16_t z = 15;

	if (tmp == NULL) {
		Serial.println("Nicht gefunden");
		return -1;
	}
	else {
		Serial.println("Gefunden");
		tmp->pFunction(&x, &y, &z);
		return 0;
	}
}

const int Hdl_ACK(uint8_t *index, uint8_t *props, uint16_t *data) {
	
	Serial.println((int)*index, HEX);
	Serial.println((int)*props, HEX);
	Serial.println((int)*data, HEX);
	return 0;
}

const int Hdl_StartMove(uint8_t *index, uint8_t *props, uint16_t *data) {
	
}

const int SetNewTargetPos(uint8_t *index, uint8_t *props, uint16_t *data) {

}

const int GetActualTargetPos(uint8_t *index, uint8_t *props, uint16_t *data) {

}

const int GetActualPos(uint8_t *index, uint8_t *props, uint16_t *data) {

}

const int SetNewTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data) {

}

const int GetActualTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data) {

}

const int GetActualAngle(uint8_t *index, uint8_t *props, uint16_t *data) {

}