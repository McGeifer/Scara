/* calc.h */

#ifndef _CALC_h
#define _CALC_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define CONVERT_COORDINATE_TO_ROBOT 0       /* instruction to convert from field coordinates to robot coordinates */
#define CONVERT_COORDINATE_TO_FIELD 1       /* instruction to convert from robot coordinates to field coordinates */

#define MACHINE_ZERO_OFFS_X_ROBOT   90.0    /* x-offset between robot zero & machine zero (only about) */
#define MACHINE_ZERO_OFFS_Y_ROBOT   320.0   /* y-offset between robot zero & machine zero (only about) */
#define MACHINE_ZERO_OFFS_X_FIELD   170.0   /* x-offset between field zero & machine zero */
#define MACHINE_ZERO_OFFS_Y_FIELD   25.0    /* y-offset between field zero & machine zero */

#define SOLUTION_A 0x01
#define SOLUTION_B 0x02

#define SERVO_1_OFFS 2.87979327             /* conversion from field to servo orientation (165° offset to x-axis) */
#define SERVO_2_OFFS 0.52359877             /* conversion from triangle to servo orientation (30° offset to c) */

/* #undef DEG_TO_RAD
#define DEG_TO_RAD(deg) ((deg * 71) / 4068.0) evtl. schneller als über arduino.h  -------------------- TESTEN --------------------- !!!!!!!!!!!!!
#undef RAD_TO_DEG
#define RAD_TO_DEG(rad) ((rad * 4068) / 71.0) */

/*
    CalcAngle computes the angles (degree) for axis 1 & 2 out of the coordinates x & y
 */
float* CalcAngle(int16_t *xPos, int16_t *yPos);

/*
    Function to calculate actual x & y positions of the robot arm based on the axis angles (degree).
 */
float* CalcPosistion(int16_t *angleAxis1, int16_t *angleAxis2);

/* 
    Function to convert a dynamixel position value (0 - 1023) into an angle (degree).
    value > 1023 -> value = 1023
    value < 0    -> value = 0
*/
float DynaToDeg(int16_t *dxl_val);

/*
    Function to convert an angle (degree) into a dynamixel position value.
    angle > 300 -> angle = 300
    angle < 0   -> angle = 0
*/
int16_t DegToDyna(float *angle);

/*
    UpdateZPos calculates the actual position of the z-axis.
 */
int16_t UpdateZPos(void);



#endif

