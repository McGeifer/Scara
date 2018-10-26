// 
// 
// 

#include "dynamixel.h""
#include "objdir.h"
#include "status.h"

#include <DynamixelSerial2.h>

void InitDynamixel() {

	// Search for servo motors
	int tmp = 0;
	int response = 0;

	for (int i = 0; i <= 2; i++) {
		
		tmp = Dynamixel.ping(i);
		if (tmp == -1) {
			char string[128];
			sprintf(string, "no Dynamixel found @ ID: %d", i);
			SendStatus(string, STATUS_TYPE_ERROR);
		}
		else {
			char string[128];
			sprintf(string, "Dynamixel found @ ID: %d", i);
			SendStatus(string, STATUS_TYPE_INFO);
			response++;
		}
	}

	if (response != 3) {
		// set system error state to prevent further operations
		//SetObjStructData(0xFF, SYS_STAT_DYNAMIXEL_ERROR);
		return;
	}
	else {

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

		// Only change if you really know what you are doing!
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

		// Only change if you really know what you are doing!
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

		// Only change if you really know what you are doing!
		Dynamixel.setCSlope(1, 32, 32);				// compilance slope - default value
		Dynamixel.setCMargin(1, 0, 0);				// compilance margin - default value
		Dynamixel.setPunch(1, 32);					// minimum current supplied to the motor - dafault value

		// enable torque for all servo motors
		Dynamixel.torqueStatus(0, ON);
		Dynamixel.torqueStatus(1, ON);
		Dynamixel.torqueStatus(2, ON);
	}
}