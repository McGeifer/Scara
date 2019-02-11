// 
// 
// 

#include "objdir.h"
#include "status.h"
#include "dynamixel.h"

/* object dictionary */
static objStruct_t objDir[] =
{
	// basic options
	//{OBJ_IDX_ACK,							OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_START_MOVE,					OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_MOVING,						OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_POS_REACHED,					OBJ_PROP_R_, 0, NULL},

	// internal objects
	{OBJ_IDX_Z_POS_COUNT,					OBJ_PROP_RW, 0, NULL},
	{OBJ_IDX_OP_MODE,						OBJ_PROP_RW, 0, NULL},
	{OBJ_IDX_SYS_STATUS,					OBJ_PROP_RW, 4, NULL}, /* bitmask ! */    /* 0x04 for testing debug mode !!!!!!!!!!!!!!!!!!!!!!!!! */
	
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

/* tool table */
static toolTbl_t toolTbl[] =
{
	/* tool 0 - machine zero */
	{OBJ_IDX_TOOL_0, OBJ_PROP_R_, 0, 0, 0, true, NULL},
	
	/* variable tooltable entries */
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

/* static position register */
static posReg_t statPosReg[] =
{
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

/* dynamic position register */
static posReg_t *dynPosReg[PosArrayLength] = { 0 };

/* index of the last position in the position register */
static uint8_t pLastPos = 0;

// ##############################################
// pos register - help functions
// ##############################################

static posReg_t* LocatePos(uint8_t *idx)
{
	for (uint8_t i = 0; i < PosArrayLength; i++)
	{
		if (dynPosReg[i]->pointIdx == *idx)
		{
			SendStatus("LocatePos() ", "pos found", STATUS_TYPE_DEBUG);
			return dynPosReg[i];
		}
	}
	SendStatus("LocatePos() ", "pos not found", STATUS_TYPE_DEBUG);
	return NULL;
}

int16_t* GetPosRegData(uint8_t *idx)
{
	posReg_t *p = NULL;
	p = LocatePos(idx);

	if (p != NULL)
	{
		static int16_t data[3];
		data[0] = p->posRegX;
		data[1] = p->posRegY;
		data[2] = p->posRegZ;
		return data;
	}
	else
	{
		return NULL;
	}
}

uint8_t SetPosRegData(uint8_t *idx, int16_t *xVal, int16_t *yVal, int16_t *zVal)
{
	posReg_t *p = NULL;
	p = LocatePos(idx);

	if (p == NULL)
	{
		if (*idx < PosArrayLength)
		{
			dynPosReg[pLastPos] = (posReg_t*)malloc(sizeof(posReg_t)); /* allocate memory for new PosReg entry and store a pointer in the dynPosReg */
			dynPosReg[pLastPos]->pointIdx = *idx;
			dynPosReg[pLastPos]->props = OBJ_PROP_RW;
			pLastPos++;
			p = LocatePos(idx);
			if (p == NULL)
			{
				SendStatus("in function SetPosRegData(): ", "unknown error while writing new position value", STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			SendStatus("in function SetPosRegData(): ", "failed to write - max number of points (64) reached", STATUS_TYPE_ERROR);
			return -1;
		}
	}
	if (p != NULL)
	{
		if (p->props == OBJ_PROP_RW || p->props == OBJ_PROP__W) /* check if object is writable */
		{
			if (*xVal >= X_POS_MIN && *xVal <= X_POS_MAX) /* check the permissable value range */
			{
				if (*yVal >= Y_POS_MIN && *yVal <= Y_POS_MAX)
				{
					if (*zVal >= Z_POS_MIN && *zVal <= Z_POS_MAX)
					{
						p->posRegX = *xVal;
						p->posRegY = *yVal;
						p->posRegZ = *zVal;
						return 0;
					}
					else
					{
						char msg[64];
						sprintf(msg, "position P%c value out of range", *idx);
						SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_TYPE_ERROR);
						return -1;
					}
				}
				else
				{
					char msg[64];
					sprintf(msg, "position P%c value out of range", *idx);
					SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				char msg[64];
				sprintf(msg, "position P%c value out of range", *idx);
				SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			char msg[64];
			sprintf(msg, "position P%c is read only", *idx);
			SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_TYPE_ERROR);
			return -1;
		}
	}
	else
	{
		SendStatus("in function SetPosRegData(): failed to write - ", "unknown error", STATUS_TYPE_ERROR);
		return -1;
	}
}

// ##############################################
// objDir - help functions
// ##############################################

static objStruct_t* LocateObj(uint8_t index)
{
	objStruct_t *p = NULL;
	p = objDir;
	
	for (uint8_t i = 0; i < (sizeof(objDir) / sizeof(objDir[0])); i++, p++)
	{
		if (p->idx == index)
		{
			return p;
		}
	}
	return NULL;
}

int16_t GetObjData(uint8_t index)
{
	objStruct_t *p = NULL;
	p = LocateObj(index);

	if (p != NULL)
	{
		/*char msg[64];
		uint16_t tmp = p->data;
		sprintf(msg, "return value 0x%x", tmp);
		SendStatus("GetObjData(): ", "test", STATUS_TYPE_INFO);*/
		return p->data;
	}
	else
	{
		return 0;
	}
}

int8_t SetObjData(uint8_t index, int16_t data, bool internalCall) {

	objStruct_t *pObjStruct = NULL;
	pObjStruct = LocateObj(index);
	int16_t minValue = 0;
	int16_t maxValue = 0;

	if (pObjStruct != NULL) /* make sure object does exist */
	{
		if (pObjStruct->props == OBJ_PROP_RW || pObjStruct->props == OBJ_PROP__W || internalCall == true) /* check if object is writable */
		{
			switch (pObjStruct->idx)	/* set min/ max vaules for comparison */
			{
			case OBJ_IDX_X_NEW_TARGET_POS:
				minValue = X_POS_MIN;
				maxValue = X_POS_MAX;
				break;

			case OBJ_IDX_Y_NEW_TARGET_POS:
				minValue = Y_POS_MIN;
				maxValue = Y_POS_MAX;
				break;

			case OBJ_IDX_Z_NEW_TARGET_POS:
				minValue = Z_POS_MIN;
				maxValue = Z_POS_MAX;
				break;

			case OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE:
				minValue = AXIS_1_ANGLE_MIN;
				maxValue = AXIS_1_ANGLE_MAX;
				break;

			case OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE:
				minValue = AXIS_2_ANGLE_MIN;
				maxValue = AXIS_2_ANGLE_MAX;
				break;

			case OBJ_IDX_X_NEW_TARGET_SPEED:
				minValue = X_SPEED_MIN;
				maxValue = X_SPEED_MAX;
				break;

			case OBJ_IDX_Y_NEW_TARGET_SPEED:
				minValue = Y_SPEED_MIN;
				maxValue = Y_SPEED_MAX;
				break;

			case OBJ_IDX_Z_NEW_TARGET_SPEED:
				minValue = Z_SPEED_MIN;
				maxValue = Z_SPEED_MAX;
				break;

			case OBJ_IDX_AXIS_1_NEW_TARGET_SPEED:
				minValue = AXIS_1_SPEED_MIN;
				maxValue = AXIS_1_SPEED_MAX;
				break;

			case OBJ_IDX_AXIS_2_NEW_TARGET_SPEED:
				minValue = AXIS_2_SPEED_MIN;
				maxValue = AXIS_2_SPEED_MAX;
				break;

			default:
				minValue = -32768; // gute Idee?
				maxValue = 32767;
				break;
			}

			if (data >= minValue &&  data <= maxValue) /* write data if it's inside the allowed range */
			{
				pObjStruct->data = data;
				return 0;
			}
			else
			{
				char msg[64];
				sprintf(msg, "object 0x%x value out of range", index);
				SendStatus("in function SetObjData(): failed to write - ", msg, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			char msg[64];
			sprintf(msg, "object 0x%x is read only", index);
			SendStatus("in function SetObjData(): failed to write - ", msg, STATUS_TYPE_ERROR);
			return -1;
		}
	}
	else
	{
		char msg[64];
		sprintf(msg, "object 0x%x does not exist", index);
		SendStatus("in function SetObjData(): failed to write - ", msg, STATUS_TYPE_ERROR);
		return -1;
	}
}

int8_t SetActualAngles(float *servo1, float *servo2)
{
	int16_t actAngle1 = GetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE);

	if (SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, round((degrees(*servo1)) * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE, round((degrees(*servo2)) * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, actAngle1, true); /* try to set the old value of axis 1 in order to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetActualTargetAngles(float *servo1, float *servo2)
{
	int16_t oldTargetAngle1 = GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE);

	if (SetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE, round((degrees(*servo1)) * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE, round((degrees(*servo2)) * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE, oldTargetAngle1, true); /* try to set the old value of axis 1 in order to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetNewTargetAngles(float *servo1, float *servo2)
{
	int16_t oldTargetAngle1 = GetObjData(OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE);

	if (SetObjData(OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE, round((degrees(*servo1)) * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE, round((degrees(*servo2)) * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, oldTargetAngle1, true); /* try to set the old value of axis 1 in order to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetActualPositions(float *xPos, float *yPos)
{
	int16_t actPosX = GetObjData(OBJ_IDX_X_ACTUAL_POS);

	if (SetObjData(OBJ_IDX_X_ACTUAL_POS, round(*xPos * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_Y_ACTUAL_POS, round(*yPos * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_X_ACTUAL_POS, actPosX, true); /* try to set the old value of x in order to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetActualTargetPositions(float *xPos, float *yPos)
{
	int16_t actTargetPosX = GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS);

	if (SetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS, round(*xPos * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS, round(*yPos * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS, actTargetPosX, true); /* try to set the old value of x in order to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetNewTargetPositions(float *xPos, float *yPos)
{
	int16_t newTargetPosX = GetObjData(OBJ_IDX_X_NEW_TARGET_POS);

	if (SetObjData(OBJ_IDX_X_NEW_TARGET_POS, round(*xPos * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_Y_NEW_TARGET_POS, round(*yPos * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_X_NEW_TARGET_POS, newTargetPosX, true); /* try to set the old value of x in order to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

// ##############################################
// tool table - help functions
// ##############################################

toolTbl_t* LocateTool(uint8_t index)
{
	toolTbl_t *p = NULL;
	p = toolTbl;
	
	for (uint8_t i = 0; i < (sizeof(toolTbl) / sizeof(toolTbl[0])); i++, p++)
	{
		if (p->toolIdx == index)
		{
			char msg[64];
			sprintf(msg, "tool %i found", p->toolIdx);
			SendStatus("LocateTool(): ", msg, STATUS_TYPE_DEBUG);
			return p;
		}
	}
	return NULL;
}

int16_t* GetToolData(uint8_t index)
{
	toolTbl_t *p = NULL;
	p = LocateTool(index);

	if (p != NULL)
	{
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
	else
	{
		return NULL;
	}
}

