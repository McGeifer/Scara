// 
// 
// 

#include "objdir.h"
#include "handler.h"
#include "status.h"

// object dictionary
static objStruct_t objStruct_data[] = {

	// basic options
	{OBJ_IDX_ACK,					OBJ_PROP__W, 0, &SetACK},
	{OBJ_IDX_START_MOVE,			OBJ_PROP__W, 0, &SetStartMove},

	// internal objects
	{OBJ_IDX_OP_MODE,				OBJ_PROP_RW, 0, NULL},
	{OBJ_IDX_SYS_STATUS,			OBJ_PROP_RW, 0x04, NULL}, // 0x04 for testing debug mode !!!!!!!!!!!!!!!!!!!!!!!!!
	
	// position values
	{OBJ_IDX_X_NEW_TARGET_POS,		OBJ_PROP__W, 0, &SetNewTargetPos},		
	{OBJ_IDX_X_ACTUAL_TARGET_POS,	OBJ_PROP_R_, 0, &GetActualTargetPos},	
	{OBJ_IDX_X_ACTUAL_POS,			OBJ_PROP_R_, 0, &GetActualPos},			
	{OBJ_IDX_Y_NEW_TARGET_POS,		OBJ_PROP__W, 0, &SetNewTargetPos},		
	{OBJ_IDX_Y_ACTUAL_TARGET_POS,	OBJ_PROP_R_, 0, &GetActualTargetPos},	
	{OBJ_IDX_Y_ACTUAL_POS,			OBJ_PROP_R_, 0, &GetActualPos},			
	{OBJ_IDX_Z_NEW_TARGET_POS,		OBJ_PROP__W, 0, &SetNewTargetPos},		
	{OBJ_IDX_Z_ACTUAL_TARGET_POS,	OBJ_PROP_R_, 0, &GetActualTargetPos},	
	{OBJ_IDX_Z_ACTUAL_POS,			OBJ_PROP_R_, 0, &GetActualPos},			
	
	// angle values
	{0x40, OBJ_PROP__W, 0, &SetNewTargetAngle},		// axis 1 - new target angle
	{0x41, OBJ_PROP_R_, 0, &GetActualTargetAngle},	// axis 1 - actual target angle
	{0x42, OBJ_PROP_R_, 0, &GetActualAngle},		// axis 1 - actual angle
	{0x50, OBJ_PROP__W, 0, &SetNewTargetAngle},		// axis 2 - new target angle
	{0x51, OBJ_PROP_R_, 0, &GetActualTargetAngle},	// axis 2 - actual target angle
	{0x52, OBJ_PROP_R_, 0, &GetActualAngle},		// axis 2 - actual angle

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

// tool table
static toolTbl_t toolTbl[] = {

	// tool 0 - machine zero
	{0x00, OBJ_PROP_R_, 0, 0, 0, true, NULL},		// only change if you really know what you are doing!
	
	// variable tooltable entries
	//{0x01, OBJ_PROP_RW, 0, 0, 0, false, NULL},		// custom tools
	//{0x02, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{0x03, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{0x04, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{0x05, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{0x06, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{0x07, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{0x08, OBJ_PROP_RW, 0, 0, 0, false, NULL},
	{0x09, OBJ_PROP_RW, 0, 0, 0, false, NULL},
};

// static position register
static posReg_t posRegStatic[] = {

	// fixed positions 240 - 255 - only change if you really know what you are doing!
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
static posReg_t *pArray[PosArrayLength] = { NULL };
// index of the last position in the position register
static uint8_t pArrayLastPos = 0;

// ##############################################
// pos register - help functions
// ##############################################

static posReg_t* LocatePos(uint8_t *idx) {

	uint8_t i = 0;

	for (i = 0; i = PosArrayLength - 1; i++) {
		if (pArray[i]->pointIdx == *idx) {
			return pArray[i];
		}
	}
	return NULL;
}

int16_t* GetPosRegData(uint8_t *idx) {

	posReg_t *p = NULL;
	p = LocatePos(idx);

	if (p != NULL) {
		static int16_t data[3]; // static?
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

		if (*idx <= PosArrayLength - 1) {
			pArray[pArrayLastPos] = (posReg_t*)malloc(sizeof(posReg_t)); // allocate memory for new PosReg entry and store a pointer in the pArray
			pArray[pArrayLastPos]->pointIdx = *idx;
			pArray[pArrayLastPos]->props = OBJ_PROP_RW;
			pArrayLastPos++;
			p = LocatePos(idx);
		}
		else {
			SendStatus("in function SetPosRegData():", "failed to write - max number of points (64) reached ", STATUS_TYPE_ERROR);
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
				}
			}
			else {
				char msg[64];
				sprintf(msg, "position P%i value out of range", idx);
				SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else {
			char msg[64];
			sprintf(msg, "position P%i is read only", idx);
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
	p = objStruct_data;
	
	for (int i = 0; i < (sizeof(objStruct_data) / sizeof(objStruct_data[0])); i++, p++) {
		if (p->idx == index) {
			return p;
		}
	}
	return NULL;
}

uint16_t GetObjStructData(uint8_t index) {

	objStruct_t *p = NULL;
	p = LocateObj(index);

	if (p != NULL) {

		// wenn umrechung von pos von dez zu hex notwendig (pos * 10 + 32768) dann hier, damit das nicht jedes mal von Hand gemacht werden muss
		/*char msg[64];
		uint16_t tmp = p->data;
		sprintf(msg, "return value 0x%x", tmp);
		SendStatus("GetObjStructData(): ", "test", STATUS_TYPE_INFO);*/
		return p->data;
	}
	else {
		return NULL;
	}
}

int8_t SetObjStructData(uint8_t index, int16_t data) {

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!! Muss ggf. Handlerfunktion ausführen !!!!!!!!!!!!!!!!!!!!!!! falls Fehler in Handler -> return -1:


	objStruct_t *pObjStruct = NULL;
	pObjStruct = LocateObj(index);
	int16_t minValue = 0;
	int16_t maxValue = 0;

	if (pObjStruct != NULL) {	// make sure object does exist

		if (pObjStruct->props == OBJ_PROP_RW || pObjStruct->props == OBJ_PROP__W) {	// check if object is writable

			switch (pObjStruct->idx)	// set min max vaules for comparison
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
			if (data >= minValue &&  data <= maxValue) { //  write data if it's inside the allowed range
				pObjStruct->data = data;
				return 0;
			}
			else {
				char msg[64];
				sprintf(msg, "object 0x%x value out of range", index);
				SendStatus("in function SetObjStructData(): failed to write - ", msg, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else {
			char msg[64];
			sprintf(msg, "object 0x%x is read only", index);
			SendStatus("in function SetObjStructData(): failed to write - ", msg, STATUS_TYPE_ERROR);
			return -1;
		}
	}
	else {
		char msg[64];
		sprintf(msg, "object 0x%x does not exist", index);
		SendStatus("in function SetObjStructData(): failed to write - ", msg, STATUS_TYPE_ERROR);
		return -1;
	}
}

// ##############################################
// tool table - help functions
// ##############################################

toolTbl_t* LocateTool(uint8_t index) {

	toolTbl_t *p = NULL;
	p = toolTbl;
	
	for (int i = 0; i < (sizeof(toolTbl) / sizeof(toolTbl[0])); i++, p++) {
		if (p->toolIdx == index) {
			char msg[64];
			sprintf(msg, "tool %i found", p->toolIdx);
			SendStatus("LocateTool(): ", msg, STATUS_TYPE_DEBUG);
			return p;
		}
	}
	return NULL;
}

int16_t* GetToolTblData(uint8_t index) {

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
		SendStatus("GetToolTblData(): ", msg, STATUS_TYPE_DEBUG);
		return data;
	}
	else {
		return NULL;
	}
}