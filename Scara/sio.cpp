// 
// 
// Setup for serial/ USB connection

#include "Stream.h"

#include "sio.h"
#include "objdir.h"
#include "gpio.h"
#include "status.h"
#include "DynamixelSerial2/DynamixelSerial2.h"

#include <SimpleModbusSlave.h>
#include <FastCRC.h>

// serial buffer
const uint8_t	bufferLength = 128;
static uint8_t	buffer[bufferLength];
static char		cBuffer[bufferLength];
static uint8_t	idx = 0;
FastCRC8		CRC8;
const uint8_t	minBufferLength = 11; // minimum buffer length for a valid instruction ---> Magnet_An();

void InitSio(void)
{
	// Setup for USB connection;
	Serial.begin(1000000);
	Serial.setTimeout(500);

	// Setup for Modbus connection (Serial1)
	modbus_configure(115200, 1, 0, TOTAL_REGS_SIZE_MDB, 0);

	// Setup for Dynamixel connection (Serial2)
	dynamixelBegin(1000000, 2);
}

// Choose the correct handler to process the data received by the serial connection (selected by operation mode).
void HandleSIO(void)
{
	// get system error state - abort if system is in error state
	if (!(GetObjData(OBJ_IDX_OP_MODE) & SYS_STAT_ERROR))
	{
		switch (GetObjData(OBJ_IDX_OP_MODE))
		{
		case OP_MODE_MODBUS:
			HandleModbusData();
			break;

		case OP_MODE_RAPID:
			HandleRapidString();
			break;

		case OP_MODE_SCARA:
			HandleScaraData();
			break;

		default:
			// ???
			break;
		}
	}
}

int8_t ParseRadpid(void)
{
	enum tmpData
	{
		xPos,
		yPos,
		zPos,
		xSpeed,
		ySpeed,
		zSpeed,
	};

	char inputString[bufferLength];
	char *outputString[bufferLength];
	char *part;
	const char delimiter[] = " :=][(),_;";
	
	int i = 0;
	int idxOffs = 0;
	bool offsetMode = false;
	bool pointMode = false;
	uint8_t pointIdx = 0xFF;
	int16_t dataToWrite[sizeof(tmpData)];

	cBuffer[idx + 1] = '\0';	// terminate string
	// buffer overflow möglich!?

	strcpy(inputString, cBuffer);	// creat copy of the receiveBuffer to prevent change of data by strtok
	part = strtok(inputString, delimiter);	// split the inputString into multiple tokens

	while(part != NULL) /* create tokens */
	{ 
		outputString[i] = part;
		char msg[64];
		sprintf(msg, "token[%i]: %s", i, outputString[i]);
		SendStatus("ParseRapid(): ", msg, STATUS_TYPE_DEBUG);
		i++;
		part = strtok(NULL, delimiter);
	}
	
	// for example parse -> CONST robtarget p1:=[[1, 119, 100],,,];
	// outputString	index:    0      1      2     3   4    5
	if (strcmp(outputString[0], "CONST") == 0)
	{
		if (strcmp(outputString[1], "robtarget") == 0)
		{
			if (strchr(outputString[2], 'p'))
			{
				// get point number
				uint8_t pointIdx = (uint8_t)strtol(outputString[2] + 1, NULL, 10);

				// get offset values
				int16_t posXTmp = (int16_t)strtol(outputString[3], NULL, 10);
				int16_t posYTmp = (int16_t)strtol(outputString[4], NULL, 10);
				int16_t posZTmp = (int16_t)strtol(outputString[5], NULL, 10);

				if (SetPosRegData(&pointIdx, &posXTmp, &posYTmp, &posZTmp) == 0)
				{
					return 0;
				}
				else
				{
					SendStatus("in function HandleRapidString(): ", "failed to write point data", STATUS_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				SendStatus("in function HandleRapidString(): ", "point index expected", STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			SendStatus("in function HandleRapidString(): ", "robtarget expected", STATUS_TYPE_ERROR);
			return -1;
		}
	}

	// switch the magnetic gripper on or off
	// for example parse -> Magnet_on();
	// outputstring index:    0    1
	else if (strcmp(outputString[0], "Magnet") == 0)
	{
		if (strcmp(outputString[1], "on") == 0)
		{
			digitalWrite(MAGNET_PIN, HIGH);
			SendStatus("in function HandleRapidString(): ", "Magnet: on", STATUS_TYPE_INFO);
			return 0;
		}
		else if (strcmp(outputString[1], "off") == 0)
		{
			digitalWrite(MAGNET_PIN, LOW);
			SendStatus("in function HandleRapidString(): ", "Magnet: off", STATUS_TYPE_INFO);
			return 0;
		}
		else
		{
			SendStatus("in function HandleRapidString(): ", "Magnet_on / off expected", STATUS_TYPE_ERROR);
			return -1;
		}
	}

	// check for MoveC, MoveJ or MoveL
	// for example parse -> MoveJ Offs(p2,   -9,   109,   90), vmax, fine, tool0;
	// or				 -> MoveJ p1, vmax, fine, tool0;
	// or				 -> MoveJ (1,  119,  100), vmax, fine, tool0;
	// outputString	index:    0    1    2     3     4      5     6     7     8
	else if (strstr(outputString[0], "Move"))
	{
		if (strcmp(outputString[0], "MoveC") == 0)
		{
			SendStatus("in function HandleRapidString(): ", "MoveC not supported", STATUS_TYPE_ERROR);
			return -1;
		}
		else if (strcmp(outputString[0], "MoveL") == 0)
		{
			SendStatus("in function HandleRapidString(): ", "MoveL not supported", STATUS_TYPE_ERROR);
			return -1;
		}
		else if (strcmp(outputString[0], "MoveJ") == 0)
		{
			// setzte betriebsart ??? gibt ja momentan eh nur eine
		}
		else
		{
			SendStatus("in function HandleRapidString(): ", "No proper Move{C, L, J} comamnd detected", STATUS_TYPE_ERROR);
			return -1;
		}

		// check for Offs and/ or point
		if (strcmp(outputString[1], "Offs") == 0)
		{
			if (strchr(outputString[2], 'p'))
			{
				pointIdx = (int8_t)strtol(outputString[2] + 1, NULL, 10);

				if (GetPosRegData(&pointIdx) != NULL)
				{
					offsetMode = true;
					pointMode = true;
				}
				else
				{
					char msg[64];
					sprintf(msg, "point P%i does not exist", pointIdx);
					SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				SendStatus("in function HandleRapidString(): ", "point definition expected for offset movement", STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else if (strchr(outputString[1], 'p'))
		{
			pointIdx = (uint8_t)strtol(outputString[1] + 1, NULL, 10);

			if (GetPosRegData(&pointIdx) != NULL)
			{
				pointMode = true;
			}
			else
			{
				char msg[64];
				sprintf(msg, "point P%i does not exist", pointIdx);
				SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
				return -1;
			}
		}

		// set temporary position values (tool offset not included jet)
		if (!pointMode && !offsetMode)
		{
			dataToWrite[xPos] = (int16_t)strtol(outputString[1], NULL, 10);
			dataToWrite[yPos] = (int16_t)strtol(outputString[2], NULL, 10);
			dataToWrite[zPos] = (int16_t)strtol(outputString[3], NULL, 10);
			char msg[64];
			sprintf(msg, "normal mode - temporary position values x: %i, y: %i, z: %i", dataToWrite[xPos], dataToWrite[yPos], dataToWrite[zPos]);
			SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);
		}
		else if (pointMode && offsetMode)
		{
			int16_t *pPosRegData = GetPosRegData(&pointIdx);
			char msg[64];
			sprintf(msg, "posRegData[] x: %i, y: %i, z: %i", pPosRegData[0], pPosRegData[1], pPosRegData[2]);
			SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);
			
			if (pPosRegData != NULL)
			{
				dataToWrite[xPos] = pPosRegData[0] + (int16_t)strtol(outputString[3], NULL, 10);
				dataToWrite[yPos] = pPosRegData[1] + (int16_t)strtol(outputString[4], NULL, 10);
				dataToWrite[zPos] = pPosRegData[2] + (int16_t)strtol(outputString[5], NULL, 10);
				char msg[64];
				sprintf(msg, "point & offset mode - temporary position values[] x: %i, y: %i, z: %i", dataToWrite[xPos], dataToWrite[yPos], dataToWrite[zPos]);
				SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);
			}
			else
			{
				char msg[32];
				sprintf(msg, "point P%i does not exist", pointIdx);
				SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else if (pointMode && !offsetMode)
		{
			int16_t *pPosRegData = GetPosRegData(&pointIdx);

			if (pPosRegData != NULL)
			{
				dataToWrite[xPos] = pPosRegData[0];
				dataToWrite[yPos] = pPosRegData[1];
				dataToWrite[zPos] = pPosRegData[2];
				char msg[64];
				sprintf(msg, "point mode - temporary position values[] x: %i, y: %i, z: %i", dataToWrite[xPos], dataToWrite[yPos], dataToWrite[zPos]);
				SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);
			}
			else
			{
				char msg[32];
				sprintf(msg, "point P%i does not exist", pointIdx);
				SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
				return -1;
			}
		}

		// set outputstring[] offset for speed, positioning mode & tool (based on "lowest" position of the speed value in the outputString[])
		//					 -> MoveJ Offs(p2,   -9,   109,   90), vmax, fine, tool0;
		//					 -> MoveJ p1, vmax, fine, tool0;  <------ offset based on this type of message
		//					 -> MoveJ (1,  119,  100), vmax, fine, tool0;
		// outputString	index:    0    1    2     3     4     5      6     7     8
		if (pointMode && offsetMode)
		{
			idxOffs = 4;
		}
		else if (pointMode && !offsetMode)
		{
			idxOffs = 0;
		}
		else if (!pointMode && !offsetMode)
		{
			idxOffs = 2;
		}

		// check for speed value
		if (strcmp(outputString[2 + idxOffs], "vmax") == 0)
		{
			dataToWrite[xSpeed] = X_SPEED_MAX;
			dataToWrite[ySpeed] = Y_SPEED_MAX;
			dataToWrite[zSpeed] = Z_SPEED_MAX;
			char msg[64];
			sprintf(msg, "final speed values[] x: %i, y: %i, z: %i", dataToWrite[xSpeed], dataToWrite[ySpeed], dataToWrite[zSpeed]);
			SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);
		}
		else if (strchr(outputString[2 + idxOffs], 'v'))
		{
			int16_t speed = (int16_t)strtol(outputString[2 + idxOffs] + 1, NULL, 0);

			if (speed >= X_SPEED_MIN && speed >= Y_SPEED_MIN && speed >= Z_SPEED_MIN)
			{
				if (speed <= X_SPEED_MAX && speed <= Y_SPEED_MAX && speed <= Z_SPEED_MAX)
				{
					dataToWrite[xSpeed] = speed;
					dataToWrite[ySpeed] = speed;
					dataToWrite[zSpeed] = speed;
					char msg[64];
					sprintf(msg, "final speed values[] x: %i, y: %i, z: %i", dataToWrite[xSpeed], dataToWrite[ySpeed], dataToWrite[zSpeed]);
					SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);
				}
				else
				{
					SendStatus("in function HandleRapidString(): ", "speed value to high", STATUS_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				SendStatus("in function HandleRapidString(): ", "speed value to low", STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			SendStatus("in function HandleRapidString(): ", "speed value expected", STATUS_TYPE_ERROR);
			return -1;
		}

		// check for zone value
		if (strcmp(outputString[3 + idxOffs], "fine") == 0)
		{
			// mache weiter :)
		}
		else if (strchr(outputString[3 + idxOffs], 'z'))
		{
			SendStatus("in function HandleRapidString(): ", "fly-by point not supported (use fine)", STATUS_TYPE_ERROR);
			return -1;
		}
		else
		{
			SendStatus("in function HandleRapidString(): ", "zone value expected", STATUS_TYPE_ERROR);
			return -1;
		}

		// check for tool & tool status, if all is good add offset to temporary position value
		if (strstr(outputString[4 + idxOffs], "tool"))
		{
			uint8_t tool = (uint8_t)strtol(outputString[4 + idxOffs] + 4, NULL, 10);
			toolTbl_t *pToolTbl = NULL;
			pToolTbl = LocateTool(tool);

			if (pToolTbl != NULL)
			{
				if (pToolTbl->active != false)
				{
					int16_t *pToolOffset = GetToolData(tool);
					dataToWrite[xPos] -= pToolOffset[0];
					dataToWrite[yPos] -= pToolOffset[1];
					dataToWrite[zPos] -= pToolOffset[2];
					char msg[64];
					sprintf(msg, "final position values[] x: %i, y: %i, z: %i", dataToWrite[xPos], dataToWrite[yPos], dataToWrite[zPos]);
					SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);

					if (dataToWrite[xPos] >= X_POS_MIN && dataToWrite[xPos] <= X_POS_MAX)
					{
						if (dataToWrite[yPos] >= Y_POS_MIN && dataToWrite[yPos] <= Y_POS_MAX)
						{
							if (dataToWrite[zPos] >= Z_POS_MIN && dataToWrite[zPos] <= Z_POS_MAX)
							{
								// save speed and position values
								if (SetObjData(OBJ_IDX_X_NEW_TARGET_POS, dataToWrite[xPos], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_Y_NEW_TARGET_POS, dataToWrite[yPos], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_Z_NEW_TARGET_POS, dataToWrite[zPos], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_X_NEW_TARGET_SPEED, dataToWrite[xSpeed], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_Y_NEW_TARGET_SPEED, dataToWrite[ySpeed], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_Z_NEW_TARGET_SPEED, dataToWrite[zSpeed], false) != 0)
								{
									return -1;
								}
								// "starte Bewegung" nach erfolgreichem Schreiben
								return 0;
							}
							else
							{
								char msg[32];
								sprintf(msg, "z-position %i mm out of range", dataToWrite[zPos]);
								SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
								return -1;
							}
						}
						else
						{
							char msg[32];
							sprintf(msg, "y-position %i mm out of range", dataToWrite[yPos]);
							SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
							return -1;
						}
					}
					else
					{
						char msg[32];
						sprintf(msg, "x-position %i mm out of range", dataToWrite[xPos]);
						SendStatus("in function HandleRapidString(): ", msg, STATUS_TYPE_ERROR);
						return -1;
					}
				}
				else
				{
					SendStatus("in function HandleRapidString(): ", "selected tool is inactive", STATUS_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				SendStatus("in function HandleRapidString(): ", "tool does not exist", STATUS_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			SendStatus("in function HandleRapidString(): ", "tool expected", STATUS_TYPE_ERROR);
			return -1;
		}
	}
	else
	{
		SendStatus("in function HandleRapidString(): ", "Move{C, L, J} command expected", STATUS_TYPE_ERROR);
		return -1;
	}
}

// Receive a Modbus data package via RS232, plausibility check and error response done by the SimpleModbusClient.
void HandleModbusData(void)
{
	// function 3 and 16 register array
	static uint16_t holdingRegs[TOTAL_REGS_SIZE_MDB];

	// returns the total error count since the slave started
	holdingRegs[TOTAL_ERRORS_MDB] = modbus_update(holdingRegs);

	// write modbus packet to object
	if (SetObjData(holdingRegs[INDEX_MDB], holdingRegs[DATA_MDB], false) == 0)
	{
		//objStruct_t *recvP = LocateObj(holdingRegs[INDEX_MDB]);
		//
		//if (recvP->pFunction != NULL) {
		//	// execute handler function if available
		//	recvP->pFunction(&recvP->idx, &recvP->props, &recvP->data);
		//}
	}
	else
	{
		// error msg
	}
}

// Receive a Rapid comand string via UART, check the data for proper length and content.
void HandleRapidString(void)
{
	if (Serial.available() > 0)
	{
		if (idx < bufferLength)
		{
			cBuffer[idx] = Serial.read();

			/* debug*/
			/*char msg[64];
			sprintf(msg, "cBuffer[%i]: %c", idx, cBuffer[idx]);
			SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);*/
			/* end debug*/

			if (cBuffer[idx] == ';')
			{
				if (idx > minBufferLength)
				{
					int tmp = ParseRadpid();
					char msg[32];
					sprintf(msg, "ParseRadpid() return: %i", tmp);
					SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);

					/*if (tmp == 0) {
						char msg[64];
						sprintf(msg, "memset ""0"" -> idx: %i", idx);
						SendStatus("HandleRapidString(): ", msg, STATUS_TYPE_DEBUG);
					}*/
				}
				else
				{
					SendStatus("HandleRapidString(): ", "instruction to short", STATUS_TYPE_ERROR);
				}
				memset(cBuffer, 0, idx);
				idx = 0;
				return;
			}
			idx++;
		}
		else
		{
			SendStatus("in function HandleRapidString(): buffer overflow - ", "input string to long", STATUS_TYPE_ERROR);
			memset(cBuffer, 0, idx);
			idx = 0;
		}
	}
}

// Receive a Scara data package via UART, check the data package and store it in the object dir.
void HandleScaraData(void)
{
	if (Serial.available() > 0)
	{
		if (idx <= bufferLength)
		{
			buffer[idx] = Serial.read();
			idx++;

			if (idx >= SCARA_PACKET_LENGTH)
			{
				for (uint8_t i = 0; i < (bufferLength - 3); i++) // bufferlength ??????? eher idx oder?
				{
					// check crc for packet
					uint8_t crc = CRC8.smbus(&buffer[idx - SCARA_PACKET_LENGTH], 3);

					if (crc == buffer[idx - 1])
					{
						uint8_t recvIndex = buffer[idx - SCARA_PACKET_LENGTH + 1];
						uint16_t recvData = (buffer[idx - SCARA_PACKET_LENGTH + 2] << 8) | buffer[idx - SCARA_PACKET_LENGTH + 3];

						// save data in object
						if (SetObjData(recvIndex, recvData, false) == 0)
						{
							/*objStruct_t *recvP = LocateObj(recvIndex);*/
							memset(buffer, 0, idx);
							idx = 0;

							//if (recvP->pFunction != NULL) {
							//	// execute handler function if available
							//	recvP->pFunction(&recvP->idx, &recvP->props, &recvP->data);
							//}
						}
						else
						{
							// fehler
						}
					}
					else
					{
						SendStatus("in function HandleScaraData(): ", "invalid crc checksum", STATUS_TYPE_WARNING);
					}
				}
			}
		}
		else
		{
			SendStatus("in function HandleScaraData(): ", "input data to long", STATUS_TYPE_ERROR);
			memset(buffer, 0, idx);
			idx = 0;
		}
	}
}