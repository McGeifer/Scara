// 
// 
// Setup for serial/ USB connection

#include "sio.h"
#include "objdir.h"
#include "Stream.h"
#include "DynamixelSerial2.h"

void InitSio() {

	// Setup for USB connection;
	Serial.begin(115200);
	Serial.setTimeout(500);

	// Setup for Modbus connection (Serial1)
	//modbus_configure(115200, 1, 0, TOTAL_REGS_SIZE_MODBUS, 0);

	// Setup for Dynamixel connection (Serial2)
	Dynamixel.begin(1000000, 2);
}