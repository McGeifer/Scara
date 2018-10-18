// 
// 
// 

#include "handler.h"
#include "objdir.h"
#include "status.h"

int SetACK(const uint8_t *index, const uint8_t *props, const uint16_t *data) {
	
	Serial.println((int)*index, HEX);
	Serial.println((int)*props, HEX);
	Serial.println((int)*data, HEX);
	return 0;
}

int SetStartMove(const uint8_t *index, const uint8_t *props, const uint16_t *data) {
	
}

int8_t GetSystemError() {
	return GetObjStructData(0xFF);
}

int SetNewTargetPos(const uint8_t *index, const uint8_t *props, const uint16_t *data) {

	char* string;
	sprintf(string, "Write operation not allowed for object: %x - object ist read only", index);
	SendStatus(string, STATUS_TYPE_ERROR);
	return -1;
}

int GetActualTargetPos(const uint8_t *index, const uint8_t *props, const uint16_t *data){}
int GetActualPos(const uint8_t *index, const uint8_t *props, const uint16_t *data){}
int SetNewTargetAngle(const uint8_t *index, const uint8_t *props, const uint16_t *data){}
int GetActualTargetAngle(const uint8_t *index, const uint8_t *props, const uint16_t *data){}
int GetActualAngle(const uint8_t *index, const uint8_t *props, const uint16_t *data){}
int GetTool(const uint8_t *index, const uint8_t *props, const uint16_t *data){}
int SetTool(const uint8_t *index, const uint8_t *props, const uint16_t *data){}