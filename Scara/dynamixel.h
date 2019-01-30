// dynamixel.h

#ifndef _DYNAMIXEL_h
#define _DYNAMIXEL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define DYNA_AXIS_1_TEMP_LIMIT			70		// max temperatur 70°C
#define DYNA_AXIS_1_MIN_VOLTAGE_LIMIT	110		// min voltage 11V
#define DYNA_AXIS_1_MAX_VOLTAGE_LIMIT	120		// max voltage 12V
#define DYNA_AXIS_1_MAX_TORQUE			1023	// max torque -> max value
#define DYNA_AXIS_1_SRL					1		// return level 0=none, 1=only for read comand, 2= always
#define DYNA_AXIS_1_RDT					250		// return delay time = 2µs * vaule -> 500µs
#define DYNA_AXIS_1_LED_ALARM			127		// LED blink for all error types
#define DYNA_AXIS_1_SHUT_DOWN_ALARM		37		// turn of torque for: overload, overheating and input voltage error
#define DYNA_AXIS_1_CW_C_SLOPE			32		// compilance slope - default value
#define DYNA_AXIS_1_CCW_C_SLOPE			32		
#define DYNA_AXIS_1_CW_C_MARGIN			0		// compilance margin - default value
#define DYNA_AXIS_1_CCW_C_MARGIN		0		
#define DYNA_AXIS_1_PUNCH				32		// minimum current supplied to the motor - default value

#define DYNA_AXIS_2_TEMP_LIMIT			70
#define DYNA_AXIS_2_MIN_VOLTAGE_LIMIT	110
#define DYNA_AXIS_2_MAX_VOLTAGE_LIMIT	120
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
#define DYNA_Z_AXIS_MIN_VOLTAGE_LIMIT	110
#define DYNA_Z_AXIS_MAX_VOLTAGE_LIMIT	120
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

struct dynaConfig {
	uint8_t tempLimit;
	uint8_t minVoltageLimit;
	uint8_t maxVoltageLimit;
	uint8_t maxTorque;
	uint8_t srl;
	uint8_t rdt;
	uint8_t ledAlarm;
	uint8_t shutdownAlarm;
	uint8_t cwCSlope;
	uint8_t ccwCSlope;
	uint8_t cwCMargin;
	uint8_t ccwCMargin;
	uint8_t punch;
} cfgList[] = {

{ 	DYNA_AXIS_1_TEMP_LIMIT,
	DYNA_AXIS_1_MIN_VOLTAGE_LIMIT,
	DYNA_AXIS_1_MAX_VOLTAGE_LIMIT,
	DYNA_AXIS_1_MAX_TORQUE,
	DYNA_AXIS_1_SRL,
	DYNA_AXIS_1_RDT,
	DYNA_AXIS_1_LED_ALARM,
	DYNA_AXIS_1_SHUT_DOWN_ALARM,
	DYNA_AXIS_1_CW_C_SLOPE,
	DYNA_AXIS_1_CCW_C_SLOPE,
	DYNA_AXIS_1_CW_C_MARGIN,
	DYNA_AXIS_1_CCW_C_MARGIN,
	DYNA_AXIS_1_PUNCH
},
{	DYNA_Z_AXIS_TEMP_LIMIT,
	DYNA_AXIS_2_MIN_VOLTAGE_LIMIT,
	DYNA_AXIS_2_MAX_VOLTAGE_LIMIT,
	DYNA_Z_AXIS_MAX_TORQUE,
	DYNA_Z_AXIS_SRL,
	DYNA_Z_AXIS_RDT,
	DYNA_Z_AXIS_LED_ALARM,
	DYNA_Z_AXIS_SHUT_DOWN_ALARM,
	DYNA_Z_AXIS_CW_C_SLOPE,
	DYNA_Z_AXIS_CCW_C_SLOPE,
	DYNA_Z_AXIS_CW_C_MARGIN,
	DYNA_Z_AXIS_CCW_C_MARGIN,
	DYNA_Z_AXIS_PUNCH
},
{	DYNA_Z_AXIS_TEMP_LIMIT,
	DYNA_Z_AXIS_MIN_VOLTAGE_LIMIT,
	DYNA_Z_AXIS_MAX_VOLTAGE_LIMIT,
	DYNA_Z_AXIS_MAX_TORQUE,
	DYNA_Z_AXIS_SRL,
	DYNA_Z_AXIS_RDT,
	DYNA_Z_AXIS_LED_ALARM,
	DYNA_Z_AXIS_SHUT_DOWN_ALARM,
	DYNA_Z_AXIS_CW_C_SLOPE,
	DYNA_Z_AXIS_CCW_C_SLOPE,
	DYNA_Z_AXIS_CW_C_MARGIN,
	DYNA_Z_AXIS_CCW_C_MARGIN,
	DYNA_Z_AXIS_PUNCH
} };

enum dataType
{
	pos,
	speed
};

uint8_t(*func[])(uint8_t arg1, uint8_t arg2) = {

}

/* Basic configuration of the Dynamixel servos */
void InitDynamixel(void);

/* Handler function for Dynamixel specific error codes */
void DynamixelError(uint8_t errorBit, uint8_t id);

/* Cyclic function for updating all actual position values in the object dictionary */
void UpdateObjDir(void);

/* Cyclic function for detecting and executing movements of Dynamixel servos */
void HandleMove(void);

#endif