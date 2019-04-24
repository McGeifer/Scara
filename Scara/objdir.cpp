/* 
 * 
 */

#include "objdir.h"
#include "status.h"
#include "calc.h"
#include "dynamixel.h"

/* object dictionary */
static objDir_t obj_dir[] =
{
	/* basic options */
	/* {OBJ_IDX_ACK,							OBJ_PROP__W, 0}, */
	{OBJ_IDX_START_MOVE,					OBJ_PROP__W, 0, NULL},
	{OBJ_IDX_MOVING,						OBJ_PROP_R_, 0, NULL},
	{OBJ_IDX_POS_REACHED,					OBJ_PROP_R_, 0, NULL},
	
	/* internal objects */
	{OBJ_IDX_Z_POS_COUNT,					OBJ_PROP_RW, 0, NULL},
	{OBJ_IDX_OP_MODE,						OBJ_PROP_RW, 0, NULL},
	{OBJ_IDX_SYS_STATUS,					OBJ_PROP_RW, 0x04, NULL}, /* bit mask ! */    /* 0x04 for testing debug mode !!!!!!!!!!!!!!!!!!!!!!!!! */
	
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
	{OBJ_IDX_AXIS_2_ACTUAL_SPEED,			OBJ_PROP_R_, 0, NULL}
};

/* tool table */
static toolTbl_t tool_tbl[] =
{
	/* tool 0 - machine zero */
	{OBJ_IDX_TOOL_0, OBJ_PROP_R_, 0, 0, 0, true},
	
	/* variable tool table entries */
	{OBJ_IDX_TOOL_1, OBJ_PROP_RW, 25, -100, 75, true},
	{OBJ_IDX_TOOL_2, OBJ_PROP_RW, -50, 50, 25, true},
	{OBJ_IDX_TOOL_3, OBJ_PROP_RW, 0, 0, 0, false},
	{OBJ_IDX_TOOL_4, OBJ_PROP_RW, 0, 0, 0, false},
	{OBJ_IDX_TOOL_5, OBJ_PROP_RW, 0, 0, 0, false},
	{OBJ_IDX_TOOL_6, OBJ_PROP_RW, 0, 0, 0, false},
	{OBJ_IDX_TOOL_7, OBJ_PROP_RW, 0, 0, 0, false},
	{OBJ_IDX_TOOL_8, OBJ_PROP_RW, 0, 0, 0, false},
	{OBJ_IDX_TOOL_9, OBJ_PROP_RW, 0, 0, 0, false}
};

/* static position register */
static posReg_t stat_pos_reg[] =
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
	{OBJ_IDX_STAT_POS_GN_COIN_5,	OBJ_PROP_R_, 0, 0, 0}
};

/* dynamic position register */
static posReg_t *dyn_pos_reg[PosArrayLength] = { 0 };

/* idx of the last position in the position register */
static uint8_t p_last_pos = 0;


static posReg_t* LocatePos(uint8_t *idx)
{
	for (uint8_t i = 0; i < PosArrayLength; i++)
	{
		if (dyn_pos_reg[i]->point_idx == *idx)
		{
			SendStatus("LocatePos() ", "pos found", SYS_STAT_MSG_TYPE_DEBUG);
			return dyn_pos_reg[i];
		}
	}
	SendStatus("LocatePos() ", "pos not found", SYS_STAT_MSG_TYPE_DEBUG);
	return NULL;
}

int16_t* GetPosRegData(uint8_t *idx)
{
	posReg_t *p = NULL;
	p = LocatePos(idx);

	if (p != NULL)
	{
		static int16_t data[3];
		data[0] = p->x_pos;
		data[1] = p->y_pos;
		data[2] = p->z_pos;
		return data;
	}
	else
	{
		return NULL;
	}
}

uint8_t SetPosRegData(uint8_t *idx, int16_t *x_pos, int16_t *y_pos, int16_t *z_pos)
{
	posReg_t *p = NULL;
	p = LocatePos(idx);

	if (p == NULL)
	{
		if (*idx < PosArrayLength)
		{
			dyn_pos_reg[p_last_pos] = (posReg_t*)malloc(sizeof(posReg_t)); /* allocate memory for new PosReg entry and store a pointer in the dyn_pos_reg */
			dyn_pos_reg[p_last_pos]->point_idx = *idx;
			dyn_pos_reg[p_last_pos]->props = OBJ_PROP_RW;
			p_last_pos++;
			p = LocatePos(idx);
			if (p == NULL)
			{
				SendStatus("in function SetPosRegData(): ", "unknown error while writing new position value", SYS_STAT_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			SendStatus("in function SetPosRegData(): ", "failed to write - max number of points (64) reached", SYS_STAT_MSG_TYPE_ERROR);
			return -1;
		}
	}
	if (p != NULL)
	{
		if (p->props == OBJ_PROP_RW || p->props == OBJ_PROP__W) /* check if object is writable */
		{
			if (*x_pos >= X_POS_MIN && *x_pos <= X_POS_MAX) /* check the permissible value range */
			{
				if (*y_pos >= Y_POS_MIN && *y_pos <= Y_POS_MAX)
				{
					if (*z_pos >= Z_POS_MIN && *z_pos <= Z_POS_MAX)
					{
						p->x_pos = *x_pos;
						p->y_pos = *y_pos;
						p->z_pos = *z_pos;
						return 0;
					}
					else
					{
						char msg[64];
						sprintf(msg, "position P%c value out of range", *idx);
						SendStatus("in function SetPosRegData(): failed to write - ", msg, SYS_STAT_MSG_TYPE_ERROR);
						return -1;
					}
				}
				else
				{
					char msg[64];
					sprintf(msg, "position P%c value out of range", *idx);
					SendStatus("in function SetPosRegData(): failed to write - ", msg, SYS_STAT_MSG_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				char msg[64];
				sprintf(msg, "position P%c value out of range", *idx);
				SendStatus("in function SetPosRegData(): failed to write - ", msg, SYS_STAT_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			char msg[64];
			sprintf(msg, "position P%c is read only", *idx);
			SendStatus("in function SetPosRegData(): failed to write - ", msg, SYS_STAT_MSG_TYPE_ERROR);
			return -1;
		}
	}
	else
	{
		SendStatus("in function SetPosRegData(): failed to write - ", "unknown error", SYS_STAT_MSG_TYPE_ERROR);
		return -1;
	}
}

static objDir_t* LocateObj(uint8_t idx)
{
	objDir_t *p = NULL;
	p = obj_dir;
	
	for (uint8_t i = 0; i < (sizeof(obj_dir) / sizeof(obj_dir[0])); i++, p++)
	{
		if (p->idx == idx)
		{
			return p;
		}
	}
	return NULL;
}

int16_t GetObjData(uint8_t idx)
{
	objDir_t *p = NULL;
	p = LocateObj(idx);

	if (p != NULL)
	{
		/*char msg[64];
		uint16_t tmp = p->data;
		sprintf(msg, "return value 0x%x", tmp);
		SendStatus("GetObjData(): ", "test", SYS_STAT_MSG_TYPE_INFO);*/
		return p->data;
	}
	else
	{
		return -1;
	}
}

int8_t SetObjData(uint8_t idx, int16_t data, bool internal_call) {

	objDir_t *p = NULL;
	p = LocateObj(idx);
	int16_t min_val = 0;
	int16_t max_val = 0;

	if (p != NULL) /* make sure object does exist */
	{
		if (p->props == OBJ_PROP_RW || p->props == OBJ_PROP__W || internal_call == true) /* check if object is writable */
		{
			switch (p->idx)	/* set min/ max values for comparison */
			{
			case OBJ_IDX_X_NEW_TARGET_POS:
				min_val = X_POS_MIN;
				max_val = X_POS_MAX;
				break;

			case OBJ_IDX_Y_NEW_TARGET_POS:
				min_val = Y_POS_MIN;
				max_val = Y_POS_MAX;
				break;

			case OBJ_IDX_Z_NEW_TARGET_POS:
				min_val = Z_POS_MIN;
				max_val = Z_POS_MAX;
				break;

			case OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE:
				min_val = AXIS_1_ANGLE_MIN;
				max_val = AXIS_1_ANGLE_MAX;
				break;

			case OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE:
				min_val = AXIS_2_ANGLE_MIN;
				max_val = AXIS_2_ANGLE_MAX;
				break;

			case OBJ_IDX_X_NEW_TARGET_SPEED:
				min_val = X_SPEED_MIN;
				max_val = X_SPEED_MAX;
				break;

			case OBJ_IDX_Y_NEW_TARGET_SPEED:
				min_val = Y_SPEED_MIN;
				max_val = Y_SPEED_MAX;
				break;

			case OBJ_IDX_Z_NEW_TARGET_SPEED:
				min_val = Z_SPEED_MIN;
				max_val = Z_SPEED_MAX;
				break;

			case OBJ_IDX_AXIS_1_NEW_TARGET_SPEED:
				min_val = AXIS_1_SPEED_MIN;
				max_val = AXIS_1_SPEED_MAX;
				break;

			case OBJ_IDX_AXIS_2_NEW_TARGET_SPEED:
				min_val = AXIS_2_SPEED_MIN;
				max_val = AXIS_2_SPEED_MAX;
				break;

			default:
				min_val = -32768; /* gute Idee? */
				max_val = 32767;
				break;
			}

			if (data >= min_val &&  data <= max_val) /* write data if it's inside the allowed range */
			{
				p->data = data;
				return 0;
			}
			else
			{
				char msg[64];
				sprintf(msg, "object 0x%x value out of range", idx);
				SendStatus("in function SetObjData(): failed to write - ", msg, SYS_STAT_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			char msg[64];
			sprintf(msg, "object 0x%x is read only", idx);
			SendStatus("in function SetObjData(): failed to write - ", msg, SYS_STAT_MSG_TYPE_ERROR);
			return -1;
		}
	}
	else
	{
		char msg[64];
		sprintf(msg, "object 0x%x does not exist", idx);
		SendStatus("in function SetObjData(): failed to write - ", msg, SYS_STAT_MSG_TYPE_ERROR);
		return -1;
	}
}

int8_t SetActualAngles(float *servo_1, float *servo_2)
{
	int16_t act_angle_1 = GetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE);

	if (SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, round((degrees(*servo_1)) * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE, round((degrees(*servo_2)) * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, act_angle_1, true); /* try to set the old value of axis 1 to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetActualTargetAngles(float *servo_1, float *servo_2)
{
	int16_t old_target_angle_1 = GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE);

	if (SetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE, round((degrees(*servo_1)) * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE, round((degrees(*servo_2)) * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE, old_target_angle_1, true); /* try to set the old value of axis 1 to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetNewTargetAngles(float *servo_1, float *servo_2)
{
	int16_t old_target_angle_1 = GetObjData(OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE);

	if (SetObjData(OBJ_IDX_AXIS_1_NEW_TARGET_ANGLE, round((degrees(*servo_1)) * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_AXIS_2_NEW_TARGET_ANGLE, round((degrees(*servo_2)) * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, old_target_angle_1, true); /* try to set the old value of axis 1 to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetActualPositions(float *x_pos, float *y_pos)
{
	int16_t actual_x_pos = GetObjData(OBJ_IDX_X_ACTUAL_POS);

	if (SetObjData(OBJ_IDX_X_ACTUAL_POS, round(*x_pos * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_Y_ACTUAL_POS, round(*y_pos * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_X_ACTUAL_POS, actual_x_pos, true); /* try to set the old value of x to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetActualTargetPositions(float *x_pos, float *y_pos)
{
	int16_t actual_target_pos_X = GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS);

	if (SetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS, round(*x_pos * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS, round(*y_pos * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS, actual_target_pos_X, true); /* try to set the old value of x to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int8_t SetNewTargetPositions(float *x_pos, float *y_pos)
{
	int16_t new_target_pos_x = GetObjData(OBJ_IDX_X_NEW_TARGET_POS);

	if (SetObjData(OBJ_IDX_X_NEW_TARGET_POS, round(*x_pos * 10.0), true) == 0)
	{
		if (SetObjData(OBJ_IDX_Y_NEW_TARGET_POS, round(*y_pos * 10.0), true) == 0)
		{
			return 0;
		}
		else
		{
			SetObjData(OBJ_IDX_X_NEW_TARGET_POS, new_target_pos_x, true); /* try to set the old value of x to avoid inconsistent data */
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

toolTbl_t* LocateTool(uint8_t idx)
{
	toolTbl_t *p = NULL;
	p = tool_tbl;
	
	for (uint8_t i = 0; i < (sizeof(tool_tbl) / sizeof(tool_tbl[0])); i++, p++)
	{
		if (p->tool_idx == idx)
		{
			char msg[64];
			sprintf(msg, "tool %u found", p->tool_idx);
			SendStatus("LocateTool(): ", msg, SYS_STAT_MSG_TYPE_DEBUG);
			return p;
		}
	}
	return NULL;
}

int16_t* GetToolData(uint8_t idx)
{
	toolTbl_t *p = NULL;
	p = LocateTool(idx);

	if (p != NULL)
	{
		static int16_t data[4];
		data[0] = p->x_offset;
		data[1] = p->y_offset;
		data[2] = p->z_offset;
		data[3] = p->active;
		char msg[64];
		sprintf(msg, "tool offset values: x: %i, y: %i, z: %i", p->x_offset, p->y_offset, p->z_offset);
		SendStatus("GetToolData(): ", msg, SYS_STAT_MSG_TYPE_DEBUG);
		return data;
	}
	else
	{
		return NULL;
	}
}

void UpdateOD(void)
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
		for (uint8_t id = 0; id < 3; id++)
		{
			//data[id][angle] = dynamixelReadPosition(id); /********** was mit z-Achse? was f�r R�ckgabewerte bei continuous turn modus? ************/
			//data[id][speed] = dynamixelReadSpeed(id);

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
				dxlPrintErrorMessage(error, id);
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
					SetObjData(OBJ_IDX_Z_ACTUAL_POS, UpdateZPos(), true); //  falsch !!!!!!!!!!!!!
					SetObjData(OBJ_IDX_Z_ACTUAL_SPEED, data[id][speed], true);
					break;
				default:
					SendStatus("UpdateOD(): ", "unknown device ID", SYS_STAT_ERROR);
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