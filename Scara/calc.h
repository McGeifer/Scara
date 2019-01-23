// calc.h

#ifndef _CALC_h
#define _CALC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// 
#define CONVERT_COORDINATE_TO_ROBOT	0		// instruction to convert from field coordinates to robot coordinates
#define CONVERT_COORDINATE_TO_FIELD	1		// instruction to convert from robot coordinates to field coordinates

#define MACHINE_ZERO_OFFS_X_ROBOT 90.0		// x-offset between robot zero & machine zero (only about)
#define MACHINE_ZERO_OFFS_Y_ROBOT 320.0		// y-offset between robot zero & machine zero (only about)
#define MACHINE_ZERO_OFFS_X_FIELD 170.0		// x-offset between field zero & machine zero
#define MACHINE_ZERO_OFFS_Y_FIELD 25.0		// y-offset between field zero & machine zero

#define DYNA_TO_DEG(val) (round((val * 300.0) / 1023)) // convert a dynamixel position value (0 - 1023) into an angle (degree)
#define DEG_TO_DYNA(val) (round((val / 1023.0) * 300)) // convert an anggle (degree) into te dynamixel position value

//#define deg_to_rad(deg) ((deg * 71) / 4068.0) evtl. schnell als �ber arduino.h
//#define rad_to_deg(rad) ((rad * 4068) / 71.0)

enum coordinates {
	x,
	y
};

uint8_t CalcAngle(int16_t xPos, int16_t yPos);
uint8_t CalcPosistion(int16_t angle1, int16_t angle2);
int16_t CalcZPos(void);
float* ConvertCoordinate(uint8_t direction, float *xValue, float *yValue);
#endif

