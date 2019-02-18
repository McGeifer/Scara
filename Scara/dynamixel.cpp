// 
// 
// 

#include "dynamixel.h"
#include "objdir.h"
#include "status.h"
#include "calc.h"
#include "gpio.h"



static dxl_t dxlCrtlTbl[] = {
	{DXL_P_MODOEL_NUMBER_H,			DXL_P_RD,		0,	0},
	{DXL_P_MODEL_NUMBER_L,			DXL_P_RD,		0,	0},
	{DXL_P_VERSION,					DXL_P_RD,		0,	0},
	{DXL_P_ID,						DXL_P_RD_WR,	0,	253},
	{DXL_P_BAUD_RATE,				DXL_P_RD_WR,	0,	254},
	{DXL_P_RETURN_DELAY_TIME,		DXL_P_RD_WR,	0,	254},
	{DXL_P_CW_ANGLE_LIMIT_L,		DXL_P_RD_WR,	0,	1023},
	{DXL_P_CW_ANGLE_LIMIT_H,		DXL_P_RD_WR,	0,	1023},
	{DXL_P_CCW_ANGLE_LIMIT_L,		DXL_P_RD_WR,	0,	1023},
	{DXL_P_CCW_ANGLE_LIMIT_H,		DXL_P_RD_WR,	0,	1023},
	{DXL_P_LIMIT_TEMPERATURE,		DXL_P_RD_WR,	0,	150},
	{DXL_P_DOWN_LIMIT_VOLTAGE,		DXL_P_RD_WR,	50, 250},
	{DXL_P_UP_LIMIT_VOLTAGE,		DXL_P_RD_WR,	50, 250},
	{DXL_P_MAX_TORQUE_L,			DXL_P_RD_WR,	0,	1023},
	{DXL_P_MAX_TORQUE_H,			DXL_P_RD_WR,	0,	1023},
	{DXL_P_RETURN_LEVEL,			DXL_P_RD_WR,	0,	2},
	{DXL_P_ALARM_LED,				DXL_P_RD_WR,	0,	127},
	{DXL_P_ALARM_SHUTDOWN,			DXL_P_RD_WR,	0,	127},
	{DXL_P_DOWN_CALIBRATION_L,		DXL_P_RD,		0,	0},
	{DXL_P_DOWN_CALIBRATION_H,		DXL_P_RD,		0,	0},
	{DXL_P_UP_CALIBRATION_L,		DXL_P_RD,		0,	0},
	{DXL_P_UP_CALIBRATION_H,		DXL_P_RD,		0,	0},
	{DXL_P_TORQUE_ENABLE,			DXL_P_RD_WR,	0,	1},
	{DXL_P_LED,						DXL_P_RD_WR,	0,	1},
	{DXL_P_CW_COMPLIANCE_MARGIN,	DXL_P_RD_WR,	0,	254},
	{DXL_P_CCW_COMPLIANCE_MARGIN,	DXL_P_RD_WR,	0,	254},
	{DXL_P_CW_COMPLIANCE_SLOPE,		DXL_P_RD_WR,	1,	254},
	{DXL_P_CCW_COMPLIANCE_SLOPE,	DXL_P_RD_WR,	1,	254},
	{DXL_P_GOAL_POSITION_L,			DXL_P_RD_WR,	0,	1023},
	{DXL_P_GOAL_POSITION_H,			DXL_P_RD_WR,	0,	1023},
	{DXL_P_GOAL_SPEED_L,			DXL_P_RD_WR,	0,	1023},
	{DXL_P_GOAL_SPEED_H,			DXL_P_RD_WR,	0,	1023},
	{DXL_P_TORQUE_LIMIT_L,			DXL_P_RD_WR,	0,	1023},
	{DXL_P_TORQUE_LIMIT_H,			DXL_P_RD_WR,	0,	1023},
	{DXL_P_PRESENT_POSITION_L,		DXL_P_RD,		0,	0},
	{DXL_P_PRESENT_POSITION_H,		DXL_P_RD,		0,	0},
	{DXL_P_PRESENT_SPEED_L,			DXL_P_RD,		0,	0},
	{DXL_P_PRESENT_SPEED_H,			DXL_P_RD,		0,	0},
	{DXL_P_PRESENT_LOAD_L,			DXL_P_RD,		0,	0},
	{DXL_P_PRESENT_LOAD_H,			DXL_P_RD,		0,	0},
	{DXL_P_PRESENT_VOLTAGE,			DXL_P_RD,		0,	0},
	{DXL_P_PRESENT_TEMPERATURE,		DXL_P_RD,		0,	0},
	{DXL_P_REGISTERED_INSTRUCTION,	DXL_P_RD_WR,	0,	1},
	{DXL_P_MOVING,					DXL_P_RD,		0,	0},
	{DXL_P_LOCK,					DXL_P_RD_WR,	1,	1},
	{DXL_P_PUNCH_L,					DXL_P_RD_WR,	0,	1023},
	{DXL_P_PUNCH_H,					DXL_P_RD_WR,	0,	1023},
};

/* 
 * Initialization of the parameter list for setting up the Dynamixel
 * servos when starting the controller
 */
static uint16_t dynaParamList[3][13] = 
{
	{	DXL_AXIS_1_LED_ALARM,
		DXL_AXIS_1_TEMP_LIMIT,
		DXL_AXIS_1_LOW_VOLTAGE_LIMIT,
		DXL_AXIS_1_HIGH_VOLTAGE_LIMIT,
		DXL_AXIS_1_MAX_TORQUE,
		DXL_AXIS_1_SRL,
		DXL_AXIS_1_RDT,
		DXL_AXIS_1_SHUT_DOWN_ALARM,
		DXL_AXIS_1_CW_C_SLOPE,
		DXL_AXIS_1_CCW_C_SLOPE,
		DXL_AXIS_1_CW_C_MARGIN,
		DXL_AXIS_1_CCW_C_MARGIN,
		DXL_AXIS_1_PUNCH
	},
	{	DXL_Z_AXIS_LED_ALARM,
		DXL_Z_AXIS_TEMP_LIMIT,
		DXL_AXIS_2_LOW_VOLTAGE_LIMIT,
		DXL_AXIS_2_HIGH_VOLTAGE_LIMIT,
		DXL_Z_AXIS_MAX_TORQUE,
		DXL_Z_AXIS_SRL,
		DXL_Z_AXIS_RDT,
		DXL_Z_AXIS_SHUT_DOWN_ALARM,
		DXL_Z_AXIS_CW_C_SLOPE,
		DXL_Z_AXIS_CCW_C_SLOPE,
		DXL_Z_AXIS_CW_C_MARGIN,
		DXL_Z_AXIS_CCW_C_MARGIN,
		DXL_Z_AXIS_PUNCH
	},
	{	DXL_Z_AXIS_LED_ALARM,
		DXL_Z_AXIS_TEMP_LIMIT,
		DXL_Z_AXIS_LOW_VOLTAGE_LIMIT,
		DXL_Z_AXIS_HIGH_VOLTAGE_LIMIT,
		DXL_Z_AXIS_MAX_TORQUE,
		DXL_Z_AXIS_SRL,
		DXL_Z_AXIS_RDT,
		DXL_Z_AXIS_SHUT_DOWN_ALARM,
		DXL_Z_AXIS_CW_C_SLOPE,
		DXL_Z_AXIS_CCW_C_SLOPE,
		DXL_Z_AXIS_CW_C_MARGIN,
		DXL_Z_AXIS_CCW_C_MARGIN,
		DXL_Z_AXIS_PUNCH
	}
};

void InitDynamixel(void) 
{
	int tmp = -1;
	int response = 0;

	/* search for servo motors */
	for (int i = 0; i <= 2; i++)
	{
		tmp = dynamixelPing(i) * (-1);

		if (tmp != 0 )
		{
			DynamixelError(tmp, i);
			char msg[32];
			sprintf(msg, "no Dynamixel found @ ID: %d", i);
			SendStatus("InitDynamixel(): ", msg, STATUS_MSG_TYPE_ERROR);
		}
		else
		{
			char msg[64];
			sprintf(msg, "Dynamixel found @ ID: %d", i);
			SendStatus("InitDynamixel(): ", msg, STATUS_MSG_TYPE_INFO);
			response++;
		}
	}

	if (response != 3)
	{
		SendStatus("InitDynamixel(): ", "check wiring of Dynamixel servos and restart the controller", STATUS_MSG_TYPE_ERROR);
		/* if one of the servos could not be found then set the system error state to prevent further operations */
#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR, false);
#endif
		return;
	}
	else
	{
		/* 
		 * List of setup functions for the dynamixel servos. The functions will be executed for all 3 Dynamixel servos by using the parameter list above.
		 * The list also stores the function names for proper error messages. Do not change the order of the functions in the structure unless you change the
		 * dynaParamList accordingly
		 */
		funcPtrTbl_t funcPtrTbl[] = {
		
			{dynamixelSetLEDAlarm, "dynamixelSetLEDAlarm"},
			{dynamixelSetTempLimit, "dynamixelSetTempLimit"},
			{dynamixelSetLowVoltageLimit, "dynamixelSetLowVoltageLimit"},
			{dynamixelSetHighVoltageLimit, "dynamixelSetHighVoltageLimit"},
			{dynamixelSetMaxTorque, "dynamixelSetMaxTorque"},
			{dynamixelSetSRL, "dynamixelSetSRL"},
			{dynamixelSetRDT, "dynamixelSetRDT"},
			{dynamixelSetShutdownAlarm, "dynamixelSetShutdownAlarm"},
			{dynamixelSetCWCSlope, "dynamixelSetCWCSlope"},
			{dynamixelSetCCWCSlope, "dynamixelSetCCWCSlope"},
			{dynamixelSetCWCMargin, "dynamixelSetCWCMargin"},
			{dynamixelSetCCWCMargin, "dynamixelSetCCWCMargin"},
			{dynamixelSetPunch, "dynamixelSetPunch"}
		};

		/* execute setup functions */
		for (uint8_t i = 0; i < DXL_ID_SUM; i++) /* outer loop for iterating the servo ids */
		{ 
			funcPtrTbl_t *p = NULL;
			p = funcPtrTbl;

			for (uint8_t j = 0; j < (sizeof(funcPtrTbl) / sizeof(funcPtrTbl[0])); j++, p++) /* inner loop for iterating the parameter list */
			{
				int16_t error = p->funcPtr(i, dynaParamList[i][j]);
				Serial.print("i = ");
				Serial.println(i);
				Serial.print("j = ");
				Serial.println(j);
				Serial.print("error = ");
				Serial.println(error);

				if (error < 0) 
				{
					DynamixelError(error * (-1), i);	/* executing the setup functions */
					char msg[64];
					sprintf(msg, "Error while calling: %s", p->funcName);
					SendStatus("InitDynamixel(): ", msg, STATUS_MSG_TYPE_ERROR);
					//return;
				}
			}
		}
		/* enable continuous turn for z axis */
		if (int16_t error = dynamixelSetEndless(DXL_ID_Z_AXIS, ON) < 0)
		{
			DynamixelError(error * (-1), DXL_ID_Z_AXIS);
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

void DynamixelError(int16_t errorBit, uint8_t id)
{
	if (errorBit > 0 && errorBit <= 0x0080)
	{
		char msg[64];

		switch (errorBit)
		{
		case 0x0001:
			sprintf(msg, "Dynamixel - Input Voltage Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
			break;

		case 0x0002:
			sprintf(msg, "Dynamixel - Angle Limit Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
			break;

		case 0x0004:
			sprintf(msg, "Dynamixel - Overheating Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
			break;

		case 0x0008:
			sprintf(msg, "Dynamixel - Range Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
			break;

		case 0x0010:
			sprintf(msg, "Dynamixel - Checksum Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
			break;

		case 0x0020:
			sprintf(msg, "Dynamixel - Overload Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
			break;

		case 0x0040:
			sprintf(msg, "Dynamixel - Instruction Error @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
			break;

		case 0x0080:
		default:
			sprintf(msg, "Dynamixel - no response @ ID: %d", id);
			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
			break;
		}
#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR, false);
#endif
	}
//	else
//	{
//		char msg[64];
//
//		if (errorBit == -1)
//		{
//			sprintf(msg, "Dynamixel - no response @ ID: %d", id);
//			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
//		}
//		else
//		{
//			sprintf(msg, "Dynamixel - unknown Error bit @ ID: %d", id);
//			SendStatus("DynamixelError(): ", msg, STATUS_MSG_TYPE_ERROR);
//		}
//#ifndef _DEBUG
//		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR, false);
//#endif
//	}
}



void HandleMove(void) {

	enum minMaxPos {
		MIN_POS,
		MAX_POS
	};

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
			{ actTargetPos[DXL_ID_AXIS_1] - DXL_POS_TOLERANCE, actTargetPos[DXL_ID_AXIS_1] + DXL_POS_TOLERANCE },
			{ actTargetPos[DXL_ID_AXIS_2] - DXL_POS_TOLERANCE, actTargetPos[DXL_ID_AXIS_2] + DXL_POS_TOLERANCE },
			{ actTargetPos[DXL_ID_Z_AXIS] - DXL_POS_TOLERANCE, actTargetPos[DXL_ID_Z_AXIS] + DXL_POS_TOLERANCE },
		};

		if (!start && !moving)
		{
			return;
		}
		else if (start && !moving)
		{
			/*  *  - schreibe neue Zielpos in aktuelle Zielpos
				*  - übergebe neue Zielpos an Dynamixel
				*  - starte Bewegung der Motoren (sync Start?)
				*  - setze moving
				*  - start Kommando zurück setzten
				*  - ende */

				/* goal position changed? */
			if (actPos[DXL_ID_AXIS_1] < posWindow[DXL_ID_AXIS_1][MIN_POS] || actPos[DXL_ID_AXIS_1] > posWindow[DXL_ID_AXIS_1][MAX_POS] ||
				actPos[DXL_ID_AXIS_2] < posWindow[DXL_ID_AXIS_2][MIN_POS] || actPos[DXL_ID_AXIS_2] > posWindow[DXL_ID_AXIS_2][MAX_POS] ||
				actPos[DXL_ID_Z_AXIS] < posWindow[DXL_ID_Z_AXIS][MIN_POS] || actPos[DXL_ID_Z_AXIS] > posWindow[DXL_ID_Z_AXIS][MAX_POS])
			{
				SetObjData(OBJ_IDX_POS_REACHED, 0, false);
				
				if (newTargetPos[DXL_ID_AXIS_1] != actTargetPos[DXL_ID_AXIS_1])
				{
					SetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_X_NEW_TARGET_POS), false);
				}
				if (newTargetPos[DXL_ID_AXIS_2] != actTargetPos[DXL_ID_AXIS_2])
				{
					SetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Y_NEW_TARGET_POS), false);
				}
				if (newTargetPos[DXL_ID_Z_AXIS] != actTargetPos[DXL_ID_Z_AXIS])
				{
					SetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Z_NEW_TARGET_POS), false);

					if (newTargetPos[DXL_ID_Z_AXIS] > actTargetPos[DXL_ID_Z_AXIS])
					{
						dynamixelTurn(DXL_ID_Z_AXIS, RIGTH, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); /* besser über moveRW für sync start mit x & y */
					}
					else
					{
						dynamixelTurn(DXL_ID_Z_AXIS, LEFT, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); /* besser über moveRW für sync start mit x & y */
					}
				}	
				int16_t val1 = GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS);
				int16_t val2 = GetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS);
				/* calculate new target position*/
				CalcAngle(&val1, &val2);
				dynamixelMoveSpeedRW(DXL_ID_AXIS_1, DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE)), GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_SPEED));
				dynamixelMoveSpeedRW(DXL_ID_AXIS_2, DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE)), GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_SPEED));
				dynamixelAction(); /* sync start */

				SetObjData(OBJ_IDX_MOVING, 1, true);
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
			if (actPos[DXL_ID_AXIS_1] >= posWindow[DXL_ID_AXIS_1][MIN_POS] && actPos[DXL_ID_AXIS_1] <= posWindow[DXL_ID_AXIS_1][MAX_POS] &&
				actPos[DXL_ID_AXIS_2] >= posWindow[DXL_ID_AXIS_2][MIN_POS] && actPos[DXL_ID_AXIS_2] <= posWindow[DXL_ID_AXIS_2][MAX_POS] &&
				actPos[DXL_ID_Z_AXIS] >= posWindow[DXL_ID_Z_AXIS][MIN_POS] && actPos[DXL_ID_Z_AXIS] <= posWindow[DXL_ID_Z_AXIS][MAX_POS])
			{  
				SetObjData(OBJ_IDX_MOVING, 0, true);
				SetObjData(OBJ_IDX_POS_REACHED, 1, false);
			}
			else
			{
				/* z-Achse Rampe Geschwindigkeit */
			}
		}
	}
	else
	{
		/* stoppe Bewegung, deaktiviere Drehmoment? */
	}
}

/*  */
static dxl_t* GetDxlControlTablePointer(uint8_t dxlInstruction)
{
	dxl_t *p = NULL;
	p = dxlCrtlTbl;

	for (uint8_t i = 0; i < (sizeof(dxlCrtlTbl) / sizeof(dxlCrtlTbl[0])); i++, p++)
	{
		if (p->adress == dxlInstruction)
		{
			return p;
		}
	}
	return NULL;
}

uint8_t DxlWrite(uint8_t dxlID, uint8_t dxlInstruction, uint8_t *paramList)
{
	dxl_t *p = NULL;
	p = GetDxlControlTablePointer(dxlInstruction);

	if (p != NULL)
	{

	}
	else
	{
		/* object does not exist */
		return -1;
	}
}

void DxlStartCom(uint16_t baudRate, uint8_t directionPin)
{
	/* set pin mode! */
	beginCom(baudRate, directionPin);
}