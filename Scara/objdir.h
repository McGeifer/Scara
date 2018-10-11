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
			uint16_t	data;				// object data
	const	int			(*pFunction)(uint8_t*, uint8_t*, uint16_t*);	// pointer for handler function
} ObjStruct;

// object properties
#define OBJ_READ	0x10
#define OBJ_WRITE	0x20

#define OBJ_PROP_R_	(OBJ_READ)				// read only
#define OBJ_PROP__W	(OBJ_WRITE)				// write only
#define OBJ_PROP_RW	(OBJ_READ | OBJ_WRITE)	// read & write

// object index
// basic comands
#define OBJ_IDX_ACK				0xA0	// acknowledge
#define OBJ_IDX_START_MOVE		0x3F	// start movement of robot axis
// position values
#define OBJ_IDX_X_NEW_TARGET	0x10	// x - new target position
#define OBJ_IDX_X_ACT_TARGET	0x11	// x - actual target position
#define OBJ_IDX_X_ACT			0x12	// x - actual position
#define OBJ_IDX_Y_NEW_TARGET	0x20	// y - new target position
#define OBJ_IDX_Y_ACT_TARGET	0x21	// y - actual target position
#define OBJ_IDX_Y_ACT			0x22	// y - actual position
#define OBJ_IDX_Z_NEW_TARGET	0x30	// z - new target position
#define OBJ_IDX_Z_ACT_TARGET	0x31	// z - actual target position
#define OBJ_IDX_Z_ACT			0x32	// z - actual position
// angle values
#define OBJ_IDX_AX1_NEW_TARGET	0x40	// axis 1 - new target angle
#define OBJ_IDX_AX1_ACT_TARGET	0x41	// axis 1 - actual target angle
#define OBJ_IDX_AX1_ACT_		0x42	// axis 1 - actual angle
#define OBJ_IDX_AX2_NEW_TARGET	0x50	// axis 1 - new target angle
#define OBJ_IDX_AX2_ACT_TARGET	0x51	// axis 1 - actual target angle
#define OBJ_IDX_AX2_ACT_		0x52	// axis 1 - actual angle

// function declarations
ObjStruct* LocateObjDir(uint8_t index, uint8_t props, uint16_t data);

#endif