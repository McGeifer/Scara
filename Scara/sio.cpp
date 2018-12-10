// 
// 
// Setup for serial/ USB connection

#include "Stream.h"

#include "sio.h"
#include "objdir.h"
#include "gpio.h"
#include "status.h"

#include <DynamixelSerial2.h>
#include <SimpleModbusSlave.h>
#include <FastCRC.h>

// serial buffer
static const uint8_t bufferLength = 64;
static uint8_t buffer[bufferLength];
static char cBuffer[bufferLength];
static uint8_t ndx = 0;
static FastCRC8 CRC8;

void InitSio() {

	// Setup for USB connection;
	Serial.begin(115200);
	Serial.setTimeout(500);

	// Setup for Modbus connection (Serial1)
	modbus_configure(115200, 1, 0, TOTAL_REGS_SIZE_MDB, 0);

	// Setup for Dynamixel connection (Serial2)
	Dynamixel.begin(1000000, 2);
}

// Choose the correct handler to process the data received by serial connection (selected by operation mode).
void HandleSIO() {

	// get system error state - abort if system is in error state
	if (!(GetObjStructData(0xFF) & SYS_STAT_ERROR)) {

		switch (GetObjStructData(0xFE))	{

		case OP_MODE_MODBUS:
			HandleModbusData();
			//SendStatus("HandleSio(); ", "HandleModbusData() selected", STATUS_TYPE_DEBUG);
			break;

		case OP_MODE_RAPID:
			HandleRapidString();
			//SendStatus("HandleSio(); ", "HandleRapidString() selected", STATUS_TYPE_DEBUG);
			break;

		case OP_MODE_SCARA:
			HandleScaraData();
			//SendStatus("HandleSio(); ", "HandleScaraData() selected", STATUS_TYPE_DEBUG);
			break;

		default:
			// ???
			break;
		}
	}
}

uint8_t ParseRadpid() {

	enum tmpData {
		xPos,
		yPos,
		zPos,
		xSpeed,
		ySpeed,
		zSpeed,
	};

	char inputString[bufferLength];
	char* outputString[bufferLength];
	char* part;
	const char delimiter[] = " :=][(),_";
	int i = 0;
	int idxOffs = 0;
	bool offsetMode = false;
	bool pointMode = false;
	uint8_t pointIdx = 0xFF;
	uint16_t dataToWrite[sizeof(tmpData)];

	cBuffer[ndx + 1] = '\0'; // terminate string
	strcpy(inputString, cBuffer);	// creat copy of the receiveBuffer to prevent change of data by strtok
	part = strtok(inputString, delimiter);	// split the inputString into multiple tokens

	for (size_t i = 0; i < length; i++)
	{

	}

	while(part != NULL) {
		// create tokens
		outputString[i] = part;
		i++;
		part = strtok(NULL, delimiter);
	}

	// for example parse -> CONST robtarget p1:=[[1, 119, 100],,,];
	// outputString	index:	  0		 1	     2	  3	  4	   5
	if(strstr(outputString[0], "CONST")) {

		if(strstr(outputString[1], "robtarget")) {

			if(strchr(outputString[2], 'p')) {
				// get point number
				int16_t pointIdx = (int16_t)strtol(outputString[2] + 1, NULL, 10);

				// get offset values
				int16_t posXTmp = (int16_t)strtol(outputString[3], NULL, 10);
				int16_t posYTmp = (int16_t)strtol(outputString[4], NULL, 10);
				int16_t posZTmp = (int16_t)strtol(outputString[5], NULL, 10);

				// set rapidPosReg !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// set point idx
				return 0;
			}
			else {
				SendStatus("in function HandleRapidString(): ", "point index expected", STATUS_TYPE_ERROR);
				return 0;
			}
		}
		else {
			SendStatus("in function HandleRapidString(): ", "robtarget expected", STATUS_TYPE_ERROR);
			return 0;
		}
	}

	// switch the magnetic gripper on or off
	// for example parse -> Magnet_on();
	// outputstring index:	  0     1
	else if(strstr(outputString[0], "Magnet")) {

		if(strstr(outputString[1], "on")) {
			digitalWrite(MAGNET_PIN, HIGH);
			SendStatus("in function HandleRapidString(): ", "Magnet: on", STATUS_TYPE_INFO);
			return 0;
		}
		else if(strstr(outputString[1], "off")) {
			digitalWrite(MAGNET_PIN, LOW);
			SendStatus("in function HandleRapidString(): ", "Magnet: off", STATUS_TYPE_INFO);
			return 0;
		}
		else {
			SendStatus("in function HandleRapidString(): ", "Magnet_on / off expected", STATUS_TYPE_ERROR);
			return 0;
		}
	}

	// check for MoveC, MoveJ or MoveL
	// for example parse -> MoveJ Offs(p2,   -9,   109,   90), vmax, fine, tool0;
	// or				 -> MoveJ p1, vmax, fine, tool0;
	// or				 -> MoveJ (1,  119,  100), vmax, fine, tool0;
	// outputString	index:	  0		1   2	  3		4	   5	6	   7	8
	else if(strstr(outputString[0], "Move")) {
		if(strcmp(outputString[0], "MoveC") == 0) {
			SendStatus("in function HandleRapidString(): ", "MoveC not supported", STATUS_TYPE_ERROR);
			return 0;
		}
		else if(strcmp(outputString[0], "MoveL") == 0) {
			SendStatus("in function HandleRapidString(): ", "MoveL not supported", STATUS_TYPE_ERROR);
			return 0;
		}
		else if(strcmp(outputString[0], "MoveJ") == 0) {
			// setzte betriebsart ??? gibt ja momentan eh nur eine
		}
		else {
			SendStatus("in function HandleRapidString(): ", "No proper Move{C, L, J} comamnd detected", STATUS_TYPE_ERROR);
			return 0;
		}

		// check for Offs and/ or point
		if(strcmp(outputString[1], "Offs") == 0) {

			if(strchr(outputString[2], 'p')) {
				pointIdx = (int8_t)strtol(outputString[2] + 1, NULL, 10);

				if(GetPosRegData(&pointIdx) != NULL) {
					offsetMode = true;
					pointMode = true;
				}
				else {
					char msg[64];
					sprintf(msg, "point P%i does not exist", pointIdx);
					SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
					return 0;
				}
			}
			else {
				SendStatus("in function HandleRapidString(): ", "point definition expected for offset movement", STATUS_TYPE_ERROR);
				return 0;
			}
		}
		else if(strchr(outputString[1], 'p')) {
			pointIdx = (uint8_t)strtol(outputString[1] + 1, NULL, 10);

			if(GetPosRegData(&pointIdx) != NULL) {
				pointMode = true;
			}
			else {
				char msg[64];
				sprintf(msg, "point P%i does not exist", pointIdx);
				SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
				return 0;
			}
		}

		// set temporary position values (tool offset not included jet)
		if(!pointMode && !offsetMode) {
			dataToWrite[xPos] = (uint16_t)strtol(outputString[1], NULL, 10);
			dataToWrite[yPos] = (uint16_t)strtol(outputString[2], NULL, 10);
			dataToWrite[zPos] = (uint16_t)strtol(outputString[3], NULL, 10);
		}
		else if(pointMode && offsetMode) {
			uint16_t *pPosRegData = GetPosRegData(&pointIdx);

			if(pPosRegData != NULL) {
				dataToWrite[xPos] = pPosRegData[0] + (uint16_t)strtol(outputString[3], NULL, 10);
				dataToWrite[yPos] = pPosRegData[1] + (uint16_t)strtol(outputString[4], NULL, 10);
				dataToWrite[zPos] = pPosRegData[2] + (uint16_t)strtol(outputString[5], NULL, 10);
			}
			else {
				char msg[64];
				sprintf(msg, "point P%i does not exist", pointIdx);
				SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
				return 0;
			}
		}
		else if(pointMode && !offsetMode) {
			uint16_t *pPosRegData = GetPosRegData(&pointIdx);

			if(pPosRegData != NULL) {
				dataToWrite[xPos] = pPosRegData[0];
				dataToWrite[yPos] = pPosRegData[1];
				dataToWrite[zPos] = pPosRegData[2];
			}
			else {
				char msg[64];
				sprintf(msg, "point P%i does not exist", pointIdx);
				SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
				return 0;
			}
		}

		// set outputstring[] offset for speed, positioning mode & tool (based on "lowest" position of the speed value in the outputString[])
		//					 -> MoveJ Offs(p2,   -9,   109,   90), vmax, fine, tool0;
		//					 -> MoveJ p1, vmax, fine, tool0;		<- offset based on this type of message
		//					 -> MoveJ (1,  119,  100), vmax, fine, tool0;
		// outputString	index:	  0		1   2	  3		4	   5	6	   7	8
		if (pointMode && offsetMode) {
			idxOffs = 4;
		}
		else if (pointMode && !offsetMode) {
			idxOffs = 0;
		}
		else if (!pointMode && !offsetMode) {
			idxOffs = 2;
		}

		// check for speed value
		if(strstr(outputString[2 + idxOffs], "vmax")) {
			dataToWrite[xSpeed] = X_SPEED_MAX;
			dataToWrite[ySpeed] = Y_SPEED_MAX;
			dataToWrite[zSpeed] = Z_SPEED_MAX;
		}
		else if(strchr(outputString[2 + idxOffs], 'v')) {
			uint16_t speed = (uint16_t)strtol(outputString[2 + idxOffs] + 1, NULL, 0);

			if (speed >= X_SPEED_MIN && speed >= Y_SPEED_MIN && speed >= Z_SPEED_MIN) {
			
				if (speed <= X_SPEED_MAX && speed <= Y_SPEED_MAX && speed <= Z_SPEED_MAX) {
					dataToWrite[xSpeed] = speed;
					dataToWrite[ySpeed] = speed;
					dataToWrite[zSpeed] = speed;
				}
				else {
					SendStatus("in function HandleRapidString(): ", "speed value to high", STATUS_TYPE_ERROR);
					return 0;
				}
			}
			else {
				SendStatus("in function HandleRapidString(): ", "speed value to low", STATUS_TYPE_ERROR);
				return 0;
			}
		}
		else {
			SendStatus("in function HandleRapidString(): ", "speed value expected", STATUS_TYPE_ERROR);
			return 0;
		}

		// check for zone value
		if(strstr(outputString[3 + idxOffs], "fine")) {
			// mache weiter :)
		}
		else if(strchr(outputString[3 + idxOffs], 'z')) {
			SendStatus("in function HandleRapidString(): ", "fly-by point not supported (use fine)", STATUS_TYPE_ERROR);
			return 0;
		}
		else {
			SendStatus("in function HandleRapidString(): ", "zone value expected", STATUS_TYPE_ERROR);
			return 0;
		}

		// check for tool and add offset to position value
		if(strstr(outputString[4 + idxOffs], "tool")) {

			uint8_t tool = (uint8_t)strtol(outputString[4 + idxOffs] + 4, NULL, 0);
			int16_t *pToolTblOffset = GetToolTblData(tool);

			if(pToolTblOffset == NULL) {
				SendStatus("in function HandleRapidString(): ", "tool does not exist", STATUS_TYPE_ERROR);
				return 0;
			}

			dataToWrite[xPos] -= pToolTblOffset[0];
			dataToWrite[yPos] -= pToolTblOffset[1];
			dataToWrite[zPos] -= pToolTblOffset[2];

			if (dataToWrite[xPos] >= X_POS_MIN && dataToWrite[xPos] <= X_POS_MAX) {

				if (dataToWrite[yPos] >= Y_POS_MIN && dataToWrite[yPos] <= Y_POS_MAX) {

					if (dataToWrite[zPos] >= Z_POS_MIN && dataToWrite[zPos] <= Z_POS_MAX) {
						
						// save speed and position values
						if (SetObjStructData(0x60, dataToWrite[xSpeed]) != 0) {
							return 0;
						}
						if (SetObjStructData(0x70, dataToWrite[ySpeed]) != 0) {
							return 0;
						}
						if (SetObjStructData(0x80, dataToWrite[zSpeed]) != 0) {
							return 0;
						}
						if (SetObjStructData(0x10, dataToWrite[xPos]) != 0) {
							return 0;
						}
						if (SetObjStructData(0x20, dataToWrite[yPos]) != 0) {
							return 0;
						}
						if (SetObjStructData(0x30, dataToWrite[zPos]) != 0) {
							return 0;
						}
						return 1;
					}
					else {
						char msg[64];
						sprintf(msg, "z-position %imm not in allowed range", dataToWrite[zPos]);
						SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
						return 0;
					}
				}
				else {
					char msg[64];
					sprintf(msg, "y-position %imm not in allowed range", dataToWrite[yPos]);
					SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
					return 0;
				}
			}
			else {
				char msg[64];
				sprintf(msg, "x-position %imm not in allowed range", dataToWrite[xPos]);
				SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
				return 0;
			}
		}
		else {
			SendStatus("in function HandleRapidString(): ", "tool expected", STATUS_TYPE_ERROR);
			return 0;
		}
	}
	else {
		SendStatus("in function HandleRapidString(): ", "Move{C, L, J} command expected", STATUS_TYPE_ERROR);
		return 0;
	}
}

// Receive a Modbus data package via RS232, plausibility check and error response done by the SimpleModbusClient library.
static void HandleModbusData() {

	// function 3 and 16 register array
	static unsigned int holdingRegs[TOTAL_REGS_SIZE_MDB];

	// returns the total error count since the slave started
	holdingRegs[TOTAL_ERRORS_MDB] = modbus_update(holdingRegs);

	// write modbus packet to object
	if (SetObjStructData(holdingRegs[INDEX_MDB], holdingRegs[DATA_MDB]) == 0) {
		//objStruct_t *recvP = LocateObj(holdingRegs[INDEX_MDB]);
		//
		//if (recvP->pFunction != NULL) {
		//	// execute handler function if available
		//	recvP->pFunction(&recvP->idx, &recvP->props, &recvP->data);
		//}
	}
}

// Receive a Rapid comand string via UART, check the data for proper length and .
static void HandleRapidString() {

	if (Serial.available() > 0) {

		if (ndx <= bufferLength - 1) {
			cBuffer[ndx] = Serial.read();

			/* debug*/
			char msg[64];
			sprintf(msg, "cBuffer[%i]: %c", ndx, cBuffer[ndx]);
			SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);
			/* end debug*/

			if (cBuffer[ndx] == ';') {
				if (ParseRadpid()) {
					memset(cBuffer, 0, ndx);
					ndx = 0;
					return;
				}
			}
			ndx++;
		}
		else {
			memset(cBuffer, 0, ndx);
			ndx = 0;
			SendStatus("in function HandleRapidString(): buffer overflow - ", "input string to long", STATUS_TYPE_ERROR);
		}
	}
}



// Receive a Scara data package via UART, check the data package and store it in the object dictionary.
static void HandleScaraData() {

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
							/*objStruct_t *recvP = LocateObj(recvIndex);*/
							memset(buffer, 0, ndx);
							ndx = 0;

							//if (recvP->pFunction != NULL) {
							//	// execute handler function if available
							//	recvP->pFunction(&recvP->idx, &recvP->props, &recvP->data);
							//}
						}
					}
					else
						SendStatus("in function HandleScaraData(): ", "crc checksum invalid", STATUS_TYPE_WARNING);
				}
			}
		}
		else {
			memset(buffer, 0, ndx);
			ndx = 0;
			SendStatus("in function HandleScaraData(): ", "input data to long", STATUS_TYPE_ERROR);
		}
	}
}