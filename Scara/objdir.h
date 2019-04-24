/* objdir.h */

#ifndef _OBJDIR_h
#define _OBJDIR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/* data structure for the object dictionary */
typedef struct {
	const	uint8_t		idx;						/* idx number of object */
			uint8_t		props;						/* object properties */
			int16_t		data;						/* object data */
			int8_t		(*p)(uint8_t, int16_t);		/* pointer for handler function*/
} objDir_t;

/* data structure for the tool table */
typedef struct {
	const	uint8_t		tool_idx;					/* idx number of tool */
	const	uint8_t		props;						/* object properties */
			int16_t		x_offset;					/* x offset */
			int16_t		y_offset;					/* y offset */
			int16_t		z_offset;					/* z offset */
			bool		active;						/* tool is active */
} toolTbl_t;

/* data structure to store position values */
typedef struct {
			uint8_t		point_idx;
			uint8_t		props;
			int16_t		x_pos;
			int16_t		y_pos;
			int16_t		z_pos;
} posReg_t;


/* modbus holding registers */
enum modbusHoldingReg {
	INDEX_MDB,										/* the first register starts at address 0 */
	DATA_MDB,										/* data  */
	CRC_MDB,										/* crc value */
	TOTAL_ERRORS_MDB,								/* leave this one */
	TOTAL_REGS_SIZE_MDB								/* total number of registers. Function 3 and 16 share the same register array */
};

/* object idx list */
/* basic options */
#define OBJ_IDX_ACK							0xF0	/* acknowledge */
#define OBJ_IDX_START_MOVE					0xF1	/* start movement of robot axis */
#define OBJ_IDX_MOVING						0xF2	/* system is moving */
#define OBJ_IDX_POS_REACHED					0xF3	/* target position reached */

/* internal objects */
#define OBJ_IDX_Z_POS_COUNT					0xFD	/* z - light barrier counter */
#define OBJ_IDX_OP_MODE						0xFE	/* system operation mode */
#define OBJ_IDX_SYS_STATUS					0xFF	/* system status - bit-mask !  */

/* position values */
#define OBJ_IDX_X_NEW_TARGET_POS			0x10	/* x - new target position */
#define OBJ_IDX_X_ACTUAL_TARGET_POS			0x11	/* x - actual target position */
#define OBJ_IDX_X_ACTUAL_POS				0x12	/* x - actual position */
#define OBJ_IDX_Y_NEW_TARGET_POS			0x20	/* y - new target position */
#define OBJ_IDX_Y_ACTUAL_TARGET_POS			0x21	/* y - actual target position */
#define OBJ_IDX_Y_ACTUAL_POS				0x22	/* y - actual position */
#define OBJ_IDX_Z_NEW_TARGET_POS			0x30	/* z - new target position */
#define OBJ_IDX_Z_ACTUAL_TARGET_POS			0x31	/* z - actual target position */
#define OBJ_IDX_Z_ACTUAL_POS				0x32	/* z - actual position */								

/* angle values */
#define OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE		0x40	/* axis 1 - new target angle */
#define OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE	0x41	/* axis 1 - actual target angle */
#define OBJ_IDX_AXIS_1_ACTUAL_ANGLE			0x42	/* axis 1 - actual angle */
#define OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE		0x50	/* axis 2 - new target angle */
#define OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE	0x51	/* axis 2 - actual target angle */
#define OBJ_IDX_AXIS_2_ACTUAL_ANGLE			0x52	/* axis 2 - actual angle */

/* speed values */
#define OBJ_IDX_X_NEW_TARGET_SPEED			0x60	/* x - new target speed */
#define OBJ_IDX_X_ACTUAL_TARGET_SPEED		0x61	/* x - actual target speed */
#define OBJ_IDX_X_ACTUAL_SPEED				0x62	/* x - actual speed */
#define OBJ_IDX_Y_NEW_TARGET_SPEED			0x70	/* y - new target speed */
#define OBJ_IDX_Y_ACTUAL_TARGET_SPEED		0x71	/* y - actual target speed */
#define OBJ_IDX_Y_ACTUAL_SPEED				0x72	/* y - actual speed */
#define OBJ_IDX_Z_NEW_TARGET_SPEED			0x80	/* z - new target speed */
#define OBJ_IDX_Z_ACTUAL_TARGET_SPEED		0x81	/* z - actual target speed */
#define OBJ_IDX_Z_ACTUAL_SPEED				0x82	/* z - actual speed */
#define OBJ_IDX_AXIS_1_NEW_TARGET_SPEED		0x90	/* axis 1 - new target speed */
#define OBJ_IDX_AXIS_1_ACTUAL_TARGET_SPEED	0x91	/* axis 1 - actual target speed */
#define OBJ_IDX_AXIS_1_ACTUAL_SPEED			0x92	/* axis 1 - actual speed */
#define OBJ_IDX_AXIS_2_NEW_TARGET_SPEED		0xA0	/* axis 2 - new target speed */
#define OBJ_IDX_AXIS_2_ACTUAL_TARGET_SPEED	0xA1	/* axis 2 - actual target speed */
#define OBJ_IDX_AXIS_2_ACTUAL_SPEED			0xA2	/* axis 2 - actual speed */

/* tools */
#define OBJ_IDX_TOOL_0						0x00	/* tool 0 - machine zero - only change if you really know what you are doing! */
#define OBJ_IDX_TOOL_1						0x01	/* custom tool 1 - 9 */
#define OBJ_IDX_TOOL_2						0x02	
#define OBJ_IDX_TOOL_3						0x03	
#define OBJ_IDX_TOOL_4						0x04	
#define OBJ_IDX_TOOL_5						0x05	
#define OBJ_IDX_TOOL_6						0x06	
#define OBJ_IDX_TOOL_7						0x07	
#define OBJ_IDX_TOOL_8						0x08	
#define OBJ_IDX_TOOL_9						0x09	

/* static positions - only change if you really know what you are doing! */
#define OBJ_IDX_STAT_POS_HOME				0x00	
#define OBJ_IDX_STAT_POS_RD_COIN_1			0x01	
#define OBJ_IDX_STAT_POS_RD_COIN_2			0x02	
#define OBJ_IDX_STAT_POS_RD_COIN_3			0x03	
#define OBJ_IDX_STAT_POS_RD_COIN_4			0x04	
#define OBJ_IDX_STAT_POS_RD_COIN_5			0x05	
#define OBJ_IDX_STAT_POS_GN_COIN_1			0x06	
#define OBJ_IDX_STAT_POS_GN_COIN_2			0x07	
#define OBJ_IDX_STAT_POS_GN_COIN_3			0x08	
#define OBJ_IDX_STAT_POS_GN_COIN_4			0x09	
#define OBJ_IDX_STAT_POS_GN_COIN_5			0x0A	

/* object properties */
#define OBJ_PROP_R_							0x10	/* read only */
#define OBJ_PROP__W							0x20	/* write only */
#define OBJ_PROP_RW					(0x10 | 0x20)	/* read & write */

/* operation modes */
#define OP_MODE_MODBUS						0x01	/* operation mode modbus - use SimpleModbusClient */
#define OP_MODE_RAPID						0x02	/* operation mode rapid - use "Rapid" protocol */
#define OP_MODE_SCARA						0x03	/* operation mode scara - use "Scara" protocol */

/* status message types */
#define SYS_STAT_MSG_TYPE_NONE				0x01	/* display a message without status type (try not to use) */
#define SYS_STAT_MSG_TYPE_INFO				0x02	/* display normal info message */
#define SYS_STAT_MSG_TYPE_WARNING			0x03	/* display a warning message */
#define SYS_STAT_MSG_TYPE_ERROR				0x04	/* display a error message */
#define SYS_STAT_MSG_TYPE_DEBUG				0x05	/* display debug info */

/* system status */
#define SYS_STAT_RUNNING					0x01	/* no error - system operational */
#define SYS_STAT_DYNAMIXEL_ERROR			0x02	/* error  - dynamixel servo motors */
#define SYS_STAT_DEBUG						0x04	/* print additional debug info  */
#define SYS_STAT_UNKOWN_ERROR				0x08	/* unknown error - not used jet */
#define SYS_STAT_SILENT						0x10	/* no messages will be send */
#define SYS_STAT_ERROR (SYS_STAT_DYNAMIXEL_ERROR | SYS_STAT_UNKOWN_ERROR) /* system in error state */

/* min/ max position/ angle values and tolerances */
#define X_POS_MIN 							0
#define X_POS_MAX							2000
#define Y_POS_MIN							0
#define Y_POS_MAX							2000
#define Z_POS_MIN							0
#define Z_POS_MAX							2000

#define AXIS_1_ANGLE_MIN					600
#define AXIS_1_ANGLE_MAX					2400
#define AXIS_2_ANGLE_MIN					600
#define AXIS_2_ANGLE_MAX					2400

/* object dir - min/ max speed values */
#define X_SPEED_MIN							1
#define X_SPEED_MAX							255
#define Y_SPEED_MIN							1
#define Y_SPEED_MAX							255
#define Z_SPEED_MIN							1
#define Z_SPEED_MAX							255

#define AXIS_1_SPEED_MIN					1
#define AXIS_1_SPEED_MAX					255
#define AXIS_2_SPEED_MIN					1
#define AXIS_2_SPEED_MAX					255

/* miscellaneous */
#define PosArrayLength						64

/* mechanical parameters */
#define Z_GRADIENT							8.0		/* gradient of the z axis spindle */
#define Z_RESOLUTION						16.0	/* number of impulses for a whole turn of the spindle */
#define Z_LENGHT							200.0	/* length of z axis in mm */
#define AXIS_1_LENGTH						195.0
#define AXIS_2_LENGTH						200.0

/* 
	Function to return the x, y & z values for a given position idx stored in the position register
 */
int16_t* GetPosRegData(uint8_t *idx);

/*
	Function for writing data to the position register
 */
uint8_t SetPosRegData(uint8_t *idx, int16_t *x_pos, int16_t *y_pos, int16_t *z_pos);

/*
	Function to return the stored data of an object
 */
int16_t GetObjData(uint8_t idx);

/* 
	Function for writing data to the object dictionary. Objects can only be written if they have the property "writable". 
	This mechanism can be disabled if the parameter internal_call is set to "true" (use with caution!)
 */
int8_t SetObjData(uint8_t idx, int16_t data, bool internal_call);

/*
	Write "actualAngles" (rad) in obj_dir.
	return	0 = operation successful
	return -1 = operation failed
 */
int8_t SetActualAngles(float *servo_1, float *servo_2);

/* 
	Write "actualTargetAngles" (rad) in obj_dir.
	return	0 = operation successful
	return -1 = operation failed
 */
int8_t SetActualTargetAngles(float *servo_1, float *servo_2);

/*
	Write "newTargetAngles" (rad) in obj_dir.
	return	0 = operation successful
	return -1 = operation failed
 */
int8_t SetNewTargetAngles(float *servo_1, float *servo_2);

/*
	Write "actualPos" (mm) in obj_dir.
	return	0 = operation successful
	return -1 = operation failed
 */
int8_t SetActualPositions(float *x_pos, float *y_pos);

/* 
	Write "actualTargetPos" (mm) in obj_dir.
	return	0 = operation successful
	return -1 = operation failed
 */
int8_t SetActualTargetPositions(float *x_pos, float *y_pos);

/* 
	Write "newTargetPos" (mm) in obj_dir.
	return	0 = operation successful
	return -1 = operation failed
 */
int8_t SetNewTargetPositions(float *x_pos, float *y_pos);

/*
	Function to return the stored offset values for a given tool
 */
int16_t* GetToolData(uint8_t idx);

/*
	Function to search the tool table. If the given entry is found it returns a pointer to it, if not it returns a NULL pointer
 */
toolTbl_t* LocateTool(uint8_t idx);

/*
	Cyclic function for updating all actual position, angle & speed values of the obj_dir
 */
void UpdateOD(void);


#endif