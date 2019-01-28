// 
// 
// 

#include "dynamixel.h"
#include "objdir.h"
#include "status.h"
#include "calc.h"

#include <DynamixelSerial2.h>

void InitDynamixel() {

	// Search for servo motors
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
		// set system error state to prevent further operations
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR); // bitmask!
		return;
	}
	else {

	// basic settings for the dynamixel servo motors

	// ########################
	// Axis 1
	// ########################

	Dynamixel.setTempLimit(x, 70);				// max temperatur 70°C
	Dynamixel.setVoltageLimit(x, 110, 120);		// min voltage 11V, max 12V
	Dynamixel.setMaxTorque(x, 1023);			// max torque -> max value
	Dynamixel.setSRL(x, 1);						// return level 0=none, 1=only for read comand, 2= always
	Dynamixel.setRDT(x, 250);					// return delay time = 2µs * vaule -> 500µs
	Dynamixel.setLEDAlarm(x, 127);				// LED blink for all error types
	Dynamixel.setShutdownAlarm(x, 37);			// turn of torque for: overload, overheating and input voltage error

	// Only change if you really know what you are doing!
	Dynamixel.setCSlope(x, 32, 32);				// compilance slope - default value
	Dynamixel.setCMargin(x, 0, 0);				// compilance margin - default value
	Dynamixel.setPunch(x, 32);					// minimum current supplied to the motor - default value

	// ########################
	// Axis 2
	// ########################

	Dynamixel.setTempLimit(y, 70);				// max temperatur 70°C
	Dynamixel.setVoltageLimit(y, 110, 120);		// min voltage 11V, max 12V
	Dynamixel.setMaxTorque(y, 1023);			// max torque -> max value
	Dynamixel.setSRL(y, 1);						// return level 0=none, 1=only for read comand, 2= always
	Dynamixel.setRDT(y, 250);					// return delay time = 2µs * vaule -> 500µs
	Dynamixel.setLEDAlarm(y, 127);				// LED blink for all error types
	Dynamixel.setShutdownAlarm(y, 37);			// turn of torque for: overload, overheating and input voltage error

	// Only change if you really know what you are doing!
	Dynamixel.setCSlope(y, 32, 32);				// compilance slope - default value
	Dynamixel.setCMargin(y, 0, 0);				// compilance margin - default value
	Dynamixel.setPunch(y, 32);					// minimum current supplied to the motor - default value

	// ########################
	// Axis 3 (z-axis)
	// ########################

	Dynamixel.setTempLimit(z, 70);				// max temperatur 70°C
	Dynamixel.setVoltageLimit(z, 110, 120);		// min voltage 11V, max 12V
	Dynamixel.setMaxTorque(z, 1023);			// max torque -> max value
	Dynamixel.setSRL(z, 1);						// return level 0=none, 1=only for read comand, 2= always
	Dynamixel.setRDT(z, 250);					// return delay time = 2µs * vaule -> 500µs
	Dynamixel.setLEDAlarm(z, 127);				// LED blink for all error types
	Dynamixel.setShutdownAlarm(z, 37);			// turn of torque for: overload, overheating and input voltage error

	// Only change if you really know what you are doing!
	Dynamixel.setCSlope(z, 32, 32);				// compilance slope - default value
	Dynamixel.setCMargin(z, 0, 0);				// compilance margin - default value
	Dynamixel.setPunch(z, 32);					// minimum current supplied to the motor - default value
	Dynamixel.setEndless(z, ON);

	// enable torque for all servo motors
	Dynamixel.torqueStatus(BROADCAST_ID, ON);
	}
}

void DynamixelError(uint8_t errorBit, uint8_t id) {

	char msg[64];

	if (errorBit > 0 && errorBit <= 0x80) {

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
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR);
	}
}

void UpdatePos(void) {

	uint8_t id = 0;
	int16_t data[3] = { 0 };
	uint8_t error = 0;

	if (!(GetObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_DYNAMIXEL_ERROR)) {
		
		for (id = 0; id < 3; id++) {
			data[id] = Dynamixel.readPosition(id); /*was mit z achse? was für Rückgabewerte bei continous turn modus?*/

			if (data[id] < 0) {
				error = data[id] * (-1); /*error is negative by dynamixel library*/
				DynamixelError(error, id);
				return;
			}
			else {

				if (id == DYNA_ID_AXIS_1) {
					SetObjData(OBJ_IDX_AXIS_1_ACTUAL_ANGLE, DYNA_TO_DEG(data[id]));
				}
				else if (id == DYNA_ID_AXIS_2) {
					SetObjData(OBJ_IDX_AXIS_2_ACTUAL_ANGLE, DYNA_TO_DEG(data[id]));
				}
				else if (id == DYNA_ID_AXIS_Z) {
					SetObjData(OBJ_IDX_Z_ACTUAL_POS, UpdateZPos());
				}
			}
		}
		if (CalcPosistion(data[0], data[1]) == -1) {
			SetObjData(OBJ_IDX_SYS_STATUS, (GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_ERROR));
		}
	}
}

void HandleMove(void) {

	if (!(GetObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_DYNAMIXEL_ERROR)) {
		Serial.println("HandleMove();");

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
		float posWindow[3][2] = { /* allowed position window */
			{ actPos[x] - POS_TOLERANCE, actPos[x] + POS_TOLERANCE },
			{ actPos[y] - POS_TOLERANCE, actPos[y] + POS_TOLERANCE },
			{ actPos[z] - POS_TOLERANCE, actPos[z] + POS_TOLERANCE },
		};

		if (start && !moving) {

			/*  - schreibe neue Zielpos in aktuelle Zielpos
				*  - übergebe neue Zielpos an Dynamixel
				*  - starte Bewegung der Motoren (synch Start?)
				*  - setze moving
				*  - startkomando zurück setzten
				*  - ende */

			if (actPos[x] < posWindow[x][minPos] || actPos[x] > posWindow[x][maxPos] ||
				actPos[y] < posWindow[y][minPos] || actPos[y] > posWindow[y][maxPos] ||
				actPos[z] < posWindow[z][minPos] || actPos[z] > posWindow[z][maxPos]) {

				if (newTargetPos[x] != actTargetPos[x]) {
					SetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_X_NEW_TARGET_POS));
				}
				if (newTargetPos[y] != actTargetPos[y]) {
					SetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Y_NEW_TARGET_POS));
				}
				if (newTargetPos[z] != actTargetPos[z]) {
					SetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Z_NEW_TARGET_POS));

					if (newTargetPos[z] > actTargetPos[z]) {
						Dynamixel.turn(z, RIGTH, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); // besser über moveRW für sync start mit x & y
					}
					else {
						Dynamixel.turn(z, LEFT, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); // besser über moveRW für sync start mit x & y
					}
				}
				CalcAngle(GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS), GetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS));
				Dynamixel.moveSpeedRW(x, DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE)), GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_SPEED));
				Dynamixel.moveSpeedRW(y, DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE)), GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_SPEED));
				Dynamixel.action(); /* sync start */
				SetObjData(OBJ_IDX_MOVING, 1);
				SetObjData(OBJ_IDX_START_MOVE, 0);
			}
			else {
				SetObjData(OBJ_IDX_START_MOVE, 0);
			}
		}
		else if (start && moving) {
			SetObjData(OBJ_IDX_START_MOVE, 0);
		}
		else if (!start && moving) {
			
			if (actPos[x] >= posWindow[x][minPos] && actPos[x] <= posWindow[x][maxPos] &&
				actPos[y] >= posWindow[y][minPos] && actPos[y] <= posWindow[y][maxPos] &&
				actPos[z] >= posWindow[z][minPos] && actPos[z] <= posWindow[z][maxPos]) {  /* target position reached */
				
				SetObjData(OBJ_IDX_MOVING, 0);
				// send pos reached!?
			}
			else {
				// z-achse rampe geschwindigkeit
			}
		}
	}
	else {
		// stoppe Bewegung, deaktiviere Drehmoment
	}
}