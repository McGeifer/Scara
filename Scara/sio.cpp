// 
// 
// Setup for serial/ USB connection

#include "Stream.h"

#include "sio.h"
#include "objdir.h"
#include "gpio.h"
#include "status.h"
#include "dynamixel.h"

#include <SimpleModbusSlave.h>
#include "FastCRC.h"

#define SCARA_PACKET_LENGTH 4

// serial buffer
const uint8_t	buffer_length = 128;
static uint8_t	buffer[buffer_length];
static char		c_buffer[buffer_length];
static uint8_t	idx = 0;
FastCRC8		crc8;
const uint8_t	min_buffer_length = 11; // minimum buffer length for a valid instruction ---> Magnet_An();

void initSio(void)
{
	// Setup for USB connection;
	Serial.begin(1000000);
	Serial.setTimeout(500);

	// Setup for Modbus connection (Serial1)
	modbus_configure(115200, 1, 0, TOTAL_REGS_SIZE_MDB, 0);

	// Setup for Dynamixel connection (Serial2)
	dxlStartCom(1000000, DXL_DIRECTION_PIN);
}

// Choose the correct handler to process the data received by the serial connection (selected by operation mode).
void handleSIO(void)
{
	// get system error state - abort if system is in error state
	if (!(GetObjData(OBJ_IDX_OP_MODE) & SYS_STAT_ERROR))
	{
		switch (GetObjData(OBJ_IDX_OP_MODE))
		{
		case OP_MODE_MODBUS:
			handleModbusData();
			break;

		case OP_MODE_RAPID:
			handleRapidString();
			break;

		case OP_MODE_SCARA:
			handleScaraData();
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

	char input_string[buffer_length];
	char *output_string[buffer_length];
	char *part;
	const char delimiter[] = " :=][(),_;";
	
	int i = 0;
	int idx_offs = 0;
	bool offset_mode = false;
	bool point_mode = false;
	uint8_t point_idx = 0xFF;
	int16_t data_to_write[sizeof(tmpData)];

	c_buffer[idx + 1] = '\0';	// terminate string
	// buffer overflow möglich!?

	strcpy(input_string, c_buffer);				/* create copy of the receiveBuffer to prevent change of data by strtok */
	part = strtok(input_string, delimiter);		/* split the input_string into multiple tokens */

	while(part != NULL) /* create tokens */
	{ 
		output_string[i] = part;
		char msg[64];
		sprintf(msg, "token[%i]: %s", i, output_string[i]);
		SendStatus("ParseRapid(): ", msg, STATUS_MSG_TYPE_DEBUG);
		i++;
		part = strtok(NULL, delimiter);
	}
	
	// for example parse -> CONST robtarget p1:=[[1, 119, 100],,,];
	// output_string	index:    0      1      2     3   4    5
	if (strcmp(output_string[0], "CONST") == 0)
	{
		if (strcmp(output_string[1], "robtarget") == 0)
		{
			if (strchr(output_string[2], 'p') != NULL)
			{
				// get point number
				uint8_t point_idx = (uint8_t)strtol(output_string[2] + 1, NULL, 10);

				// get offset values
				int16_t pos_x_tmp = (int16_t)strtol(output_string[3], NULL, 10);
				int16_t pos_y_tmp = (int16_t)strtol(output_string[4], NULL, 10);
				int16_t pos_z_tmp = (int16_t)strtol(output_string[5], NULL, 10);

				if (SetPosRegData(&point_idx, &pos_x_tmp, &pos_y_tmp, &pos_z_tmp) == 0)
				{
					return 0;
				}
				else
				{
					SendStatus("in function handleRapidString(): ", "failed to write point data", STATUS_MSG_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				SendStatus("in function handleRapidString(): ", "point index expected", STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			SendStatus("in function handleRapidString(): ", "robtarget expected", STATUS_MSG_TYPE_ERROR);
			return -1;
		}
	}

	// switch the magnetic gripper on or off
	// for example parse -> Magnet_on();
	// outputstring index:    0    1
	else if (strcmp(output_string[0], "Magnet") == 0)
	{
		if (strcmp(output_string[1], "on") == 0)
		{
			digitalWrite(GPIO_PIN_MAGNET, HIGH);
			SendStatus("in function handleRapidString(): ", "Magnet: on", STATUS_MSG_TYPE_INFO);
			return 0;
		}
		else if (strcmp(output_string[1], "off") == 0)
		{
			digitalWrite(GPIO_PIN_MAGNET, LOW);
			SendStatus("in function handleRapidString(): ", "Magnet: off", STATUS_MSG_TYPE_INFO);
			return 0;
		}
		else
		{
			SendStatus("in function handleRapidString(): ", "Magnet_on / off expected", STATUS_MSG_TYPE_ERROR);
			return -1;
		}
	}

	// check for MoveC, MoveJ or MoveL
	// for example parse -> MoveJ Offs(p2,   -9,   109,   90), vmax, fine, tool0;
	// or				 -> MoveJ p1, vmax, fine, tool0;
	// or				 -> MoveJ (1,  119,  100), vmax, fine, tool0;
	// output_string	index:    0    1    2     3     4      5     6     7     8
	else if (strstr(output_string[0], "Move") != NULL)
	{
		if (strcmp(output_string[0], "MoveC") == 0)
		{
			SendStatus("in function handleRapidString(): ", "MoveC not supported", STATUS_MSG_TYPE_ERROR);
			return -1;
		}
		else if (strcmp(output_string[0], "MoveL") == 0)
		{
			SendStatus("in function handleRapidString(): ", "MoveL not supported", STATUS_MSG_TYPE_ERROR);
			return -1;
		}
		else if (strcmp(output_string[0], "MoveJ") == 0)
		{
			// setzte betriebsart ??? gibt ja momentan eh nur eine
		}
		else
		{
			SendStatus("in function handleRapidString(): ", "No proper Move{C, L, J} command detected", STATUS_MSG_TYPE_ERROR);
			return -1;
		}

		// check for Offs and/ or point
		if (strcmp(output_string[1], "Offs") == 0)
		{
			if (strchr(output_string[2], 'p'))
			{
				point_idx = (int8_t)strtol(output_string[2] + 1, NULL, 10);

				if (GetPosRegData(&point_idx) != NULL)
				{
					offset_mode = true;
					point_mode = true;
				}
				else
				{
					char msg[64];
					sprintf(msg, "point P%i does not exist", point_idx);
					SendStatus("in function handleRapidString(): ", msg, STATUS_MSG_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				SendStatus("in function handleRapidString(): ", "point definition expected for offset movement", STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else if (strchr(output_string[1], 'p') != NULL)
		{
			point_idx = (uint8_t)strtol(output_string[1] + 1, NULL, 10);

			if (GetPosRegData(&point_idx) != NULL)
			{
				point_mode = true;
			}
			else
			{
				char msg[64];
				sprintf(msg, "point P%i does not exist", point_idx);
				SendStatus("in function handleRapidString(): ", msg, STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}

		// set temporary position values (tool offset not included jet)
		if (!point_mode && !offset_mode)
		{
			data_to_write[xPos] = (int16_t)strtol(output_string[1], NULL, 10);
			data_to_write[yPos] = (int16_t)strtol(output_string[2], NULL, 10);
			data_to_write[zPos] = (int16_t)strtol(output_string[3], NULL, 10);
			char msg[64];
			sprintf(msg, "normal mode - temporary position values x: %i, y: %i, z: %i", data_to_write[xPos], data_to_write[yPos], data_to_write[zPos]);
			SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);
		}
		else if (point_mode && offset_mode)
		{
			int16_t *pPosRegData = GetPosRegData(&point_idx);
			char msg[64];
			sprintf(msg, "posRegData[] x: %i, y: %i, z: %i", pPosRegData[0], pPosRegData[1], pPosRegData[2]);
			SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);
			
			if (pPosRegData != NULL)
			{
				data_to_write[xPos] = pPosRegData[0] + (int16_t)strtol(output_string[3], NULL, 10);
				data_to_write[yPos] = pPosRegData[1] + (int16_t)strtol(output_string[4], NULL, 10);
				data_to_write[zPos] = pPosRegData[2] + (int16_t)strtol(output_string[5], NULL, 10);
				char msg[64];
				sprintf(msg, "point & offset mode - temporary position values[] x: %i, y: %i, z: %i", data_to_write[xPos], data_to_write[yPos], data_to_write[zPos]);
				SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);
			}
			else
			{
				char msg[32];
				sprintf(msg, "point P%i does not exist", point_idx);
				SendStatus("in function handleRapidString(): ", msg, STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else if (point_mode && !offset_mode)
		{
			int16_t *p_pos_reg_data = GetPosRegData(&point_idx);

			if (p_pos_reg_data != NULL)
			{
				data_to_write[xPos] = p_pos_reg_data[0];
				data_to_write[yPos] = p_pos_reg_data[1];
				data_to_write[zPos] = p_pos_reg_data[2];
				char msg[64];
				sprintf(msg, "point mode - temporary position values[] x: %i, y: %i, z: %i", data_to_write[xPos], data_to_write[yPos], data_to_write[zPos]);
				SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);
			}
			else
			{
				char msg[32];
				sprintf(msg, "point P%i does not exist", point_idx);
				SendStatus("in function handleRapidString(): ", msg, STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}

		// set outputstring[] offset for speed, positioning mode & tool (based on "lowest" position of the speed value in the output_string[])
		//					 -> MoveJ Offs(p2,   -9,   109,   90), vmax, fine, tool0;
		//					 -> MoveJ p1, vmax, fine, tool0;  <------ offset based on this type of message
		//					 -> MoveJ (1,  119,  100), vmax, fine, tool0;
		// output_string	index:    0    1    2     3     4     5      6     7     8
		if (point_mode && offset_mode)
		{
			idx_offs = 4;
		}
		else if (point_mode && !offset_mode)
		{
			idx_offs = 0;
		}
		else if (!point_mode && !offset_mode)
		{
			idx_offs = 2;
		}

		// check for speed value
		if (strcmp(output_string[2 + idx_offs], "vmax") == 0)
		{
			data_to_write[xSpeed] = X_SPEED_MAX;
			data_to_write[ySpeed] = Y_SPEED_MAX;
			data_to_write[zSpeed] = Z_SPEED_MAX;
			char msg[64];
			sprintf(msg, "final speed values[] x: %i, y: %i, z: %i", data_to_write[xSpeed], data_to_write[ySpeed], data_to_write[zSpeed]);
			SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);
		}
		else if (strchr(output_string[2 + idx_offs], 'v') != NULL)
		{
			int16_t speed = (int16_t)strtol(output_string[2 + idx_offs] + 1, NULL, 0);

			if (speed >= X_SPEED_MIN && speed >= Y_SPEED_MIN && speed >= Z_SPEED_MIN)
			{
				if (speed <= X_SPEED_MAX && speed <= Y_SPEED_MAX && speed <= Z_SPEED_MAX)
				{
					data_to_write[xSpeed] = speed;
					data_to_write[ySpeed] = speed;
					data_to_write[zSpeed] = speed;
					char msg[64];
					sprintf(msg, "final speed values[] x: %i, y: %i, z: %i", data_to_write[xSpeed], data_to_write[ySpeed], data_to_write[zSpeed]);
					SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);
				}
				else
				{
					SendStatus("in function handleRapidString(): ", "speed value to high", STATUS_MSG_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				SendStatus("in function handleRapidString(): ", "speed value to low", STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			SendStatus("in function handleRapidString(): ", "speed value expected", STATUS_MSG_TYPE_ERROR);
			return -1;
		}

		// check for zone value
		if (strcmp(output_string[3 + idx_offs], "fine") == 0)
		{
			// mache weiter :)
		}
		else if (strchr(output_string[3 + idx_offs], 'z') != NULL)
		{
			SendStatus("in function handleRapidString(): ", "fly-by point not supported (use fine)", STATUS_MSG_TYPE_ERROR);
			return -1;
		}
		else
		{
			SendStatus("in function handleRapidString(): ", "zone value expected", STATUS_MSG_TYPE_ERROR);
			return -1;
		}

		// check for tool & tool status, if all is good add offset to temporary position value
		if (strstr(output_string[4 + idx_offs], "tool") != NULL)
		{
			uint8_t tool = (uint8_t)strtol(output_string[4 + idx_offs] + 4, NULL, 10);
			toolTbl_t *pToolTbl = NULL;
			pToolTbl = LocateTool(tool);

			if (pToolTbl != NULL)
			{
				if (pToolTbl->active != false)
				{
					int16_t *pToolOffset = GetToolData(tool);
					data_to_write[xPos] -= pToolOffset[0];
					data_to_write[yPos] -= pToolOffset[1];
					data_to_write[zPos] -= pToolOffset[2];
					char msg[64];
					sprintf(msg, "final position values[] x: %i, y: %i, z: %i", data_to_write[xPos], data_to_write[yPos], data_to_write[zPos]);
					SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);

					if (data_to_write[xPos] >= OBJ_POS_X_MIN && data_to_write[xPos] <= OBJ_POS_X_MAX)
					{
						if (data_to_write[yPos] >= OBJ_POS_Y_MIN && data_to_write[yPos] <= OBJ_POS_Y_MAX)
						{
							if (data_to_write[zPos] >= OBJ_POS_Z_MIN && data_to_write[zPos] <= OBJ_POS_Z_MAX)
							{
								// save speed and position values
								if (SetObjData(OBJ_IDX_X_NEW_TARGET_POS, data_to_write[xPos], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_Y_NEW_TARGET_POS, data_to_write[yPos], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_Z_NEW_TARGET_POS, data_to_write[zPos], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_X_NEW_TARGET_SPEED, data_to_write[xSpeed], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_Y_NEW_TARGET_SPEED, data_to_write[ySpeed], false) != 0)
								{
									return -1;
								}
								if (SetObjData(OBJ_IDX_Z_NEW_TARGET_SPEED, data_to_write[zSpeed], false) != 0)
								{
									return -1;
								}
								// "starte Bewegung" nach erfolgreichem Schreiben
								return 0;
							}
							else
							{
								char msg[32];
								sprintf(msg, "z-position %i mm out of range", data_to_write[zPos]);
								SendStatus("in function handleRapidString(): ", msg, STATUS_MSG_TYPE_ERROR);
								return -1;
							}
						}
						else
						{
							char msg[32];
							sprintf(msg, "y-position %i mm out of range", data_to_write[yPos]);
							SendStatus("in function handleRapidString(): ", msg, STATUS_MSG_TYPE_ERROR);
							return -1;
						}
					}
					else
					{
						char msg[32];
						sprintf(msg, "x-position %i mm out of range", data_to_write[xPos]);
						SendStatus("in function handleRapidString(): ", msg, STATUS_MSG_TYPE_ERROR);
						return -1;
					}
				}
				else
				{
					SendStatus("in function handleRapidString(): ", "selected tool is inactive", STATUS_MSG_TYPE_ERROR);
					return -1;
				}
			}
			else
			{
				SendStatus("in function handleRapidString(): ", "tool does not exist", STATUS_MSG_TYPE_ERROR);
				return -1;
			}
		}
		else
		{
			SendStatus("in function handleRapidString(): ", "tool expected", STATUS_MSG_TYPE_ERROR);
			return -1;
		}
	}
	else
	{
		SendStatus("in function handleRapidString(): ", "Move{C, L, J} command expected", STATUS_MSG_TYPE_ERROR);
		return -1;
	}
}

// Receive a Modbus data package via RS232, plausibility check and error response done by the SimpleModbusClient.
void handleModbusData(void)
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

// Receive a Rapid command string via UART, check the data for proper length and content.
void handleRapidString(void)
{
	if (Serial.available() > 0)
	{
		if (idx < buffer_length)
		{
			c_buffer[idx] = Serial.read();

			/* debug*/
			/*char msg[64];
			sprintf(msg, "c_buffer[%i]: %c", idx, c_buffer[idx]);
			SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);*/
			/* end debug*/

			if (c_buffer[idx] == ';')
			{
				if (idx > min_buffer_length)
				{
					int tmp = ParseRadpid();
					char msg[32];
					sprintf(msg, "ParseRadpid() return: %i", tmp);
					SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);

					/*if (tmp == 0) {
						char msg[64];
						sprintf(msg, "memset ""0"" -> idx: %i", idx);
						SendStatus("handleRapidString(): ", msg, STATUS_MSG_TYPE_DEBUG);
					}*/
				}
				else
				{
					SendStatus("handleRapidString(): ", "instruction to short", STATUS_MSG_TYPE_ERROR);
				}
				memset(c_buffer, 0, idx);
				idx = 0;
				return;
			}
			idx++;
		}
		else
		{
			SendStatus("in function handleRapidString(): buffer overflow - ", "input string to long", STATUS_MSG_TYPE_ERROR);
			memset(c_buffer, 0, idx);
			idx = 0;
		}
	}
}

// Receive a Scara data package via UART, check the data package and store it in the object dir.
void handleScaraData(void)
{
	if (Serial.available() > 0)
	{
		if (idx <= buffer_length)
		{
			buffer[idx] = Serial.read();
			idx++;

			if (idx >= SCARA_PACKET_LENGTH)
			{
				for (uint8_t i = 0; i < (buffer_length - 3); i++) // bufferlength ??????? eher idx oder?
				{
					// check crc for packet
					uint8_t crc = crc8.smbus(&buffer[idx - SCARA_PACKET_LENGTH], 3);

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
						SendStatus("in function handleScaraData(): ", "invalid crc checksum", STATUS_MSG_TYPE_WARNING);
					}
				}
			}
		}
		else
		{
			SendStatus("in function handleScaraData(): ", "input data to long", STATUS_MSG_TYPE_ERROR);
			memset(buffer, 0, idx);
			idx = 0;
		}
	}
}