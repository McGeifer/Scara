// objdir.h

#ifndef _OBJDIR_h
#define _OBJDIR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// ##############################################
// object dictionary
// ##############################################

// The basic data structure for the object dictionary.
typedef struct {
	const	uint8_t		idx;				// index number of object
	const	uint8_t		props;				// object properties
			int16_t	data;					// object data --- 0xFFFF is reserved for error handling ###
			int			(*pFunction)(const uint8_t* idx, const uint8_t* props, const int16_t* data);	// pointer for handler function
} objStruct_t;

// The basic data structure for the tool table.
typedef struct {
	const	uint8_t		toolIdx;			// index number of tool
	const	uint8_t		props;				// object properties
			int16_t		offsetX;			// x offset
			int16_t		offsetY;			// y offset
			int16_t		offsetZ;			// z offset
			bool		active;				// tool is active?
			int			(*pFunction)(const uint8_t* toolIndex); // pointer for handler function
} toolTbl_t;

// Register to store position values
typedef struct {
			uint8_t		pointIdx;
			uint8_t		props;
			int16_t		posRegX;
			int16_t		posRegY;
			int16_t		posRegZ;
} posReg_t;

// ##############################################
// modbus
// ##############################################

// modbus holing registers
enum holding_registers {
	INDEX_MDB,				// The first register starts at address 0
	DATA_MDB,
	CRC_MDB,
	TOTAL_ERRORS_MDB,		// Leave this one
	TOTAL_REGS_SIZE_MDB		// Total number of registers. Function 3 and 16 share the same register array
};

// ##############################################
// object indices
// ##############################################

// basic options
#define OBJ_IDX_ACK						0xF0		// acknowledge
#define OBJ_IDX_START_MOVE				0xF1		// start movement of robot axis

// internal objects
#define OBJ_IDX_OP_MODE					0xFE		// operation mode: modbus = 1, rapid = 2 or scara = 3
#define OBJ_IDX_SYS_STATUS				0xFF		// system status

// position values
#define OBJ_IDX_X_NEW_TARGET_POS		0x10		// x - new target position
#define OBJ_IDX_X_ACTUAL_TARGET_POS		0x11		// x - actual target position
#define OBJ_IDX_X_ACTUAL_POS			0x12		// x - actual position
#define OBJ_IDX_Y_NEW_TARGET_POS		0x20		// y - new target position
#define OBJ_IDX_Y_ACTUAL_TARGET_POS		0x21		// y - actual target position
#define OBJ_IDX_Y_ACTUAL_POS			0x22		// y - actual position
#define OBJ_IDX_Z_NEW_TARGET_POS		0x30		// z - new target position
#define OBJ_IDX_Z_ACTUAL_TARGET_POS		0x31		// z - actual target position
#define OBJ_IDX_Z_ACTUAL_POS			0x32		// z - actual position											


// ##############################################
// object properties
// ##############################################
#define OBJ_READ	0x10
#define OBJ_WRITE	0x20

#define OBJ_PROP_R_	(OBJ_READ)				// read only
#define OBJ_PROP__W	(OBJ_WRITE)				// write only
#define OBJ_PROP_RW	(OBJ_READ | OBJ_WRITE)	// read & write

// ##############################################
// operation modes
// ##############################################
#define OP_MODE_MODBUS				0x01	// use SimpleModbusClient
#define OP_MODE_RAPID				0x02	// use "Rapid" protocoll
#define OP_MODE_SCARA				0x03	// use "Scara" protocoll

// ##############################################
// status message types
// ##############################################
#define STATUS_TYPE_NOTYPE			0x01	// display a message without status type (try not to use)
#define STATUS_TYPE_INFO			0x02	// display normal info message
#define STATUS_TYPE_WARNING			0x03	// display a warning message
#define STATUS_TYPE_ERROR			0x04	// display a error message
#define STATUS_TYPE_DEBUG			0x05	// display debug info

// ##############################################
// system status
// ##############################################
#define SYS_STAT_RUNNING			0x01	// no error - system operational
#define SYS_STAT_DYNAMIXEL_ERROR	0x02	// not all 3 dynamixel servo motors could be found
#define SYS_STAT_DEBUG				0x04	// print additional debug info 
#define SYS_STAT_UNKOWN_ERROR		0x08	// uknown error - not used jet
#define SYS_STAT_SILENT				0x10	// no messages will be send
#define SYS_STAT_ERROR (SYS_STAT_DYNAMIXEL_ERROR | SYS_STAT_UNKOWN_ERROR) // system is in error state


				

// ##############################################
// object dictionary min / max position and angle values
// ##############################################
#define X_POS_MIN 					1			
#define X_POS_MAX					200			
#define Y_POS_MIN					1			
#define Y_POS_MAX					200			
#define Z_POS_MIN					1			
#define Z_POS_MAX					200			

#define AXIS_1_ANGLE_MIN			-105	
#define AXIS_1_ANGLE_MAX			105		
#define AXIS_2_ANGLE_MIN			-105	
#define AXIS_2_ANGLE_MAX			105		

// ##############################################
// object dictionary min / max speed values
// ##############################################
#define X_SPEED_MIN 1											
#define X_SPEED_MAX 255
#define Y_SPEED_MIN 1											
#define Y_SPEED_MAX 255
#define Z_SPEED_MIN 1											
#define Z_SPEED_MAX 255

#define AXIS_1_SPEED_MIN 1
#define AXIS_1_SPEED_MAX 255
#define AXIS_2_SPEED_MIN 1
#define AXIS_2_SPEED_MAX 255

// ##############################################
// miscellaneous
// ##############################################
#define PosArrayLength 64

// ##############################################
// function declarations
// ##############################################

// Function to return the x, y & z values for a given index stored in the position register.
int16_t* GetPosRegData(uint8_t *idx);

// Funtion for writing data to the position register.
uint8_t SetPosRegData(uint8_t *idx, int16_t *xValue, int16_t *yValue, int16_t *zValue);

// Function to return the stored data of an object.
uint16_t GetObjStructData(uint8_t index);

// Funtion for writing data to the object dictionary.
// Warning! Make sure the calling function only bypasses valid data, this function will not check it. !?!?????????????????????????????!?!??!?!?!?!?!?!?!?
int8_t SetObjStructData(uint8_t index, int16_t data);

// Function to return the stored offset values for a given tool.
int16_t* GetToolTblData(uint8_t index);

// Function to search the position register. If the given index is found it returns TRUE if not FALSE.
//posReg_t* LocatePos(uint8_t *idx);

// Function to search the object dictionary. If the given entry is found it returns a pointer to it, if not it returns a NULL pointer.
//objStruct_t* LocateObj(uint8_t index);

// Function to search the tool table. If the given entry is found it returns a pointer to it, if not it returns a NULL pointer.
//toolTbl_t* LocateTool(uint8_t index);

#endif