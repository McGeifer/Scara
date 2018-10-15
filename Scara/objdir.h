// objdir.h

#ifndef _OBJDIR_h
#define _OBJDIR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// The basic data structure for the object dictionary.
typedef struct {

	const	uint8_t		index;				// indexnumber of object
	const	uint8_t		props;				// object properties
			uint16_t	data;				// object data ### 0xFFFF is reserved for error handling ###
	const	int			(*pFunction)(uint8_t* index, uint8_t* props, uint16_t* data);	// pointer for handler function
} ObjStruct;

// Tool table to store offset values for different tools.
typedef struct {

	const	uint8_t		toolIndex;			// indexnumber of tool
	const	uint8_t		props;				// object properties
			uint16_t	offsetX;			// x offset
			uint16_t	offsetY;			// y offset
			uint16_t	offsetZ;			// z offset
			bool		toolAct;			// tool ist active?
} ToolTbl;

// Modbus holing registers
enum holding_registers {

	CMD_MODBUS,					// The first register starts at address 0
	PARA_MODBUS,
	DATA1_MODBUS,
	DATA2_MODBUS,
	CRC_MODBUS,
	TOTAL_ERRORS_MODBUS,		// Leave this one
	TOTAL_REGS_SIZE_MODBUS		// Total number of registers. Function 3 and 16 share the same register array
};

// Function 3 and 16 register array
static unsigned int holdingRegs[TOTAL_REGS_SIZE_MODBUS];

// ##############################################
// object properties
// ##############################################
#define OBJ_READ	0x10
#define OBJ_WRITE	0x20

#define OBJ_PROP_R_	(OBJ_READ)				// read only
#define OBJ_PROP__W	(OBJ_WRITE)				// write only
#define OBJ_PROP_RW	(OBJ_READ | OBJ_WRITE)	// read & write

// ##############################################
// Operation modes
// ##############################################
#define OP_MODE_SCARA	0x01				// use "Scara" protocoll
#define OP_MODE_MODBUS	0x02				// use SimpleModbusClient
#define OP_MODE_RAPID	0x03				// use "Rapid" protocoll

// ##############################################
// Status message types
// ##############################################
//#define STATUS_TYPE_NOTYPE		0x01		// display a message without status type
#define STATUS_TYPE_INFO		0x02		// display normal info message
#define STATUS_TYPE_WARNING		0x03		// display a warning message
#define STATUS_TYPE_ERROR		0x04		// display a error message

// ##############################################
// min max values for the object dictionary
// ##############################################

// basic options
#define OP_MODE 

// position values
#define X_NEW_TARGET_POS_MIN -200
#define X_NEW_TARGET_POS_MAX 200
#define Y_NEW_TARGET_POS_MIN -200
#define Y_NEW_TARGET_POS_MAX 200
#define Z_NEW_TARGET_POS_MIN -200
#define Z_NEW_TARGET_POS_MAX 200

// angle values
#define AXIS_1_NEW_TARGET_ANGLE_MIN -105
#define AXIS_1_NEW_TARGET_ANGLE_MAX 105
#define AXIS_2_NEW_TARGET_ANGLE_MIN -105
#define AXIS_2_NEW_TARGET_ANGLE_MAX 105

// ##############################################
// function declarations
// ##############################################

// Function to search the object dictionary. If the given entry is found it returns a pointer to it, if not it returns a NULL pointer.
ObjStruct* LocateObj(uint8_t index);

// Function to search the tool table. If the given entry is found it returns a pointer to it, if not it returns a NULL pointer.  
ToolTbl* LocateTool(uint8_t index);

// Function to return the stored data of an object dictionary entry
int16_t GetObjStructData(uint8_t index);

// Funtion for writing data to the object dictionary
// Warning! This function will not check the data, make sure the calling function only bypasses valid write requests and data.
void SetObjStructData(uint8_t index, int16_t data);

#endif