// dynamixel.h

#ifndef _DYNAMIXEL_h
#define _DYNAMIXEL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DynamixelSerial2\DynamixelSerial2.h"

#define DYNA_AXIS_1_TEMP_LIMIT			70		// max temperatur 70°C
#define DYNA_AXIS_1_LOW_VOLTAGE_LIMIT	110		// min voltage 11V
#define DYNA_AXIS_1_HIGH_VOLTAGE_LIMIT	120		// max voltage 12V
#define DYNA_AXIS_1_MAX_TORQUE			1023	// max torque -> max value
#define DYNA_AXIS_1_SRL					1		// return level 0=none, 1=only for read comand, 2= always
#define DYNA_AXIS_1_RDT					250		// return delay time = 2µs * vaule -> 500µs
#define DYNA_AXIS_1_LED_ALARM			127		// LED blink for all error types
#define DYNA_AXIS_1_SHUT_DOWN_ALARM		37		// turn of torque for: overload, overheating and input voltage error
#define DYNA_AXIS_1_CW_C_SLOPE			32		// compilance slope (clock wise) - default value
#define DYNA_AXIS_1_CCW_C_SLOPE			32		// compilance slope (counter clock wise) - default value
#define DYNA_AXIS_1_CW_C_MARGIN			0		// compilance margin (clock wise) - default value
#define DYNA_AXIS_1_CCW_C_MARGIN		0		// compilance margin (counter clock wise) - default value
#define DYNA_AXIS_1_PUNCH				32		// minimum current supplied to the motor - default value

#define DYNA_AXIS_2_TEMP_LIMIT			70
#define DYNA_AXIS_2_LOW_VOLTAGE_LIMIT	110
#define DYNA_AXIS_2_HIGH_VOLTAGE_LIMIT	120
#define DYNA_AXIS_2_MAX_TORQUE			1023
#define DYNA_AXIS_2_SRL					1
#define DYNA_AXIS_2_RDT					150
#define DYNA_AXIS_2_LED_ALARM			127
#define DYNA_AXIS_2_SHUT_DOWN_ALARM		37
#define DYNA_AXIS_2_CW_C_SLOPE			32
#define DYNA_AXIS_2_CCW_C_SLOPE			32
#define DYNA_AXIS_2_CW_C_MARGIN			0
#define DYNA_AXIS_2_CCW_C_MARGIN		0
#define DYNA_AXIS_2_PUNCH				32

#define DYNA_Z_AXIS_TEMP_LIMIT			70
#define DYNA_Z_AXIS_LOW_VOLTAGE_LIMIT	110
#define DYNA_Z_AXIS_HIGH_VOLTAGE_LIMIT	120
#define DYNA_Z_AXIS_MAX_TORQUE			1023
#define DYNA_Z_AXIS_SRL					1
#define DYNA_Z_AXIS_RDT					250
#define DYNA_Z_AXIS_LED_ALARM			127
#define DYNA_Z_AXIS_SHUT_DOWN_ALARM		37
#define DYNA_Z_AXIS_CW_C_SLOPE			32
#define DYNA_Z_AXIS_CCW_C_SLOPE			32
#define DYNA_Z_AXIS_CW_C_MARGIN			0
#define DYNA_Z_AXIS_CCW_C_MARGIN		0
#define DYNA_Z_AXIS_PUNCH				32

/* function pointer typedef for the dynamixel setup functions*/
typedef int16_t(*funcPtr)(uint8_t id, int16_t val/*, const char **funcName*/);

enum dataType
{
	angle,
	speed
};

/* Setup function for the Dynamixel servos */
void InitDynamixel(void);

/* Handler function for Dynamixel-specific error codes. If an error code is reported to
the function, it will issue an error message. In addition, the execution of the main loop
is stopped to prevent inadvertent behavior */
void DynamixelError(uint8_t errorBit, uint8_t id);

/* Cyclic function for updating all actual position/ angle values of the object dictionary */
void UpdateObjDir(void);

/* Cyclic function for detecting the move status of the system and if necessary executing
move commands of the Dynamixel servos */
void HandleMove(void);

#endif