// calc.h

#ifndef _CALC_h
#define _CALC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define CONVERT_COORDINATE_TO_ROBOT	0		/* instruction to convert from field coordinates to robot coordinates */
#define CONVERT_COORDINATE_TO_FIELD	1		/* instruction to convert from robot coordinates to field coordinates */

#define MACHINE_ZERO_OFFS_X_ROBOT 90.0		/* x-offset between robot zero & machine zero (only about) */
#define MACHINE_ZERO_OFFS_Y_ROBOT 320.0		/* y-offset between robot zero & machine zero (only about) */
#define MACHINE_ZERO_OFFS_X_FIELD 170.0		/* x-offset between field zero & machine zero */
#define MACHINE_ZERO_OFFS_Y_FIELD 25.0		/* y-offset between field zero & machine zero */

#define DYNA_TO_DEG(val) (round((val * 300.0) / 1023)) /* convert a dynamixel position value (0 - 1023) into an angle (degree) */
#define DEG_TO_DYNA(val) (round((val / 1023.0) * 300)) /* convert an angle (degree) into a dynamixel position value */

#define SOLUTION_A 0x01
#define SOLUTION_B 0x02

#define SERVO_1_OFFS 2.87979327				/* conversion from field to servo orientation (165° offset to x-axis) */
#define SERVO_2_OFFS 0.52359877				/* conversion from triangle to servo orientation (30° offset to c) */

/* #undef DEG_TO_RAD
#define DEG_TO_RAD(deg) ((deg * 71) / 4068.0) evtl. schneller als über arduino.h  -------------------- TESTEN --------------------- !!!!!!!!!!!!!
#undef RAD_TO_DEG
#define RAD_TO_DEG(rad) ((rad * 4068) / 71.0) */

/* CalcAngle computes the angles for axis 1 & 2 out of the coordinates x & y */
float* CalcAngle(int16_t *xVal, int16_t *yVal);

/* Using vector operations to calculate actual x & y position of robot arm based on position values */
float* CalcPosistion(int16_t *angleAxis1, int16_t *angleAxis2);

/* UpdateZPos calculates the actual position of the z-axis. */
int8_t UpdateZPos(void);



#endif

