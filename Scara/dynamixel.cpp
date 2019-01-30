// 
// 
// 

#include "dynamixel.h"
#include "objdir.h"
#include "status.h"
#include "calc.h"

#include <DynamixelSerial2.h>

void InitDynamixel(void) {

	/* Search for servo motors */
	int tmp = 0;
	int response = 0;

	for (int i = 0; i <= 2; i++) {
		tmp = Dynamixel.ping(i);

		if (tmp == -1) {
			char msg[32];
			sprintf(msg, "no Dynamixel found @ ID: %d", i);
			SendStatus("InitDynamixel(): ", msg, STATUS_TYPE_ERROR);
		}
		else {
			char msg[32];
			sprintf(msg, "Dynamixel found @ ID: %d", i);
			SendStatus("InitDynamixel(): ", msg, STATUS_TYPE_INFO);
			response++;
		}
	}

	if (response != 3) {
		SendStatus("InitDynamixel(): ", "check wiring of dynamixel servos and restart the controller", STATUS_TYPE_ERROR);
		/* set system error state to prevent further operations */
#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR);
#endif
		return;
	}
	else {

		for (uint8_t i = 0; i < ID_TOTAL_SIZE; i++)	{ /* write configuration parameters to dynamixel */
			
			for (size_t i = 0; i < length; i++)	{

				uint8_t error = Dynamixel.setTempLimit(i, cfgList[i].tempLimit);

				if (error < 0) {
					DynamixelError(error * (-1), i);
					return;
				}
			}

			/*
			Dynamixel.setTempLimit(i, cfgList[i].tempLimit);
			Dynamixel.setVoltageLimit(i, cfgList[i].minVoltageLimit, cfgList[i].maxVoltageLimit);
			Dynamixel.setMaxTorque(i, cfgList[i].maxTorque);
			Dynamixel.setSRL(i, cfgList[i].srl);
			Dynamixel.setRDT(i, cfgList[i].rdt);
			Dynamixel.setLEDAlarm(i, cfgList[i].ledAlarm);
			Dynamixel.setShutdownAlarm(i, cfgList[i].shutdownAlarm);
			Dynamixel.setCSlope(i, cfgList[i].cwCSlope, cfgList[i].ccwCSlope);
			Dynamixel.setCMargin(i, cfgList[i].cwCMargin, cfgList[i].ccwCMargin);
			Dynamixel.setPunch(i, cfgList[i].punch);
			*/
		}
	
		Dynamixel.setEndless(ID_Z_AXIS, ON);

		// enable torque for all servo motors
		Dynamixel.torqueStatus(BROADCAST_ID, ON);
	}
}

void DynamixelError(uint8_t errorBit, uint8_t id) {

	if (errorBit > 0 && errorBit <= 0x80) {
		char msg[64];

		switch (errorBit) {

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
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR);
#endif
	}
}

void UpdateObjDir(void) {

	int16_t data[ID_TOTAL_SIZE][2] = { 0 };
	uint8_t error = 0;

	if (!(GetObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_ERROR)) {
		
		for (uint8_t id = 0; id < (ID_TOTAL_SIZE); id++) {
			data[id][pos] = Dynamixel.readPosition(id); /********** was mit z achse? was für Rückgabewerte bei continous turn modus? ************/
			data[id][speed] = Dynamixel.readSpeed(id);

			if (data[id][pos] < 0 || data[id][speed] < 0) {
				
				if (data[id][pos] < 0) { /* error value is negative by dynamixel library */
					error = data[id][pos] * (-1); 
				}
				else {
					error = data[id][speed] * (-1);
				}
				DynamixelError(error, id);
				return;
			}
			else {

				switch (id)	{ /* store actual speed and position values, abort with error state if unknown id is detected */

				case DYNA_ID_AXIS_1:
					SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, DYNA_TO_DEG(data[id][pos]));
					SetObjData(OBJ_IDX_AXIS_1_ACTUAL_SPEED, data[id][speed]);
					break;
				case DYNA_ID_AXIS_2:
					SetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE, DYNA_TO_DEG(data[id][pos]));
					SetObjData(OBJ_IDX_AXIS_2_ACTUAL_SPEED, data[id][speed]);
					break;
				case DYNA_ID_AXIS_Z:
					SetObjData(OBJ_IDX_Z_ACTUAL_POS, UpdateZPos());
					SetObjData(OBJ_IDX_Z_ACTUAL_SPEED, data[id][speed]);
					break;
				default:
					SendStatus("UpdateObjDir(): ", "unknown device ID", SYS_STAT_ERROR);
					return;
				}
			}
		}
		if (CalcPosistion(data[ID_AXIS_1][pos], data[ID_AXIS_2][pos]) == -1) { /* calcualte related x and y positions */
#ifndef _DEBUG
			SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_ERROR);
#endif
		}
		if (data[ID_AXIS_1][speed] > 0 || data[ID_AXIS_2][speed] > 0 || data[ID_Z_AXIS][speed] > 0) { /********** Was ist mit z-achse?? Wie sehen die Geschwindigkeitswerte aus? > 0 richtig ? ************/
			SetObjData(OBJ_IDX_MOVING, 1); /* system is moving */
		}
		else {
			SetObjData(OBJ_IDX_MOVING, 0); /* system is not moving */
		}
	}
}

void HandleMove(void) {

	if (!(GetObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_ERROR)) {

		uint8_t moving = (uint8_t)GetObjData(OBJ_IDX_MOVING);
		uint8_t start = (uint8_t)GetObjData(OBJ_IDX_START_MOVE);
		
		float actPos[3] = { /* actual position values */
			GetObjData(OBJ_IDX_X_ACTUAL_POS) / 10.0,
			GetObjData(OBJ_IDX_Y_ACTUAL_POS) / 10.0,
			GetObjData(OBJ_IDX_Z_ACTUAL_POS) / 10.0
		};
		float actTargetPos[3] = { /* actual target position values */
			GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS) / 10.0
		};
		float newTargetPos[3] = { /* new target position values */
			GetObjData(OBJ_IDX_X_NEW_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Y_NEW_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Z_NEW_TARGET_POS) / 10.0,
		};
		float posWindow[3][2] = { /* allowed position windows */
			{ actTargetPos[ID_AXIS_1] - POS_TOLERANCE, actTargetPos[ID_AXIS_1] + POS_TOLERANCE },
			{ actTargetPos[ID_AXIS_2] - POS_TOLERANCE, actTargetPos[ID_AXIS_2] + POS_TOLERANCE },
			{ actTargetPos[ID_Z_AXIS] - POS_TOLERANCE, actTargetPos[ID_Z_AXIS] + POS_TOLERANCE },
		};

		if (!start && !moving) {
			return;
		}
		else if (start && !moving) {

			/*  *  - schreibe neue Zielpos in aktuelle Zielpos
				*  - übergebe neue Zielpos an Dynamixel
				*  - starte Bewegung der Motoren (synch Start?)
				*  - setze moving
				*  - startkomando zurück setzten
				*  - ende */

			if (actPos[ID_AXIS_1] < posWindow[ID_AXIS_1][MIN_POS] || actPos[ID_AXIS_1] > posWindow[ID_AXIS_1][MAX_POS] ||
				actPos[ID_AXIS_2] < posWindow[ID_AXIS_2][MIN_POS] || actPos[ID_AXIS_2] > posWindow[ID_AXIS_2][MAX_POS] ||
				actPos[ID_Z_AXIS] < posWindow[ID_Z_AXIS][MIN_POS] || actPos[ID_Z_AXIS] > posWindow[ID_Z_AXIS][MAX_POS]) { /* goal position changed? */

				if (newTargetPos[ID_AXIS_1] != actTargetPos[ID_AXIS_1]) {
					SetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_X_NEW_TARGET_POS));
				}
				if (newTargetPos[ID_AXIS_2] != actTargetPos[ID_AXIS_2]) {
					SetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Y_NEW_TARGET_POS));
				}
				if (newTargetPos[ID_Z_AXIS] != actTargetPos[ID_Z_AXIS]) {
					SetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Z_NEW_TARGET_POS));

					if (newTargetPos[ID_Z_AXIS] > actTargetPos[ID_Z_AXIS]) {
						Dynamixel.turn(ID_Z_AXIS, RIGTH, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); /* besser über moveRW für sync start mit x & y */
					}
					else {
						Dynamixel.turn(ID_Z_AXIS, LEFT, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); /* besser über moveRW für sync start mit x & y */
					}
				}
				CalcAngle(GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS), GetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS));
				Dynamixel.moveSpeedRW(ID_AXIS_1, DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE)), GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_SPEED));
				Dynamixel.moveSpeedRW(ID_AXIS_2, DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE)), GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_SPEED));
				Dynamixel.action(); /* sync start */
				//SetObjData(OBJ_IDX_MOVING, 1);
				SetObjData(OBJ_IDX_START_MOVE, 0);
			}
			else {
				SetObjData(OBJ_IDX_START_MOVE, 0);
			}
		}
		else if (start && moving) {
			SetObjData(OBJ_IDX_START_MOVE, 0);
		}
		else /* (!start && moving) */ {

			if (actPos[ID_AXIS_1] >= posWindow[ID_AXIS_1][MIN_POS] && actPos[ID_AXIS_1] <= posWindow[ID_AXIS_1][MAX_POS] &&
				actPos[ID_AXIS_2] >= posWindow[ID_AXIS_2][MIN_POS] && actPos[ID_AXIS_2] <= posWindow[ID_AXIS_2][MAX_POS] &&
				actPos[ID_Z_AXIS] >= posWindow[ID_Z_AXIS][MIN_POS] && actPos[ID_Z_AXIS] <= posWindow[ID_Z_AXIS][MAX_POS]) {  /* target position reached? */
				
				//SetObjData(OBJ_IDX_MOVING, 0);
				SetObjData(OBJ_IDX_POS_REACHED, 1);
			}
			else {
				/* z-achse rampe geschwindigkeit */
			}
		}
	}
	else {
		/* stoppe Bewegung, deaktiviere Drehmoment? */
	}
}