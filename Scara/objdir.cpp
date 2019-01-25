// 
// 
// 

#include "objdir.h"
#include "status.h"

// object dictionary
static objStruct_t objDir[] = {

	// basic options
	{OBJ_IDX_ACK,							OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_START_MOVE,					OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_MOVING,						OBJ_PROP_R_, 0, NULL},

	// internal objects
	{OBJ_IDX_Z_POS_COUNT,					OBJ_PROP_RW, 0, NULL},
	{OBJ_IDX_OP_MODE,						OBJ_PROP_RW, 0, NULL},
	{OBJ_IDX_SYS_STATUS,					OBJ_PROP_RW, 4, NULL}, /*0x04 for testing debug mode !!!!!!!!!!!!!!!!!!!!!!!!!*/
	
	// position values
	{OBJ_IDX_X_NEW_TARGET_POS,				OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_X_ACTUAL_TARGET_POS,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_X_ACTUAL_POS,					OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Y_NEW_TARGET_POS,				OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_Y_ACTUAL_TARGET_POS,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Y_ACTUAL_POS,					OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Z_NEW_TARGET_POS,				OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_Z_ACTUAL_TARGET_POS,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Z_ACTUAL_POS,					OBJ_PROP_R_, 0, NULL},
	
	// angle values
	{OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE,		OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE,	OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_1_ACTUAL_ANGLE,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE,		OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE,	OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_2_ACTUAL_ANGLE,			OBJ_PROP_R_, 0, NULL},

	// speed values
	{OBJ_IDX_X_NEW_TARGET_SPEED,			OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_X_ACTUAL_TARGET_SPEED,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_X_ACTUAL_SPEED,				OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Y_NEW_TARGET_SPEED,			OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_Y_ACTUAL_TARGET_SPEED,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Y_ACTUAL_SPEED,				OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Z_NEW_TARGET_SPEED,			OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_Z_ACTUAL_TARGET_SPEED,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Z_ACTUAL_SPEED,				OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_1_NEW_TARGET_SPEED,		OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_AXIS_1_ACTUAL_TARGET_SPEED,	OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_1_ACTUAL_SPEED,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_2_NEW_TARGET_SPEED,		OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_AXIS_2_ACTUAL_TARGET_SPEED,	OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_2_ACTUAL_SPEED,			OBJ_PROP_R_, 0, NULL},
};

// tool table
static toolTbl_t toolTbl[] = {

	// tool 0 - machine zero
	{OBJ_IDX_TOOL_0, OBJ_PROP_R_, 0, 0, 0, true, NULL},
	
	// variable tooltable entries
	{OBJ_IDX_TOOL_1, OBJ_PROP_RW, 25, -100, 75, true, NULL},
	{OBJ_IDX_TOOL_2, OBJ_PROP_RW, -50, 50, 25, true, NULL},
	{OBJ_IDX_TOOL_3, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{OBJ_IDX_TOOL_4, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{OBJ_IDX_TOOL_5, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{OBJ_IDX_TOOL_6, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{OBJ_IDX_TOOL_7, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{OBJ_IDX_TOOL_8, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{OBJ_IDX_TOOL_9, OBJ_PROP_RW, 0, 0, 0, false, NULL},
};

// static position register ..................... ist das �berhaupt notwendig!?!?!?!?!?!??!?!?!?!?!
static posReg_t posRegStatic[] = {

	// positions 0 - 63 (0x3F) - dynamicly allocated
	{OBJ_IDX_STAT_POS_HOME,			OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_RD_COIN_1,	OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_RD_COIN_2,	OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_RD_COIN_3,	OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_RD_COIN_4,	OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_RD_COIN_5,	OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_GN_COIN_1,	OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_GN_COIN_2,	OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_GN_COIN_3,	OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_GN_COIN_4,	OBJ_PROP_R_, 0, 0, 0},
	{OBJ_IDX_STAT_POS_GN_COIN_5,	OBJ_PROP_R_, 0, 0, 0},
};

// dynamic position register
static posReg_t *pArray[PosArrayLength] = { NULL };

// index of the last position in the position register
static uint8_t pArrayLastPos = 0;

// ##############################################
// pos register - help functions
// ##############################################

static posReg_t* LocatePos(uint8_t *idx) {

	for (uint8_t i = 0; i < PosArrayLength; i++) {
		if (pArray[i]->pointIdx == *idx) {
			SendStatus("LocatePos() ", "pos found", STATUS_TYPE_DEBUG);
			return pArray[i];
		}
	}
	SendStatus("LocatePos() ", "pos not found", STATUS_TYPE_DEBUG);
	return NULL;
}

int16_t* GetPosRegData(uint8_t *idx) {

	posReg_t *p = NULL;
	p = LocatePos(idx);

	if (p != NULL) {
		static int16_t data[3];
		data[0] = p->posRegX;
		data[1] = p->posRegY;
		data[2] = p->posRegZ;
		return data;
	}
	else {
		return NULL;
	}
}

uint8_t SetPosRegData(uint8_t *idx, int16_t *xValue, int16_t *yValue, int16_t *zValue) {

	posReg_t *p = NULL;
	p = LocatePos(idx);

	if (p == NULL) {

		if (*idx < PosArrayLength) {
			pArray[pArrayLastPos] = (posReg_t*)malloc(sizeof(posReg_t)); // allocate memory for new PosReg entry and store a pointer in the pArray
			pArray[pArrayLastPos]->pointIdx = *idx;
			pArray[pArrayLastPos]->props = OBJ_PROP_RW;
			pArrayLastPos++;
			p = LocatePos(idx);
			if (p == NULL) {
				SendStatus("in function SetPosRegData(): ", "unknown error while writing new position value", STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else {
			SendStatus("in function SetPosRegData(): ", "failed to write - max number of points (64) reached", STATUS_TYPE_ERROR);
			return -1;
		}
	}
	if (p != NULL) {
		
		if (p->props == OBJ_PROP_RW || p->props == OBJ_PROP__W) { // check if object is writable

			if (*xValue >= X_POS_MIN && *xValue <= X_POS_MAX) { // check the permissable value range

				if (*yValue >= Y_POS_MIN && *yValue <= Y_POS_MAX) {

					if (*zValue >= Z_POS_MIN && *zValue <= Z_POS_MAX) {
						p->posRegX = *xValue;
						p->posRegY = *yValue;
						p->posRegZ = *zValue;
						return 0;
					}
					else {
						char msg[64];
						sprintf(msg, "position P%c value out of range", *idx);
						SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_TYPE_ERROR);
						return -1;
					}
				}
				else {
					char msg[64];
					sprintf(msg, "position P%c value out of range", *idx);
					SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_TYPE_ERROR);
					return -1;
				}
			}
			else {
				char msg[64];
				sprintf(msg, "position P%c value out of range", *idx);
				SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else {
			char msg[64];
			sprintf(msg, "position P%c is read only", *idx);
			SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_TYPE_ERROR);
			return -1;
		}
	}
	else {
		SendStatus("in function SetPosRegData(): failed to write - ", "unknown error", STATUS_TYPE_ERROR);
		return -1;
	}
}

// ##############################################
// objDir - help functions
// ##############################################

static objStruct_t* LocateObj(uint8_t index) {
	
	objStruct_t *p = NULL;
	p = objDir;
	
	for (uint8_t i = 0; i < (sizeof(objDir) / sizeof(objDir[0])); i++, p++) {
		if (p->idx == index) {
			return p;
		}
	}
	return NULL;
}

int16_t GetObjData(uint8_t index) {

	objStruct_t *p = NULL;
	p = LocateObj(index);

	if (p != NULL) {

		/*char msg[64];
		uint16_t tmp = p->data;
		sprintf(msg, "return value 0x%x", tmp);
		SendStatus("GetObjData(): ", "test", STATUS_TYPE_INFO);*/
		return p->data;
	}
	else {
		return 0;
	}
}

int8_t SetObjData(uint8_t index, int16_t data) {

	objStruct_t *pObjStruct = NULL;
	pObjStruct = LocateObj(index);
	int16_t minValue = 0;
	int16_t maxValue = 0;

	if (pObjStruct != NULL) { /*make sure object does exist*/

		if (pObjStruct->props == OBJ_PROP_RW || pObjStruct->props == OBJ_PROP__W) {	/*check if object is writable*/

			switch (pObjStruct->idx)	/*set min max vaules for comparison*/
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
				minValue = -32768;
				maxValue = 32767;
				break;
			}
			if (data >= minValue &&  data <= maxValue) {	/* write data if it's inside the allowed range */
				pObjStruct->data = data;
				return 0;
			}
			else {
				char msg[64];
				sprintf(msg, "object 0x%x value out of range", index);
				SendStatus("in function SetObjData(): failed to write - ", msg, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else {
			char msg[64];
			sprintf(msg, "object 0x%x is read only", index);
			SendStatus("in function SetObjData(): failed to write - ", msg, STATUS_TYPE_ERROR);
			return -1;
		}
	}
	else {
		char msg[64];
		sprintf(msg, "object 0x%x does not exist", index);
		SendStatus("in function SetObjData(): failed to write - ", msg, STATUS_TYPE_ERROR);
		return -1;
	}
}

// ##############################################
// tool table - help functions
// ##############################################

toolTbl_t* LocateTool(uint8_t index) {

	toolTbl_t *p = NULL;
	p = toolTbl;
	
	for (uint8_t i = 0; i < (sizeof(toolTbl) / sizeof(toolTbl[0])); i++, p++) {
		if (p->toolIdx == index) {
			char msg[64];
			sprintf(msg, "tool %i found", p->toolIdx);
			SendStatus("LocateTool(): ", msg, STATUS_TYPE_DEBUG);
			return p;
		}
	}
	return NULL;
}

int16_t* GetToolData(uint8_t index) {

	toolTbl_t *p = NULL;
	p = LocateTool(index);

	if (p != NULL) {
		static int16_t data[4];
		data[0] = p->offsetX;
		data[1] = p->offsetY;
		data[2] = p->offsetZ;
		data[3] = p->active;
		char msg[64];
		sprintf(msg, "tool offset values: x: %i, y: %i, z: %i", p->offsetX, p->offsetY, p->offsetZ);
		SendStatus("GetToolData(): ", msg, STATUS_TYPE_DEBUG);
		return data;
	}
	else {
		return NULL;
	}
}