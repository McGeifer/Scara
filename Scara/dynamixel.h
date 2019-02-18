/* dynamixel.h */

#ifndef _DYNAMIXEL_h
#define _DYNAMIXEL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DynamixelSerial2.h"

/* parameters of Dynamixel control table */
/* EEPROM AREA */
#define DXL_P_MODEL_NUMBER_L			0		/* Model Number(L) */
#define DXL_P_MODOEL_NUMBER_H			1		/* Model Number(H) */
#define DXL_P_VERSION					2		/* Version of Firmware */
#define DXL_P_ID						3		/* ID */
#define DXL_P_BAUD_RATE					4		/* Baud Rate */
#define DXL_P_RETURN_DELAY_TIME			5		/* Return Delay Time */
#define DXL_P_CW_ANGLE_LIMIT_L			6		/* CW Angle Limit(L) */
#define DXL_P_CW_ANGLE_LIMIT_H			7		/* CW Angle Limit(H) */
#define DXL_P_CCW_ANGLE_LIMIT_L			8		/* CCW Angle Limit(L) */
#define DXL_P_CCW_ANGLE_LIMIT_H			9		/* CCW Angle Limit(H) */
/* #define DXL_P_SYSTEM_DATA2			10		(Reserved) */
#define DXL_P_LIMIT_TEMPERATURE			11		/* the Highest Limit Temperature */
#define DXL_P_DOWN_LIMIT_VOLTAGE		12		/* the Lowest Limit Voltage */
#define DXL_P_UP_LIMIT_VOLTAGE			13		/* the Highest Limit Voltage */
#define DXL_P_MAX_TORQUE_L				14		/* Max Torque(L) */
#define DXL_P_MAX_TORQUE_H				15		/* Max Torque(H) */
#define DXL_P_RETURN_LEVEL				16		/* Status Return Level */
#define DXL_P_ALARM_LED					17		/* Alarm LED */
#define DXL_P_ALARM_SHUTDOWN			18		/* Alarm Shutdown */
/* #define DXL_P_OPERATING_MODE			19		(Reserved) */
#define DXL_P_DOWN_CALIBRATION_L		20		/* Down Calibration(L) */
#define DXL_P_DOWN_CALIBRATION_H		21		/* Down Calibration(H) */
#define DXL_P_UP_CALIBRATION_L			22		/* Up Calibration(L) */
#define DXL_P_UP_CALIBRATION_H			23		/* Up Calibration(H) */
												   
/* RAM AREA */								   
#define DXL_P_TORQUE_ENABLE				24		/* Torque Enable */
#define DXL_P_LED						25		/* LED */
#define DXL_P_CW_COMPLIANCE_MARGIN		26		/* CW Compliance Margin */
#define DXL_P_CCW_COMPLIANCE_MARGIN		27		/* CCW Compliance Margin */
#define DXL_P_CW_COMPLIANCE_SLOPE		28		/* CW Compliance Slope */
#define DXL_P_CCW_COMPLIANCE_SLOPE		29		/* CCW Compliance Slope */
#define DXL_P_GOAL_POSITION_L			30		/* Goal Position(L) */
#define DXL_P_GOAL_POSITION_H			31		/* Goal Position(H) */
#define DXL_P_GOAL_SPEED_L				32		/* Moving Speed(L) */
#define DXL_P_GOAL_SPEED_H				33		/* Moving Speed(H) */
#define DXL_P_TORQUE_LIMIT_L			34		/* Torque Limit(L) */
#define DXL_P_TORQUE_LIMIT_H			35		/* Torque Limit(H) */
#define DXL_P_PRESENT_POSITION_L		36		/* Present Position(L) */
#define DXL_P_PRESENT_POSITION_H		37		/* Present Position(H) */
#define DXL_P_PRESENT_SPEED_L			38		/* Present Speed(L) */
#define DXL_P_PRESENT_SPEED_H			39		/* Present Speed(H) */
#define DXL_P_PRESENT_LOAD_L			40		/* Present Load(L) */
#define DXL_P_PRESENT_LOAD_H			41		/* Present Load(H) */
#define DXL_P_PRESENT_VOLTAGE			42		/* Present Voltage */
#define DXL_P_PRESENT_TEMPERATURE		43		/* Present Temperature */
#define DXL_P_REGISTERED_INSTRUCTION	44		/* Registered Instruction */
/* #define DXL_P_PAUSE_TIME				45		(Reserved) */
#define DXL_P_MOVING					46		/* Moving */
#define DXL_P_LOCK						47		/* Lock */
#define DXL_P_PUNCH_L					48		/* Punch(L) */
#define DXL_P_PUNCH_H					49		/* Punch(H) */

/* instructions */
#define DXL_INST_PING					0x01	/* No action. Used for obtaining a Status Packet  */
#define DXL_INST_READ					0x02	/* Reading values in the Control Table  */
#define DXL_INST_WRITE					0x03	/* Writing values to the Control Table  */
#define DXL_INST_REG_WRITE				0x04	/* Similar to WRITE, but stays in standby mode until the ACION instruction is given  */
#define DXL_INST_ACTION					0x05	/* Triggers the action registered by the REG_WRITE instruction  */
#define DXL_INST_RESET					0x06	/* Changes the control table values of the Dynamixel actuator to the Factory Default Value  */
#define DXL_INST_SYNC_WRITE				0x83	/* Used for controlling many Dynamixel actuators at the same time  */

/* parameter access restrictions */
#define DXL_P_RD						0x01	/* read only */
#define DXL_P_WR						0x02	/* write only */
#define DXL_P_RD_WR				(0x01 | 0x02)	/* read & write */

/* setup parameters for function pointer table */
#define DXL_AXIS_1_TEMP_LIMIT			70		/* max temperature 70�C */
#define DXL_AXIS_1_LOW_VOLTAGE_LIMIT	110		/* min voltage 11V */
#define DXL_AXIS_1_HIGH_VOLTAGE_LIMIT	120		/* max voltage 12V */
#define DXL_AXIS_1_MAX_TORQUE			1023	/* max torque -> max value */
#define DXL_AXIS_1_SRL					1		/* return level 0=none, 1=only for read command, 2= always */
#define DXL_AXIS_1_RDT					250		/* return delay time = 2�s * value -> 500�s */
#define DXL_AXIS_1_LED_ALARM			127		/* LED blink for all error types */
#define DXL_AXIS_1_SHUT_DOWN_ALARM		37		/* turn of torque for: overload, overheating and input voltage error */
#define DXL_AXIS_1_CW_C_SLOPE			32		/* compliance slope (clock wise) - default value */
#define DXL_AXIS_1_CCW_C_SLOPE			32		/* compliance slope (counter clock wise) - default value */
#define DXL_AXIS_1_CW_C_MARGIN			0		/* compliance margin (clock wise) - default value */
#define DXL_AXIS_1_CCW_C_MARGIN			0		/* compliance margin (counter clock wise) - default value */
#define DXL_AXIS_1_PUNCH				32		/* minimum current supplied to the motor - default value */

#define DXL_AXIS_2_TEMP_LIMIT			70
#define DXL_AXIS_2_LOW_VOLTAGE_LIMIT	110
#define DXL_AXIS_2_HIGH_VOLTAGE_LIMIT	120
#define DXL_AXIS_2_MAX_TORQUE			1023
#define DXL_AXIS_2_SRL					1
#define DXL_AXIS_2_RDT					150
#define DXL_AXIS_2_LED_ALARM			127
#define DXL_AXIS_2_SHUT_DOWN_ALARM		37
#define DXL_AXIS_2_CW_C_SLOPE			32
#define DXL_AXIS_2_CCW_C_SLOPE			32
#define DXL_AXIS_2_CW_C_MARGIN			0
#define DXL_AXIS_2_CCW_C_MARGIN			0
#define DXL_AXIS_2_PUNCH				32

#define DXL_Z_AXIS_TEMP_LIMIT			70
#define DXL_Z_AXIS_LOW_VOLTAGE_LIMIT	110
#define DXL_Z_AXIS_HIGH_VOLTAGE_LIMIT	120
#define DXL_Z_AXIS_MAX_TORQUE			1023
#define DXL_Z_AXIS_SRL					1
#define DXL_Z_AXIS_RDT					250
#define DXL_Z_AXIS_LED_ALARM			127
#define DXL_Z_AXIS_SHUT_DOWN_ALARM		37
#define DXL_Z_AXIS_CW_C_SLOPE			32
#define DXL_Z_AXIS_CCW_C_SLOPE			32
#define DXL_Z_AXIS_CW_C_MARGIN			0
#define DXL_Z_AXIS_CCW_C_MARGIN			0
#define DXL_Z_AXIS_PUNCH				32


/* various */
#define DXL_POS_TOLERANCE				0.5		/* max position tolerance for a Dynamixel */
//#define DXL_DEFAULT_RETURN_PACKET_SIZE	6		/* return packet for error code */

/* dynamixel axis id's */
#define DXL_ID_AXIS_1					0x00	/* Dynamixel id for axis 1 */
#define DXL_ID_AXIS_2					0x01	/* Dynamixel id for axis 2 */
#define DXL_ID_AXIS_Z					0x02	/* Dynamixel is for z axis */
#define DXL_BROADCASTING_ID				0xFE	/* Dynamixel broadcast id */

/* connection settings */
#define DXL_DIRECTION_PIN				2		/* pin for switching the tristate buffer (switching between Tx and Rx mode) */

/* macros for serial port functions */
#define beginCom(args)  (Serial2.begin(args))	/* begin serial communication */
#define sendData(args)  (Serial2.write(args))	/* write over serial */
#define availableData() (Serial2.available())	/* check serial data available */
#define readData()      (Serial2.read())		/* read serial data */
#define peekData()      (Serial2.peek())		/* peek serial data */

/* macros for switching between Rx & Tx mode */
#define RX_MDOE LOW
#define TX_MODE HIGH
#define setDirPin(DirPin)   (pinMode(DirPin,OUTPUT))			/* Select the Switch to TX/RX Mode Pin */
#define comMode(Mode) (digitalWrite(DXL_DIRECTION_PIN, Mode))	/* Switch to Rx or Tx Mode */

/* basic structure for Dynamixel control table */
typedef struct {
	const uint8_t	adress;
	const uint8_t	access;
	const uint16_t	min_val;
	const uint16_t	max_val;
} dxl_t;

/* Basic structure of the function pointer table */
typedef struct
{
	int16_t(*funcPtr)(uint8_t id, int16_t val);
	const char*		funcName;
} funcPtrTbl_t;

/* 
 * Setup function for the Dynamixel servos
 */
void InitDynamixel(void);

/* 
 * Handler function for Dynamixel-specific error codes. If an error code is reported to
 * the function, it will issue an error message. In addition, the execution of the main loop
 * is stopped to prevent inadvertent behavior
 */
void DynamixelError(int16_t errorBit, uint8_t id);

/* 
 * Cyclic function for detecting the move status of the system and if necessary executing
 * move commands of the Dynamixel servos
 */
void HandleMove(void);

/* 
 * Opening serial connection for Dynamixel servos and set communication direction pin
 */
void DxlStartCom(uint16_t baudRate, uint8_t directionPin);

#endif