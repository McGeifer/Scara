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
#define DEG_TO_DYNA(val) (round((val / 1023.0) * 300)) // convert an angle (degree) into a dynamixel position value

//#undef DEG_TO_RAD
//#define DEG_TO_RAD(deg) ((deg * 71) / 4068.0) evtl. schneller als über arduino.h
//#undef RAD_TO_DEG
//#define RAD_TO_DEG(rad) ((rad * 4068) / 71.0)

enum coordinates {
	x,
	y
};

// CalcAngle computes the angles for axis 1 & 2 out of the coordinates x & y
int8_t CalcAngle(int16_t xVal, int16_t yVal);

// CalcPosistion computes the coordinates x & y out of the angles of axis 1 & 2
int8_t CalcPosistion(int16_t angle1, int16_t angle2);

// CalcZPos computes the actual position of the z-axis.
int8_t CalcZPos(void);

// Help function - ConvertCoordinate converts x and y coordiantes between the different coordinate systems (machine & field).
float* ConvertCoordinate(uint8_t direction, float *xValue, float *yValue);

#endif

