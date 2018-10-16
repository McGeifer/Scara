// handler.h

#ifndef _HANDLER_h
#define _HANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//int HandleSio();
const int SetACK (uint8_t *index, uint8_t *props, uint16_t *data);
const int SetStartMove (uint8_t *index, uint8_t *props, uint16_t *data);

const int8_t GetSystemError();

const int SetNewTargetPos(uint8_t *index, uint8_t *props, uint16_t *data);
const int GetActualTargetPos(uint8_t *index, uint8_t *props, uint16_t *data);
const int GetActualPos(uint8_t *index, uint8_t *props, uint16_t *data);
const int SetNewTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data);
const int GetActualTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data);
const int GetActualAngle(uint8_t *index, uint8_t *props, uint16_t *data);
const int GetTool(uint8_t *index, uint8_t *props, uint16_t *data);
const int SetTool(uint8_t *index, uint8_t *props, uint16_t *data);

#endif