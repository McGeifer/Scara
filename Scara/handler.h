// handler.h

#ifndef _HANDLER_h
#define _HANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

int HandleSio();
const int Hdl_ACK (uint8_t *index, uint8_t *props, uint16_t *data);
const int Hdl_StartMove (uint8_t *index, uint8_t *props, uint16_t *data);
const int SetNewTargetPos(uint8_t *index, uint8_t *props, uint16_t *data);
const int GetActualTargetPos(uint8_t *index, uint8_t *props, uint16_t *data);
const int GetActualPos(uint8_t *index, uint8_t *props, uint16_t *data);
const int SetNewTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data);
const int GetActualTargetAngle(uint8_t *index, uint8_t *props, uint16_t *data);
const int GetActualAngle(uint8_t *index, uint8_t *props, uint16_t *data);

#endif