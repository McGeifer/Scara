// 
// 
// 

#include "dynamixel.h"
#include "objdir.h"
#include "status.h"
#include "calc.h"
#include "DynamixelSerial2/DynamixelSerial2.h"

void InitDynamixel(void) 
{
	int tmp = 0;
	int response = 0;

	/* search for servo motors */
	for (int i = 0; i <= 2; i++)
	{
		tmp = dynamixelPing(i);

		if (tmp == -1)
		{
			char msg[32];
			sprintf(msg, "no Dynamixel found @ ID: %d", i);
			SendStatus("InitDynamixel(): ", msg, STATUS_TYPE_ERROR);
		}
		else
		{
			char msg[32];
			sprintf(msg, "Dynamixel found @ ID: %d", i);
			SendStatus("InitDynamixel(): ", msg, STATUS_TYPE_INFO);
			response++;
		}
	}

	if (response != 3)
	{
		SendStatus("InitDynamixel(): ", "check wiring of dynamixel servos and restart the controller", STATUS_TYPE_ERROR);
		/* if one of the servos could not be found then set the system error state to prevent further operations */
#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR, false);
#endif
		return;
	}
	else
	{
		/* initialization of the parameter list for setting up the dynamixel servos when starting the controller */
		uint16_t dynaParamList[3][13] =
		{
		{	DYNA_AXIS_1_TEMP_LIMIT,
			DYNA_AXIS_1_LOW_VOLTAGE_LIMIT,
			DYNA_AXIS_1_HIGH_VOLTAGE_LIMIT,
			DYNA_AXIS_1_MAX_TORQUE,
			DYNA_AXIS_1_SRL,
			DYNA_AXIS_1_RDT,
			DYNA_AXIS_1_LED_ALARM,
			DYNA_AXIS_1_SHUT_DOWN_ALARM,
			DYNA_AXIS_1_CW_C_SLOPE,
			DYNA_AXIS_1_CCW_C_SLOPE,
			DYNA_AXIS_1_CW_C_MARGIN,
			DYNA_AXIS_1_CCW_C_MARGIN,
			DYNA_AXIS_1_PUNCH,
		},
		{	DYNA_Z_AXIS_TEMP_LIMIT,
			DYNA_AXIS_2_LOW_VOLTAGE_LIMIT,
			DYNA_AXIS_2_HIGH_VOLTAGE_LIMIT,
			DYNA_Z_AXIS_MAX_TORQUE,
			DYNA_Z_AXIS_SRL,
			DYNA_Z_AXIS_RDT,
			DYNA_Z_AXIS_LED_ALARM,
			DYNA_Z_AXIS_SHUT_DOWN_ALARM,
			DYNA_Z_AXIS_CW_C_SLOPE,
			DYNA_Z_AXIS_CCW_C_SLOPE,
			DYNA_Z_AXIS_CW_C_MARGIN,
			DYNA_Z_AXIS_CCW_C_MARGIN,
			DYNA_Z_AXIS_PUNCH,
		},
		{	DYNA_Z_AXIS_TEMP_LIMIT,
			DYNA_Z_AXIS_LOW_VOLTAGE_LIMIT,
			DYNA_Z_AXIS_HIGH_VOLTAGE_LIMIT,
			DYNA_Z_AXIS_MAX_TORQUE,
			DYNA_Z_AXIS_SRL,
			DYNA_Z_AXIS_RDT,
			DYNA_Z_AXIS_LED_ALARM,
			DYNA_Z_AXIS_SHUT_DOWN_ALARM,
			DYNA_Z_AXIS_CW_C_SLOPE,
			DYNA_Z_AXIS_CCW_C_SLOPE,
			DYNA_Z_AXIS_CW_C_MARGIN,
			DYNA_Z_AXIS_CCW_C_MARGIN,
			DYNA_Z_AXIS_PUNCH,
		}
		};

		/* List of setup functions for the dynamixel servos. The functions will be executed for all 3 Dynamisel servos by using the paramter list above */
		funcPtr dynaFuncPtr[] =
		{
			dynamixelSetTempLimit,
			dynamixelSetLowVoltageLimit,
			dynamixelSetHighVoltageLimit,
			dynamixelSetMaxTorque,
			dynamixelSetSRL,
			dynamixelSetRDT,
			dynamixelSetLEDAlarm,
			dynamixelSetShutdownAlarm,
			dynamixelSetCWCSlope,
			dynamixelSetCCWCSlope,
			dynamixelSetCWCMargin,
			dynamixelSetCCWCMargin,
			dynamixelSetPunch
		};

		/* execute setup functions */
		for (uint8_t i = 0; i < ID_TOTAL_NUMBER; i++) /* outer loop for iterating the servo ids */
		{ 
			for (size_t j = 0; j < sizeof(dynaFuncPtr) / sizeof(dynaFuncPtr[0]); j++) /* inner loop for iterating the setup functions */
			{
				const char* funcName;
				if (int16_t error = dynaFuncPtr[j](i, dynaParamList[i][j], &funcName) < 0) /* executing the setup functions */
				{
					DynamixelError(error * (-1), i);
					SendStatus("While calling: ", funcName, STATUS_TYPE_ERROR);
					return;
				}
			}
		}
		/* enable continous turn for z axis */
		if (int16_t error = dynamixelSetEndless(ID_Z_AXIS, ON) < 0)
		{
			DynamixelError(error * (-1), ID_Z_AXIS);
			return;
		}
		/* enable torque for all servo motors */
		if (int16_t error = dynamixelTorqueStatus(BROADCAST_ID, ON) < 0)
		{
			DynamixelError(error * (-1), BROADCAST_ID);
			return;
		}
	}
}

void DynamixelError(uint8_t errorBit, uint8_t id)
{
	if (errorBit > 0 && errorBit <= 0x80)
	{
		char msg[64];

		switch (errorBit)
		{
		case 0x01:
			sprintf(msg, "Dynamixel - Input Voltage Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_TYPE_ERROR);
			break;

		case 0x02:
			sprintf(msg, "Dynamixel - Angle Limit Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_TYPE_ERROR);
			break;

		case 0x04:
			sprintf(msg, "Dynamixel - Overheating Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_TYPE_ERROR);
			break;

		case 0x08:
			sprintf(msg, "Dynamixel - Range Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_TYPE_ERROR);
			break;

		case 0x10:
			sprintf(msg, "Dynamixel - Checksum Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_TYPE_ERROR);
			break;

		case 0x20:
			sprintf(msg, "Dynamixel - Overload Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_TYPE_ERROR);
			break;

		case 0x40:
			sprintf(msg, "Dynamixel - Instruction Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_TYPE_ERROR);
			break;

		case 0x80:
		default:
			sprintf(msg, "Dynamixel - unknown Error bit @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_TYPE_ERROR);
			break;
		}
#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR, false);
#endif
	}
}

void UpdateObjDir(void)
{
	int16_t data[ID_TOTAL_NUMBER][2] = { 0 };
	uint8_t error = 0;
	float *result = {};

	if (!(GetObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_ERROR))
	{
		for (uint8_t id = 0; id < (ID_TOTAL_NUMBER); id++)
		{
			data[id][angle] = dynamixelReadPosition(id); /********** was mit z achse? was für Rückgabewerte bei continous turn modus? ************/
			data[id][speed] = dynamixelReadSpeed(id);

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
				DynamixelError(error, id);
				return;
			}
			else
			{
				switch (id) /* store actual speed and position values, abort with error state if unknown id is detected */
				{
				case DYNA_ID_AXIS_1:
					SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, DYNA_TO_DEG(data[id][angle]), true);
					SetObjData(OBJ_IDX_AXIS_1_ACTUAL_SPEED, data[id][speed], true);
					break;
				case DYNA_ID_AXIS_2:
					SetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE, DYNA_TO_DEG(data[id][angle]), true);
					SetObjData(OBJ_IDX_AXIS_2_ACTUAL_SPEED, data[id][speed], true);
					break;
				case DYNA_ID_AXIS_Z:
					SetObjData(OBJ_IDX_Z_ACTUAL_POS, UpdateZPos(), true);
					SetObjData(OBJ_IDX_Z_ACTUAL_SPEED, data[id][speed], true);
					break;
				default:
					SendStatus("UpdateObjDir(): ", "unknown device ID", SYS_STAT_ERROR);
					return;
				}
			}
		}
		result = CalcPosistion(&data[ID_AXIS_1][angle], &data[ID_AXIS_2][angle]);

		if (result == NULL) /* calcualte related x and y positions */
		{
#ifndef _DEBUG
			SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_ERROR, false);
#endif
		}
		else
		{
			SetActualPositions(&result[0], &result[1]);
		}
		
		
		if (data[ID_AXIS_1][speed] > 0 || data[ID_AXIS_2][speed] > 0 || data[ID_Z_AXIS][speed] > 0) /********** Was ist mit z-achse?? Wie sehen die Geschwindigkeitswerte aus? > 0 richtig ? ************/
		{ 
			SetObjData(OBJ_IDX_MOVING, 1, false); /* system is moving */
		}
		else
		{
			SetObjData(OBJ_IDX_MOVING, 0, false); /* system is not moving */
		}
	}
}

void HandleMove(void) {

	if (!(GetObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_ERROR))
	{
		uint8_t moving = (uint8_t)GetObjData(OBJ_IDX_MOVING);
		uint8_t start = (uint8_t)GetObjData(OBJ_IDX_START_MOVE);
		
		float actPos[3] = /* actual position values */
		{
			GetObjData(OBJ_IDX_X_ACTUAL_POS) / 10.0,
			GetObjData(OBJ_IDX_Y_ACTUAL_POS) / 10.0,
			GetObjData(OBJ_IDX_Z_ACTUAL_POS) / 10.0
		};
		float actTargetPos[3] = /* actual target position values */
		{
			GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS) / 10.0
		};
		float newTargetPos[3] = /* new target position values */
		{
			GetObjData(OBJ_IDX_X_NEW_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Y_NEW_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Z_NEW_TARGET_POS) / 10.0,
		};
		float posWindow[3][2] = /* allowed position windows */
		{
			{ actTargetPos[ID_AXIS_1] - POS_TOLERANCE, actTargetPos[ID_AXIS_1] + POS_TOLERANCE },
			{ actTargetPos[ID_AXIS_2] - POS_TOLERANCE, actTargetPos[ID_AXIS_2] + POS_TOLERANCE },
			{ actTargetPos[ID_Z_AXIS] - POS_TOLERANCE, actTargetPos[ID_Z_AXIS] + POS_TOLERANCE },
		};

		if (!start && !moving)
		{
			return;
		}
		else if (start && !moving)
		{
			/*  *  - schreibe neue Zielpos in aktuelle Zielpos
				*  - übergebe neue Zielpos an Dynamixel
				*  - starte Bewegung der Motoren (synch Start?)
				*  - setze moving
				*  - startkomando zurück setzten
				*  - ende */

				/* goal position changed? */
			if (actPos[ID_AXIS_1] < posWindow[ID_AXIS_1][MIN_POS] || actPos[ID_AXIS_1] > posWindow[ID_AXIS_1][MAX_POS] ||
				actPos[ID_AXIS_2] < posWindow[ID_AXIS_2][MIN_POS] || actPos[ID_AXIS_2] > posWindow[ID_AXIS_2][MAX_POS] ||
				actPos[ID_Z_AXIS] < posWindow[ID_Z_AXIS][MIN_POS] || actPos[ID_Z_AXIS] > posWindow[ID_Z_AXIS][MAX_POS])
			{
				SetObjData(OBJ_IDX_POS_REACHED, 0, false);
				
				if (newTargetPos[ID_AXIS_1] != actTargetPos[ID_AXIS_1])
				{
					SetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_X_NEW_TARGET_POS), false);
				}
				if (newTargetPos[ID_AXIS_2] != actTargetPos[ID_AXIS_2])
				{
					SetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Y_NEW_TARGET_POS), false);
				}
				if (newTargetPos[ID_Z_AXIS] != actTargetPos[ID_Z_AXIS])
				{
					SetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Z_NEW_TARGET_POS), false);

					if (newTargetPos[ID_Z_AXIS] > actTargetPos[ID_Z_AXIS])
					{
						dynamixelTurn(ID_Z_AXIS, RIGTH, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); /* besser über moveRW für sync start mit x & y */
					}
					else
					{
						dynamixelTurn(ID_Z_AXIS, LEFT, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); /* besser über moveRW für sync start mit x & y */
					}
				}	
				int16_t val1 = GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS);
				int16_t val2 = GetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS);
				/* calculate new target position*/
				CalcAngle(&val1, &val2);
				dynamixelMoveSpeedRW(ID_AXIS_1, DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE)), GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_SPEED));
				dynamixelMoveSpeedRW(ID_AXIS_2, DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE)), GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_SPEED));
				dynamixelAction(); /* sync start */

				SetObjData(OBJ_IDX_MOVING, 1, false);
				SetObjData(OBJ_IDX_START_MOVE, 0, false);
			}
			else
			{
				SetObjData(OBJ_IDX_START_MOVE, 0, false);
			}
		}
		else if (start && moving)
		{
			SetObjData(OBJ_IDX_START_MOVE, 0, false);
		}
		else /* (!start && moving) */
		{
			/* target position reached? */
			if (actPos[ID_AXIS_1] >= posWindow[ID_AXIS_1][MIN_POS] && actPos[ID_AXIS_1] <= posWindow[ID_AXIS_1][MAX_POS] &&
				actPos[ID_AXIS_2] >= posWindow[ID_AXIS_2][MIN_POS] && actPos[ID_AXIS_2] <= posWindow[ID_AXIS_2][MAX_POS] &&
				actPos[ID_Z_AXIS] >= posWindow[ID_Z_AXIS][MIN_POS] && actPos[ID_Z_AXIS] <= posWindow[ID_Z_AXIS][MAX_POS])
			{  
				SetObjData(OBJ_IDX_MOVING, 0, false);
				SetObjData(OBJ_IDX_POS_REACHED, 1, false);
			}
			else
			{
				/* z-achse rampe geschwindigkeit */
			}
		}
	}
	else
	{
		/* stoppe Bewegung, deaktiviere Drehmoment? */
	}
}