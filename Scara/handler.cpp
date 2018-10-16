// 
// 
// 

#include "handler.h"
#include "objdir.h"
#include "status.h"

//int HandleSio() {
//
//	ObjStruct *tmp;
//	tmp = LocateObj(0xA0);
//	uint8_t x = 1;
//	uint8_t y = 3;
//	uint16_t z = 15;
//
//	if (tmp == NULL) {
//		Serial.println("Nicht gefunden");
//		return -1;
//	}
//	else {
//		Serial.println("Gefunden");
//		
//		return 0;
//	}
//}

const int SetACK(uint8_t *index, uint8_t *props, uint16_t *data) {
	
	Serial.println((int)*index, HEX);
	Serial.println((int)*props, HEX);
	Serial.println((int)*data, HEX);
	return 0;
}

const int SetStartMove(uint8_t *index, uint8_t *props, uint16_t *data) {
	
}

const int8_t GetSystemError() {
	return GetObjStructData(0xFF);
}

const int SetNewTargetPos(uint8_t *index, uint8_t *props, uint16_t *data) {

	char* string;
	sprintf(string, "Write operation not allowed for object: %x - object ist read only", index);
	SendStatus(string, STATUS_TYPE_ERROR);
	return -1;
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

const int GetTool(uint8_t *index, uint8_t *props, uint16_t *data) {

}

const int SetTool(uint8_t *index, uint8_t *props, uint16_t *data) {

}