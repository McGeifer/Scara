// handler.h

#ifndef _HANDLER_h
#define _HANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//int HandleSio();
int SetACK (uint8_t *index, uint8_t *props, uint16_t *data);
int SetStartMove (uint8_t *index, uint8_t *props, uint16_t *data);

int8_t GetSystemError();

int SetNewTargetPos(uint8_t *index, uint8_t *props, uint16_t *data);
int GetActualTargetPos(uint8_t *index, uint8_t *props, uint16_t *data);
int GetActualPos(uint8_t *index, uint8_t *props, uint16_t *data);
int SetNewTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data);
int GetActualTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data);
int GetActualAngle(uint8_t *index, uint8_t *props, uint16_t *data);
int GetTool(uint8_t *index, uint8_t *props, uint16_t *data);
int SetTool(uint8_t *index, uint8_t *props, uint16_t *data);

#endif