// 
// 
// Setup for serial/ USB connection

#include "sio.h"
#include "objdir.h"
#include "Stream.h"
#include "DynamixelSerial2.h"
#include "SimpleModbusSlave.h"

void InitSio() {

	// Setup for USB connection;
	Serial.begin(115200);
	Serial.setTimeout(500);

	// Setup for Modbus connection (Serial1)
	modbus_configure(115200, 1, 0, TOTAL_REGS_SIZE_MDB, 0);

	// Setup for Dynamixel connection (Serial2)
	Dynamixel.begin(1000000, 2);
}

// Choose the correct handler to process the received data (selected by operation mode).
void HandleSIO() {

	// get system error state - abort if system is in error state
	int16_t tmp = GetObjStructData(0xFF);

	if (tmp == 0) {

		switch (GetObjStructData(0xFE))	{

		case OP_MODE_MODBUS:
			Modbus();

		case OP_MODE_RAPID:
			//

		case OP_MODE_SCARA:
			//

		default:
			// ???
			break;
		}
	}
}

// Receive a Modbus data package via RS232, plausibility check and error response done by the SimpleModbusClient.
static void Modbus() {

	// function 3 and 16 register array
	static unsigned int holdingRegs[TOTAL_REGS_SIZE_MDB];

	// returns the total error count since the slave started
	holdingRegs[TOTAL_ERRORS_MDB] = modbus_update(holdingRegs);

	int8_t response = SetObjStructData(holdingRegs[INDEX_MDB], (holdingRegs[DATA_HI_MDB] << 8) | holdingRegs[DATA_LO_MDB]);

	if (response == -1) {
		// error handling !?
	}
}

// Receive a Rapid comand string via UART, check the data for plausibility and store it in the object directory.
static void Rapid() {


}

// Receive a Scara data package via UART, check the data for plausibility and store it in the object directory.
static void Scara() {

	uint8_t index;			// object index
	uint16_t data;			// data
	uint8_t data_hi;		// data high byte
	uint8_t data_lo;		// data low byte
	uint8_t crc;			// crc8 checksum
	uint8_t buffer[128];	// receive buffer





}