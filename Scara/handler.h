// handler.h

#ifndef _HANDLER_h
#define _HANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//int HandleSio();
int SetACK					(const uint8_t *index, const uint8_t *props, const int16_t *data);
int SetStartMove			(const uint8_t *index, const uint8_t *props, const int16_t *data);

int8_t GetSystemError();

int SetNewTargetPos			(const uint8_t *index, const uint8_t *props, const int16_t *data);
int GetActualTargetPos		(const uint8_t *index, const uint8_t *props, const int16_t *data);
int GetActualPos			(const uint8_t *index, const uint8_t *props, const int16_t *data);
int SetNewTargetAngle		(const uint8_t *index, const uint8_t *props, const int16_t *data);
int GetActualTargetAngle	(const uint8_t *index, const uint8_t *props, const int16_t *data);
int GetActualAngle			(const uint8_t *index, const uint8_t *props, const int16_t *data);
int GetTool					(const uint8_t *toolIndex);
int SetTool					(const uint8_t *toolIndex);
#endif