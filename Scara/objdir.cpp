// 
// 
// 

#include "objdir.h"
#include "handler.h"
#include "status.h"

// Initial definition of the object dictionary entries.
static ObjStruct objStruct_data[] = {

	// basic options
	{0xA0, OBJ_PROP__W, 0, &SetACK},				// acknowledge
	{0x3F, OBJ_PROP__W, 0, &SetStartMove},			// start movement of robot axis

	// internal objects
	{0xFE, OBJ_PROP_RW, 0, NULL},					// operation mode: modbus = 1, rapid = 2 or scara = 3
	{0xFF, OBJ_PROP_RW, 0, NULL},					// system status
	
	// position values
	{0x10, OBJ_PROP__W, 0, &SetNewTargetPos},		// x - new target position
	{0x11, OBJ_PROP_R_, 0, &GetActualTargetPos},	// x - actual target position
	{0x12, OBJ_PROP_R_, 0, &GetActualPos},			// x - actual position
	{0x20, OBJ_PROP__W, 0, &SetNewTargetPos},		// y - new target position
	{0x21, OBJ_PROP_R_, 0, &GetActualTargetPos},	// y - actual target position
	{0x22, OBJ_PROP_R_, 0, &GetActualPos},			// y - actual position
	{0x30, OBJ_PROP__W, 0, &SetNewTargetPos},		// z - new target position
	{0x31, OBJ_PROP_R_, 0, &GetActualTargetPos},	// z - actual target position
	{0x32, OBJ_PROP_R_, 0, &GetActualPos},			// z - actual position
	
	// angle values
	{0x40, OBJ_PROP__W, 0, &SetNewTargetAngle},		// axis 1 - new target angle
	{0x41, OBJ_PROP_R_, 0, &GetActualTargetAngle},	// axis 1 - actual target angle
	{0x42, OBJ_PROP_R_, 0, &GetActualAngle},		// axis 1 - actual angle
	{0x50, OBJ_PROP__W, 0, &SetNewTargetAngle},		// axis 1 - new target angle
	{0x51, OBJ_PROP_R_, 0, &GetActualTargetAngle},	// axis 1 - actual target angle
	{0x52, OBJ_PROP_R_, 0, &GetActualAngle},		// axis 1 - actual angle
};

// Initial definition of the tool table.
static ToolTbl toolTbl[] = {

	// tool 0 - machine zero
	{0x01, OBJ_PROP_R_, 0, 0, 0, &GetTool},		// Only change if you really know what you are doing!
	
	// variable tooltable entries
	{0x10, OBJ_PROP_R_, 0, 0, 0, &GetTool},		// custom tools
	{0x11, OBJ_PROP__W, 0, 0, 0, &SetTool},
	{0x20, OBJ_PROP_R_, 0, 0, 0, &GetTool},
	{0x21, OBJ_PROP__W, 0, 0, 0, &SetTool},
	{0x30, OBJ_PROP_R_, 0, 0, 0, &GetTool},
	{0x31, OBJ_PROP__W, 0, 0, 0, &SetTool},
	{0x40, OBJ_PROP_R_, 0, 0, 0, &GetTool},
	{0x41, OBJ_PROP__W, 0, 0, 0, &SetTool},
	{0x50, OBJ_PROP_R_, 0, 0, 0, &GetTool},
	{0x51, OBJ_PROP__W, 0, 0, 0, &SetTool},
	{0x60, OBJ_PROP_R_, 0, 0, 0, &GetTool},
	{0x61, OBJ_PROP__W, 0, 0, 0, &SetTool},
	{0x70, OBJ_PROP_R_, 0, 0, 0, &GetTool},
	{0x71, OBJ_PROP__W, 0, 0, 0, &SetTool},
	{0x80, OBJ_PROP_R_, 0, 0, 0, &GetTool},
	{0x81, OBJ_PROP__W, 0, 0, 0, &SetTool},
	{0x90, OBJ_PROP_R_, 0, 0, 0, &GetTool},
	{0x91, OBJ_PROP__W, 0, 0, 0, &SetTool},
};

ObjStruct* LocateObj(uint8_t index) {
	
	ObjStruct *p;
	p = objStruct_data;

	for (int i = 0; i < (sizeof(objStruct_data) / sizeof(objStruct_data[0])); i++, p++)	{
		if (p->index == index)
			return p;
	}
	return NULL;
}

ToolTbl* LocateTool(uint8_t index) {

	ToolTbl *p;
	p = toolTbl;

	for (int i = 0; i < (sizeof(toolTbl) / sizeof(toolTbl[0])); i++, p++) {
		if (p->toolIndex == index)
			return p;
	}
	return NULL;
}

int16_t GetObjStructData(uint8_t index) {

	ObjStruct *p;
	p = LocateObj(index);

	if (p != NULL) 
		return p->data;
	else
		return NULL;
}

int8_t SetObjStructData(uint8_t index, uint16_t data) {

	ObjStruct *p;
	p = LocateObj(index);
	int16_t minValue;
	int16_t maxValue;

	if (p != NULL) {	// make sure object does exist
		if (p->props == OBJ_PROP_RW || p->props == OBJ_PROP__W) {	// check if object is writable
			switch (p->index)	// set min max vaules for comparison
			{
			case 0x10:
				minValue = X_NEW_TARGET_POS_MIN;
				maxValue = X_NEW_TARGET_POS_MAX;
				break;
			case 0x20:
				minValue = Y_NEW_TARGET_POS_MIN;
				maxValue = Y_NEW_TARGET_POS_MAX;
				break;
			case 0x30:
				minValue = Z_NEW_TARGET_POS_MIN;
				maxValue = Z_NEW_TARGET_POS_MAX;
				break;
			case 0x40:
				minValue = AXIS_1_NEW_TARGET_ANGLE_MIN;
				maxValue = AXIS_1_NEW_TARGET_ANGLE_MAX;
				break;
			case 0x50:
				minValue = AXIS_2_NEW_TARGET_ANGLE_MIN;
				maxValue = AXIS_2_NEW_TARGET_ANGLE_MAX;
				break;
			default:
				minValue = NULL;
				maxValue = NULL;
				break;
			}
			if (p->data >= minValue && p->data <= maxValue) { //  wirte data if it's inside the min max range
				p->data = data;
				return 0;
			}
			else {
				char* string;
				sprintf(string, "in function SetObjStructData(): failed to write - object %h value out of range", index);
				SendStatus(string, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else {
			char* string;
			sprintf(string, "in function SetObjStructData(): failed to write - object %h is read only", index);
			SendStatus(string, STATUS_TYPE_ERROR);
			return -1;
		}
	}
	else {
		char* string;
		sprintf(string, "in function SetObjStructData(): failed to write - object %h does not exist", index);
		SendStatus(string, STATUS_TYPE_ERROR);
		return -1;
	}
}