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

// Tool table to store offset values for different tools.
typedef struct {
	const	uint8_t		toolIndex;			// indexnumber of tool
	const	uint8_t		props;				// object properties
			uint16_t	offsetX;			// x offset
			uint16_t	offsetY;			// y offset
			uint16_t	offsetZ;			// z offset
			bool		toolAct;			// tool ist active?
}ToolTbl;

// object properties
#define OBJ_READ	0x10
#define OBJ_WRITE	0x20

#define OBJ_PROP_R_	(OBJ_READ)				// read only
#define OBJ_PROP__W	(OBJ_WRITE)				// write only
#define OBJ_PROP_RW	(OBJ_READ | OBJ_WRITE)	// read & write

// function declarations
ObjStruct* LocateObjDir(uint8_t index, uint8_t props, uint16_t data);
ToolTbl* LocateTool(uint8_t index, uint8_t props, uint16_t data);

#endif