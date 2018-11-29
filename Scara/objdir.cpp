// 
// 
// 

#include "objdir.h"
#include "handler.h"
#include "status.h"

// Initial definition of the object dictionary entries.
static ObjStruct_t objStruct_data[] = {

	// basic options
	{0xF0, OBJ_PROP__W, 0, &SetACK},				// acknowledge
	{0xF1, OBJ_PROP__W, 0, &SetStartMove},			// start movement of robot axis

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

	// speed values
	{0x60, OBJ_PROP__W, 0, NULL},					// x - new target speed
	{0x61, OBJ_PROP_R_, 0, NULL},					// x - actual speed
	{0x70, OBJ_PROP__W, 0, NULL},					// y - new target speed
	{0x71, OBJ_PROP_R_, 0, NULL},					// y - actual speed
	{0x80, OBJ_PROP__W, 0, NULL},					// z - new target speed
	{0x81, OBJ_PROP_R_, 0, NULL},					// z - actual speed
	{0x90, OBJ_PROP__W, 0, NULL},					// axis 1 - new target speed
	{0x91, OBJ_PROP_R_, 0, NULL},					// axis 1 - actual speed
	{0xA0, OBJ_PROP__W, 0, NULL},					// axis 2 - new target speed
	{0xA1, OBJ_PROP_R_, 0, NULL},					// axis 2 - actual speed
};

// Initial definition of the tool table.
static ToolTbl_t toolTbl[] = {

	// tool 0 - machine zero
	{0x01, OBJ_PROP_R_, 0, 0, 0, true, NULL},		// only change if you really know what you are doing!
	
	// variable tooltable entries
	{0x10, OBJ_PROP_R_, 0, 0, 0, false, &GetTool},	// custom tools
	{0x11, OBJ_PROP__W, 0, 0, 0, false, &SetTool},
	{0x20, OBJ_PROP_R_, 0, 0, 0, false, &GetTool},
	{0x21, OBJ_PROP__W, 0, 0, 0, false, &SetTool},
	{0x30, OBJ_PROP_R_, 0, 0, 0, false, &GetTool},
	{0x31, OBJ_PROP__W, 0, 0, 0, false, &SetTool},
	{0x40, OBJ_PROP_R_, 0, 0, 0, false, &GetTool},
	{0x41, OBJ_PROP__W, 0, 0, 0, false, &SetTool},
	{0x50, OBJ_PROP_R_, 0, 0, 0, false, &GetTool},
	{0x51, OBJ_PROP__W, 0, 0, 0, false, &SetTool},
	{0x60, OBJ_PROP_R_, 0, 0, 0, false, &GetTool},
	{0x61, OBJ_PROP__W, 0, 0, 0, false, &SetTool},
	{0x70, OBJ_PROP_R_, 0, 0, 0, false, &GetTool},
	{0x71, OBJ_PROP__W, 0, 0, 0, false, &SetTool},
	{0x80, OBJ_PROP_R_, 0, 0, 0, false, &GetTool},
	{0x81, OBJ_PROP__W, 0, 0, 0, false, &SetTool},
	{0x90, OBJ_PROP_R_, 0, 0, 0, false, &GetTool},
	{0x91, OBJ_PROP__W, 0, 0, 0, false, &SetTool},
};

// Static position register
static PosReg_t posRegStatic[] = {

	// fixed positions 240 - 255 - only change if you really know what you are doing!
	// to keep the
	{0xF0, OBJ_PROP_R_, 0, 0, 0},			// homing position
	{0xF1, OBJ_PROP_R_, 0, 0, 0},			// pos of red "coin" 1
	{0xF2, OBJ_PROP_R_, 0, 0, 0},			// pos of red "coin" 2
	{0xF3, OBJ_PROP_R_, 0, 0, 0},			// pos of red "coin" 3
	{0xF4, OBJ_PROP_R_, 0, 0, 0},			// pos of red "coin" 4
	{0xF5, OBJ_PROP_R_, 0, 0, 0},			// pos of red "coin" 5
	{0xF6, OBJ_PROP_R_, 0, 0, 0},			// pos of green "coin" 1
	{0xF7, OBJ_PROP_R_, 0, 0, 0},			// pos of green "coin" 2
	{0xF8, OBJ_PROP_R_, 0, 0, 0},			// pos of green "coin" 3
	{0xF9, OBJ_PROP_R_, 0, 0, 0},			// pos of green "coin" 4
	{0xFA, OBJ_PROP_R_, 0, 0, 0},			// pos of green "coin" 5

	// positions 0 - 63 (0x3F) - dynamicly allocated
};

// dynamic position register
static PosReg_t *pArray[PosArrayLength] = { NULL };
// index of the last position in the position register
static uint8_t idxLastPos = 0;

// ##############################################
// pos register - help functions
// ##############################################

static PosReg_t* LocatePos(uint8_t *idx) {

	uint8_t i = 0;

	for (i = 0; i = PosArrayLength - 1; i++) {
		if(pArray[i]->pointIdx == *idx)
			return pArray[i];
	}
	return NULL;
}

uint16_t* GetPosRegData(uint8_t *idx) {

	PosReg_t *p;
	p = LocatePos(idx);

	if (p != NULL) {
		static uint16_t data[3];
		data[0] = p->posRegX;
		data[1] = p->posRegY;
		data[2] = p->posRegZ;
		return data;
	}
	else
		return NULL;
}

uint8_t SetPosRegData(uint8_t *idx, uint8_t *xValue, uint8_t *yValue, uint8_t *zValue) {

	PosReg_t *p;
	p = LocatePos(idx);
	//uint16_t minValue;
	//uint16_t maxValue;

	if (p == NULL) {

		if (*idx <= PosArrayLength - 1) {
			pArray[idxLastPos] = (PosReg_t*)malloc(sizeof(PosReg_t)); // allocate memory for new PosReg entry and store a pointer to in the pArray
			pArray[idxLastPos]->pointIdx = *idx;
			pArray[idxLastPos]->props = OBJ_PROP_RW;
			idxLastPos++;
			p = LocatePos(idx);
		}
		else {
			SendStatus("in function SetPosRegData(): failed to write - max number of points (64) reached ", STATUS_TYPE_ERROR);
			return -1;
		}
	}
	
	if (p != NULL) {
		
		if (p->props == OBJ_PROP_RW || p->props == OBJ_PROP__W) {	// check if object is writable

			if (*xValue >= X_POS_MIN && *xValue <= X_POS_MAX) { // check the permissable value range

				if (*yValue >= Y_POS_MIN && *yValue <= Y_POS_MAX) {

					if (*zValue >= Z_POS_MIN && *zValue <= Z_POS_MAX) {
						p->posRegX = *xValue;
						p->posRegY = *yValue;
						p->posRegZ = *zValue;
						return 0;
					}
				}
			}
			else {
				char string[128];
				sprintf(string, "in function SetPosRegData(): failed to write - position P%i value out of range", idx);
				SendStatus(string, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else {
			char string[128];
			sprintf(string, "in function SetPosRegData(): failed to write - position P%i is read only", idx);
			SendStatus(string, STATUS_TYPE_ERROR);
			return -1;
		}
	}
	else {
		SendStatus("in function SetPosRegData(): failed to write - unknown error", STATUS_TYPE_ERROR);
		return -1;
	}
}

// ##############################################
// objDir - help functions
// ##############################################

static ObjStruct_t* LocateObj(uint8_t index) {
	
	ObjStruct_t *p;
	p = objStruct_data;

	for (int i = 0; i < (sizeof(objStruct_data) / sizeof(objStruct_data[0])); i++, p++)	{
		if (p->idx == index)
			return p;
	}
	return NULL;
}

uint16_t GetObjStructData(uint8_t index) {

	ObjStruct_t *p;
	p = LocateObj(index);

	if (p != NULL)
		return p->data;
	else
		return NULL;
}

int8_t SetObjStructData(uint8_t index, uint16_t data) {

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!! Muss ggf. Handlerfunktion ausführen !!!!!!!!!!!!!!!!!!!!!!! falls Fehler in Handler -> return -1:


	ObjStruct_t *p;
	p = LocateObj(index);
	uint16_t minValue;
	uint16_t maxValue;

	if (p != NULL) {	// make sure object does exist

		if (p->props == OBJ_PROP_RW || p->props == OBJ_PROP__W) {	// check if object is writable

			switch (p->idx)	// set min max vaules for comparison
			{
				case 0x10:
					minValue = X_POS_MIN;
					maxValue = X_POS_MAX;
					break;

				case 0x20:
					minValue = Y_POS_MIN;
					maxValue = Y_POS_MAX;
					break;

				case 0x30:
					minValue = Z_POS_MIN;
					maxValue = Z_POS_MAX;
					break;

				case 0x40:
					minValue = AXIS_1_ANGLE_MIN;
					maxValue = AXIS_1_ANGLE_MAX;
					break;

				case 0x50:
					minValue = AXIS_2_ANGLE_MIN;
					maxValue = AXIS_2_ANGLE_MAX;
					break;

				case 0x60:
					minValue = X_SPEED_MIN;
					maxValue = X_SPEED_MAX;
					break;

				case 0x70:
					minValue = Y_SPEED_MIN;
					maxValue = Y_SPEED_MAX;
					break;

				case 0x80:
					minValue = Z_SPEED_MIN;
					maxValue = Z_SPEED_MAX;
					break;

				case 0x90:
					minValue = AXIS_1_SPEED_MIN;
					maxValue = AXIS_1_SPEED_MAX;
					break;

				case 0xA0:
					minValue = AXIS_2_SPEED_MIN;
					maxValue = AXIS_2_SPEED_MAX;
					break;

				default:
					minValue = 0x0000;
					maxValue = 0xFFFF;
					break;
			}
			if (p->data >= minValue &&  p->data <= maxValue) { //  write data if it's inside the allowed range
				p->data = data;
				return 0;
			}
			else {
				char string[128];
				sprintf(string, "in function SetObjStructData(): failed to write - object 0x%x value out of range", index);
				SendStatus(string, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else {
			char string[128];
			sprintf(string, "in function SetObjStructData(): failed to write - object 0x%x is read only", index);
			SendStatus(string, STATUS_TYPE_ERROR);
			return -1;
		}
	}
	else {
		char string[128];
		sprintf(string, "in function SetObjStructData(): failed to write - object 0x%x does not exist", index);
		SendStatus(string, STATUS_TYPE_ERROR);
		return -1;
	}
}

// ##############################################
// tool table - help functions
// ##############################################

static ToolTbl_t* LocateTool(uint8_t index) {

	ToolTbl_t *p;
	p = toolTbl;

	for (int i = 0; i < (sizeof(toolTbl) / sizeof(toolTbl[0])); i++, p++) {
		if (p->toolIdx == index)
			return p;
	}
	return NULL;
}

int16_t* GetToolTblData(uint8_t index) {

	ToolTbl_t *p;
	p = LocateTool(index);

	if (p != NULL) {
		static int16_t data[4];
		data[0] = p->offsetX;
		data[1] = p->offsetY;
		data[2] = p->offsetZ;
		data[3] = p->active;
		return data;
	}
	else
		return NULL;
}