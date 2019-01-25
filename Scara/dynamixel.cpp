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
	// Axis 1 (ID - 0)
	// ########################

	Dynamixel.setTempLimit(0, 70);				// max temperatur 70°C
	Dynamixel.setVoltageLimit(0, 110, 120);		// min voltage 11V, max 12V
	Dynamixel.setMaxTorque(0, 1023);			// max torque -> max value
	Dynamixel.setSRL(0, 1);						// return level 0=none, 1=only for read comand, 2= always
	Dynamixel.setRDT(0, 250);					// return delay time = 2µs * vaule -> 500µs
	Dynamixel.setLEDAlarm(0, 127);				// LED blink for all error types
	Dynamixel.setShutdownAlarm(0, 37);			// turn of torque for: overload, overheating and input voltage error

	// Only change if you really know what you are doing!
	Dynamixel.setCSlope(0, 32, 32);				// compilance slope - default value
	Dynamixel.setCMargin(0, 0, 0);				// compilance margin - default value
	Dynamixel.setPunch(0, 32);					// minimum current supplied to the motor - default value

	// ########################
	// Axis 2 (ID - 1)
	// ########################

	Dynamixel.setTempLimit(1, 70);				// max temperatur 70°C
	Dynamixel.setVoltageLimit(1, 110, 120);		// min voltage 11V, max 12V
	Dynamixel.setMaxTorque(1, 1023);			// max torque -> max value
	Dynamixel.setSRL(1, 1);						// return level 0=none, 1=only for read comand, 2= always
	Dynamixel.setRDT(1, 250);					// return delay time = 2µs * vaule -> 500µs
	Dynamixel.setLEDAlarm(1, 127);				// LED blink for all error types
	Dynamixel.setShutdownAlarm(1, 37);			// turn of torque for: overload, overheating and input voltage error

	// Only change if you really know what you are doing!
	Dynamixel.setCSlope(1, 32, 32);				// compilance slope - default value
	Dynamixel.setCMargin(1, 0, 0);				// compilance margin - default value
	Dynamixel.setPunch(1, 32);					// minimum current supplied to the motor - default value

	// ########################
	// Axis 3 (ID - 2) (z-axis)
	// ########################

	Dynamixel.setTempLimit(1, 70);				// max temperatur 70°C
	Dynamixel.setVoltageLimit(1, 110, 120);		// min voltage 11V, max 12V
	Dynamixel.setMaxTorque(1, 1023);			// max torque -> max value
	Dynamixel.setSRL(1, 1);						// return level 0=none, 1=only for read comand, 2= always
	Dynamixel.setRDT(1, 250);					// return delay time = 2µs * vaule -> 500µs
	Dynamixel.setLEDAlarm(1, 127);				// LED blink for all error types
	Dynamixel.setShutdownAlarm(1, 37);			// turn of torque for: overload, overheating and input voltage error

	// Only change if you really know what you are doing!
	Dynamixel.setCSlope(1, 32, 32);				// compilance slope - default value
	Dynamixel.setCMargin(1, 0, 0);				// compilance margin - default value
	Dynamixel.setPunch(1, 32);					// minimum current supplied to the motor - default value

	// enable torque for all servo motors
	Dynamixel.torqueStatus(0, ON);
	Dynamixel.torqueStatus(1, ON);
	Dynamixel.torqueStatus(2, ON);
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
					SetObjData(OBJ_IDX_Z_ACTUAL_POS, CalcZPos());
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

		if (start_move) {

			if (target_pos != actual_pos) {

			}
		}





		
	}
	else {
		// stoppe Bewegung, deaktiviere Drehmoment
	}
}