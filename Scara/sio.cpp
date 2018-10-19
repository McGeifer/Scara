// 
// 
// Setup for serial/ USB connection

#include "Stream.h"

#include "sio.h"
#include "objdir.h"
#include "status.h"

#include "DynamixelSerial2.h"
#include "SimpleModbusSlave.h"
#include "FastCRC.h"

// serial buffer
static const uint8_t bufferLength = 64;
static uint8_t buffer[bufferLength];
static char cBuffer[bufferLength];
static uint8_t ndx;
FastCRC8 CRC8;


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
			Rapid();

		case OP_MODE_SCARA:
			Scara();

		default:
			// ???
			break;
		}
	}
}

// Receive a Modbus data package via RS232, plausibility check and error response done by the SimpleModbusClient library.
static void Modbus() {

	// function 3 and 16 register array
	static unsigned int holdingRegs[TOTAL_REGS_SIZE_MDB];

	// returns the total error count since the slave started
	holdingRegs[TOTAL_ERRORS_MDB] = modbus_update(holdingRegs);

	// write modbus packet to object
	if (SetObjStructData(holdingRegs[INDEX_MDB], holdingRegs[DATA_MDB]) == 0) {
		ObjStruct *recvP = LocateObj(holdingRegs[INDEX_MDB]);
		
		if (recvP->pFunction != NULL) {
			// execute handler function if available
			recvP->pFunction(&recvP->idx, &recvP->props, &recvP->data);
		}
	}
}

// Receive a Rapid comand string via UART, check the data for plausibility and store it in the object directory.
static void Rapid() {
	if (Serial.available() > 0) {

		if (ndx >= bufferLength - 1) {
			cBuffer[ndx] = Serial.read();
			ndx++;

			if (cBuffer[ndx] == ';') {
				
				// parse rapid string
				








			}
		}
		else {
			memset(cBuffer, 0, ndx);
			ndx = 0;
			SendStatus("in function Scara(): input string to long", STATUS_TYPE_ERROR);
		}
	}
}

// Receive a Scara data package via UART, check the data package and store it in the object dictionary.
static void Scara() {

	if (Serial.available() > 0) {

		if (ndx <= bufferLength) {
			buffer[ndx] = Serial.read();
			ndx++;

			if (ndx >= SCARA_PACKET_LENGTH) {

				for (uint8_t i = 0; i < (bufferLength - 3); i++) {

					// check crc for packet
					uint8_t crc = CRC8.smbus(&buffer[ndx - SCARA_PACKET_LENGTH], 3);

					if (crc == buffer[ndx - 1]) {
						uint8_t recvIndex = buffer[ndx - SCARA_PACKET_LENGTH + 1];
						uint16_t recvData = (buffer[ndx - SCARA_PACKET_LENGTH + 2] << 8) | buffer[ndx - SCARA_PACKET_LENGTH + 3];

						// save data in object
						if (SetObjStructData(recvIndex, recvData) == 0) {
							ObjStruct *recvP = LocateObj(recvIndex);
							memset(buffer, 0, ndx);
							ndx = 0;

							if (recvP->pFunction != NULL) {
								// execute handler function if available
								recvP->pFunction(&recvP->idx, &recvP->props, &recvP->data);
							}
						}
					}
					else
						SendStatus("in function Scara(): crc checksum invalid", STATUS_TYPE_WARNING);
				}
			}
		}
		else {
			memset(buffer, 0, ndx);
			ndx = 0;
			SendStatus("in function Scara(): input data to long", STATUS_TYPE_ERROR);
		}
	}
}