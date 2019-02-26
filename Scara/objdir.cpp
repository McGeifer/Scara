/* 
 * 
 */

#include "objdir.h"
#include "status.h"
#include "calc.h"
#include "dynamixel.h"

/* object dictionary */
static objStruct_t objDir[] =
{
	/* basic options */
	/* {OBJ_IDX_ACK,							OBJ_PROP__W, 0, NULL}, */
	{OBJ_IDX_START_MOVE,					OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_MOVING,						OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_POS_REACHED,					OBJ_PROP_R_, 0, NULL},

	/* internal objects */
	{OBJ_IDX_Z_POS_COUNT,					OBJ_PROP_RW, 0, NULL},
	{OBJ_IDX_OP_MODE,						OBJ_PROP_RW, 0, NULL},
	{OBJ_IDX_SYS_STATUS,					OBJ_PROP_RW, 4, NULL}, /* bit mask ! */    /* 0x04 for testing debug mode !!!!!!!!!!!!!!!!!!!!!!!!! */
	
	/* position values */
	{OBJ_IDX_X_NEW_TARGET_POS,				OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_X_ACTUAL_TARGET_POS,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_X_ACTUAL_POS,					OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Y_NEW_TARGET_POS,				OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_Y_ACTUAL_TARGET_POS,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Y_ACTUAL_POS,					OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Z_NEW_TARGET_POS,				OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_Z_ACTUAL_TARGET_POS,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_Z_ACTUAL_POS,					OBJ_PROP_R_, 0, NULL},
	
	/* angle values */
	{OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE,		OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE,	OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_1_ACTUAL_ANGLE,			OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE,		OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE,	OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_AXIS_2_ACTUAL_ANGLE,			OBJ_PROP_R_, 0, NULL},

	/* speed values */
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
	
	/* variable tool table entries */
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


static posReg_t* LocatePos(uint8_t *idx)
{
	for (uint8_t i = 0; i < PosArrayLength; i++)
	{
		if (dynPosReg[i]->pointIdx == *idx)
		{
			SendStatus("LocatePos() ", "pos found", STATUS_MSG_TYPE_DEBUG);
			return dynPosReg[i];
		}
	}
	SendStatus("LocatePos() ", "pos not found", STATUS_MSG_TYPE_DEBUG);
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
				SendStatus("in function SetPosRegData(): ", "unknown error while writing new position value", STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			SendStatus("in function SetPosRegData(): ", "failed to write - max number of points (64) reached", STATUS_MSG_TYPE_ERROR);
			return -1;
		}
	}
	if (p != NULL)
	{
		if (p->props == OBJ_PROP_RW || p->props == OBJ_PROP__W) /* check if object is writable */
		{
			if (*xVal >= OBJ_POS_X_MIN && *xVal <= OBJ_POS_X_MAX) /* check the permissible value range */
			{
				if (*yVal >= OBJ_POS_Y_MIN && *yVal <= OBJ_POS_Y_MAX)
				{
					if (*zVal >= OBJ_POS_Z_MIN && *zVal <= OBJ_POS_Z_MAX)
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
						SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_MSG_TYPE_ERROR);
						return -1;
					}
				}
				else
				{
					char msg[64];
					sprintf(msg, "position P%c value out of range", *idx);
					SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_MSG_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				char msg[64];
				sprintf(msg, "position P%c value out of range", *idx);
				SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			char msg[64];
			sprintf(msg, "position P%c is read only", *idx);
			SendStatus("in function SetPosRegData(): failed to write - ", msg, STATUS_MSG_TYPE_ERROR);
			return -1;
		}
	}
	else
	{
		SendStatus("in function SetPosRegData(): failed to write - ", "unknown error", STATUS_MSG_TYPE_ERROR);
		return -1;
	}
}

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
		SendStatus("GetObjData(): ", "test", STATUS_MSG_TYPE_INFO);*/
		return p->data;
	}
	else
	{
		return -1;
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
			switch (pObjStruct->idx)	/* set min/ max values for comparison */
			{
			case OBJ_IDX_X_NEW_TARGET_POS:
				minValue = OBJ_POS_X_MIN;
				maxValue = OBJ_POS_X_MAX;
				break;

			case OBJ_IDX_Y_NEW_TARGET_POS:
				minValue = OBJ_POS_Y_MIN;
				maxValue = OBJ_POS_Y_MAX;
				break;

			case OBJ_IDX_Z_NEW_TARGET_POS:
				minValue = OBJ_POS_Z_MIN;
				maxValue = OBJ_POS_Z_MAX;
				break;

			case OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE:
				minValue = OBJ_ANGLE_AXIS_1_MIN;
				maxValue = OBJ_ANGLE_AXIS_1_MAX;
				break;

			case OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE:
				minValue = OBJ_ANGLE_AXIS_2_MIN;
				maxValue = OBJ_ANGLE_AXIS_2_MAX;
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
				minValue = -32768; /* gute Idee? */
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
				SendStatus("in function SetObjData(): failed to write - ", msg, STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			char msg[64];
			sprintf(msg, "object 0x%x is read only", index);
			SendStatus("in function SetObjData(): failed to write - ", msg, STATUS_MSG_TYPE_ERROR);
			return -1;
		}
	}
	else
	{
		char msg[64];
		sprintf(msg, "object 0x%x does not exist", index);
		SendStatus("in function SetObjData(): failed to write - ", msg, STATUS_MSG_TYPE_ERROR);
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
			SendStatus("LocateTool(): ", msg, STATUS_MSG_TYPE_DEBUG);
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
		SendStatus("GetToolData(): ", msg, STATUS_MSG_TYPE_DEBUG);
		return data;
	}
	else
	{
		return NULL;
	}
}

void UpdateObjDir(void)
{
	int16_t data[3][2] = { 0 };
	uint8_t error = 0;
	float *result = {};

	enum dataType
	{
		angle,
		speed
	};

	if (!(GetObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_ERROR))
	{
		for (uint8_t id = 0; id < (3); id++)
		{
			data[id][angle] = dynamixelReadPosition(id); /********** was mit z-Achse? was für Rückgabewerte bei continuous turn modus? ************/
			data[id][speed] = dynamixelReadSpeed(id);

			/*Serial.print("angle");
			Serial.println(data[id][angle]);
			Serial.print("speed");
			Serial.println(data[id][speed]);*/

			if (data[id][angle] < 0 || data[id][speed] < 0)
			{

				if (data[id][angle] < 0) /* error value is negative by dynamixel library */
				{
					error = data[id][angle] * (-1);
				}
				else
				{
					error = data[id][speed] * (-1);
				}
				dxlError(error, id);
				return;
			}
			else
			{
				switch (id) /* store actual speed and position values, abort with error state if unknown id is detected */
				{
				case DXL_ID_AXIS_1:

					/* Durch die neuen Set Funktionen ersetzen !!! */
					SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, DYNA_TO_DEG(data[id][angle]), true);
					SetObjData(OBJ_IDX_AXIS_1_ACTUAL_SPEED, data[id][speed], true);
					break;
				case DXL_ID_AXIS_2:
					SetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE, DYNA_TO_DEG(data[id][angle]), true);
					SetObjData(OBJ_IDX_AXIS_2_ACTUAL_SPEED, data[id][speed], true);
					break;
				case DXL_ID_AXIS_Z:
					SetObjData(OBJ_IDX_Z_ACTUAL_POS, UpdateZPos(), true);
					SetObjData(OBJ_IDX_Z_ACTUAL_SPEED, data[id][speed], true);
					break;
				default:
					SendStatus("UpdateObjDir(): ", "unknown device ID", SYS_STAT_ERROR);
					return;
				}
			}
		}
		result = CalcPosistion(&data[DXL_ID_AXIS_1][angle], &data[DXL_ID_AXIS_2][angle]);

		if (result == NULL) /* calculate related x and y positions */
		{
#ifndef _DEBUG
			SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_ERROR, false);
#endif
		}
		else
		{
			SetActualPositions(&result[0], &result[1]);
		}


		if (data[DXL_ID_AXIS_1][speed] > 0 || data[DXL_ID_AXIS_2][speed] > 0 || data[DXL_ID_AXIS_Z][speed] > 0) /********** Was ist mit z-Achse?? Wie sehen die Geschwindigkeitswerte aus? > 0 richtig ? ************/
		{
			SetObjData(OBJ_IDX_MOVING, 1, true); /* system is moving */
		}
		else
		{
			SetObjData(OBJ_IDX_MOVING, 0, true); /* system is not moving */
		}
	}
}