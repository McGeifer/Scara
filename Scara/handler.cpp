// 
// 
// 

#include "handler.h"
#include "objdir.h"
#include "status.h"

int SetACK(uint8_t *index, uint8_t *props, uint16_t *data) {
	
	Serial.println((int)*index, HEX);
	Serial.println((int)*props, HEX);
	Serial.println((int)*data, HEX);
	return 0;
}

int SetStartMove(uint8_t *index, uint8_t *props, uint16_t *data) {
	
}

int8_t GetSystemError() {
	return GetObjStructData(0xFF);
}

int SetNewTargetPos(uint8_t *index, uint8_t *props, uint16_t *data) {

	char* string;
	sprintf(string, "Write operation not allowed for object: %x - object ist read only", index);
	SendStatus(string, STATUS_TYPE_ERROR);
	return -1;
}

int GetActualTargetPos(uint8_t *index, uint8_t *props, uint16_t *data) {

}

int GetActualPos(uint8_t *index, uint8_t *props, uint16_t *data) {

}

int SetNewTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data) {

}

int GetActualTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data) {

}

int GetActualAngle(uint8_t *index, uint8_t *props, uint16_t *data) {

}

int GetTool(uint8_t *index, uint8_t *props, uint16_t *data) {

}

int SetTool(uint8_t *index, uint8_t *props, uint16_t *data) {

}