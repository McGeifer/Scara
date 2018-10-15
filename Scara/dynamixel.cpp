// 
// 
// 

#include "dynamixel.h"
#include "objdir.h"
#include "DynamixelSerial2.h"
#include "status.h"

void InitDynamixel() {

	// Search for servo motors
	int tmp = 0;
	int response = 0;

	for (int i = 0; i < 2; i++) {

		char* string;
		tmp = Dynamixel.ping(i);
		if (tmp == -1) {

			sprintf(string, "Dynamixel not found @ ID: %d", i);
			SendStatus(string, STATUS_TYPE_ERROR);
		}
		else {

			sprintf(string, "Dynamixel found @ ID: %d", i);
			SendStatus(string, STATUS_TYPE_INFO);
			response++;
		}
	}

	if (response != 3) {

		SendStatus("Not all Dynamixel-Servos could be found, check wiring and restart the controller.", STATUS_TYPE_ERROR);
		
		// set system error state to prevent further operations
		ObjStruct *tmp;
		uint8_t index = 0xFF;
		uint8_t props = OBJ_PROP_RW;
		uint16_t data = 0x01;
		tmp = LocateObj(0xFF);
		tmp->pFunction(&index, &props, &data);
		return;
	}

	// basic settings for the dynamixel servo motors

	// ########################
	// Axis 1 (ID - 0)
	// ########################

	Dynamixel.setTempLimit(0, 70);				// max temperatur 70캜
	Dynamixel.setVoltageLimit(0, 110, 120);		// min voltage 11V, max 12V
	Dynamixel.setMaxTorque(0, 1023);			// max torque -> max value
	Dynamixel.setSRL(0, 1);						// return level 0=none, 1=only for read comand, 2= always
	Dynamixel.setRDT(0, 250);					// return delay time = 2탎 * vaule -> 500탎
	Dynamixel.setLEDAlarm(0, 127);				// LED blink for alle error types
	Dynamixel.setShutdownAlarm(0, 37);			// turn of torque for: overload, overheating and input voltage error
	
	// Only change if you know exactly what	you are doing!
	Dynamixel.setCSlope(0, 32, 32);				// compilance slope - default value
	Dynamixel.setCMargin(0, 0, 0);				// compilance margin - default value
	Dynamixel.setPunch(0, 32);					// minimum current supplied to the motor - dafault value

	// ########################
	// Axis 2 (ID - 1)
	// ########################

	Dynamixel.setTempLimit(1, 70);				// max temperatur 70캜
	Dynamixel.setVoltageLimit(1, 110, 120);		// min voltage 11V, max 12V
	Dynamixel.setMaxTorque(1, 1023);			// max torque -> max value
	Dynamixel.setSRL(1, 1);						// return level 0=none, 1=only for read comand, 2= always
	Dynamixel.setRDT(1, 250);					// return delay time = 2탎 * vaule -> 500탎
	Dynamixel.setLEDAlarm(1, 127);				// LED blink for alle error types
	Dynamixel.setShutdownAlarm(1, 37);			// turn of torque for: overload, overheating and input voltage error
		
	// Only change if you know exactly what	you are doing!
	Dynamixel.setCSlope(1, 32, 32);				// compilance slope - default value
	Dynamixel.setCMargin(1, 0, 0);				// compilance margin - default value
	Dynamixel.setPunch(1, 32);					// minimum current supplied to the motor - dafault value

	// ########################
	// Axis 3 (ID - 2) (z-axis)
	// ########################

	Dynamixel.setTempLimit(1, 70);				// max temperatur 70캜
	Dynamixel.setVoltageLimit(1, 110, 120);		// min voltage 11V, max 12V
	Dynamixel.setMaxTorque(1, 1023);			// max torque -> max value
	Dynamixel.setSRL(1, 1);						// return level 0=none, 1=only for read comand, 2= always
	Dynamixel.setRDT(1, 250);					// return delay time = 2탎 * vaule -> 500탎
	Dynamixel.setLEDAlarm(1, 127);				// LED blink for alle error types
	Dynamixel.setShutdownAlarm(1, 37);			// turn of torque for: overload, overheating and input voltage error
		
	// Only change if you know exactly what	you are doing!
	Dynamixel.setCSlope(1, 32, 32);				// compilance slope - default value
	Dynamixel.setCMargin(1, 0, 0);				// compilance margin - default value
	Dynamixel.setPunch(1, 32);					// minimum current supplied to the motor - dafault value
}