/*  */

#include "dynamixel.h"
#include "objdir.h"
#include "status.h"
#include "calc.h"
#include "gpio.h"
#include "Stream.h"

/* 
	Initialization of the parameter list for setting up the Dynamixel
	servos when starting the controller
 */
static uint8_t dxl_setup_params[3][9] = {
	{	DXL_AXIS_1_TEMP_LIMIT,
		DXL_AXIS_1_VOLTAGE_LIMIT_LOW,
		DXL_AXIS_1_VOLTAGE_LIMIT_HIGH,
		(uint8_t)DXL_AXIS_1_MAX_TORQUE,			/* low byte */
		(uint8_t)DXL_AXIS_1_MAX_TORQUE >> 8,	/* high byte */
		DXL_AXIS_1_STATUS_RETURN_LEVEL,
		DXL_AXIS_1_RETURN_DELAY_TIME,
		DXL_AXIS_1_ALARM_LED,
		DXL_AXIS_1_ALARM_SHUT_DOWN
	},
	{	DXL_Z_AXIS_TEMP_LIMIT,
		DXL_AXIS_2_VOLTAGE_LIMIT_LOW,
		DXL_AXIS_2_VOLTAGE_LIMIT_HIGH,
		(uint8_t)DXL_Z_AXIS_MAX_TORQUE,			/* low byte */
		(uint8_t)DXL_Z_AXIS_MAX_TORQUE >> 8,	/* high byte */
		DXL_Z_AXIS_STATUS_RETURN_LEVEL,
		DXL_Z_AXIS_RETURN_DELAY_TIME,
		DXL_Z_AXIS_ALARM_LED,
		DXL_Z_AXIS_ALARM_SHUT_DOWN
	},
	{	DXL_Z_AXIS_TEMP_LIMIT,
		DXL_Z_AXIS_VOLTAGE_LIMIT_LOW,
		DXL_Z_AXIS_VOLTAGE_LIMIT_HIGH,
		(uint8_t)DXL_Z_AXIS_MAX_TORQUE,			/* low byte */
		(uint8_t)DXL_Z_AXIS_MAX_TORQUE >> 8,	/* high byte */
		DXL_Z_AXIS_STATUS_RETURN_LEVEL,
		DXL_Z_AXIS_RETURN_DELAY_TIME,
		DXL_Z_AXIS_ALARM_LED,
		DXL_Z_AXIS_ALARM_SHUT_DOWN
	}
};

static dxlStatusPacket_t dxl_status_packet = { 0 };				/* the current Dynamixel status packet */
static const dxlStatusPacket_t dxl_status_packet_empty = { 0 };	/* empty instance of dxlStatusPacket_t to reset all fields to zero 
																 * !!! Attention !!! Make sure memory for dxl_status_packet.param is 
																 * deallocated and pointer is set to NULL before using empty instance
																 * to prevent memory fragmentation */

/*
	Function to receive and parse a Dynamixel status return packet. If a valid status return packet	has
	been received it will store the received data for further processing in the dxl_return_data array.
	return  0	= received data successfully stored in dxl_return_data
	return -1	= timeout while waiting for data (no data received)
	return -2	= timeout while receiving data
	return -4	= invalid data packet
	return -5   = error while allocating memory
*/
int8_t dxlGetStatusPacket(void)
{
	uint32_t time_stamp = 0;

	free(dxl_status_packet.param);					/* free previously allocated memory */
	dxl_status_packet.param = NULL;					/* clear pointer */
	dxl_status_packet = dxl_status_packet_empty;	/* reset instance to zero */

	while (micros() >= 0xFFFFFC17) /*  4294967295 �s (uint32) - 1000 �s = 4294966295 = 0xFFFFFC17 */
	{
		/* micros() will overflow after approximately 71,58 minutes, this might cause problems (not tested jet) so wait if micros() is close (1 ms) to overflow */
	}

	uint32_t timeout = micros() + DXL_TIMEOUT;

	while (micros() < timeout && dxlAvailableData() < 6)
	{
		/* wait for minimum Dynamixel status packet length of 5 or timeout */
	}

	time_stamp = micros();

	if (time_stamp > timeout && dxlAvailableData() > 0)
	{
		while (dxlAvailableData() > 0)
		{
			Serial.print(dxlReadData(), HEX);
			dxlFlush();
		}
		return DXL_ERR_TIMEOUT_WAITING_STATUS; /* timeout with data - incomplete response */
	}
	else if (time_stamp > timeout && dxlAvailableData() == 0)
	{
		return DXL_ERR_TIMEOUT_RECEIVING_STATUS; /* timeout without data - no response */
	}
	else
	{
		while (dxlAvailableData() > 0)
		{
			if (dxlPeekData() == DXL_START)
			{
				dxlReadData();									/* protocol start 0xFF */

				if (dxlPeekData() == DXL_START)
				{
					dxlReadData();								/* protocol start 0xFF */
					dxl_status_packet.id = dxlReadData();		/* id */
					dxl_status_packet.length = dxlReadData();	/* length */
					dxl_status_packet.param = (uint8_t*)calloc(dxl_status_packet.length, sizeof(uint8_t)); /* allocate memory for return data */

#ifdef _DEBUG
					char msg[64];
					sprintf(msg, "dxlGetStatusPacket: 0x%02X 0x%02X", dxl_status_packet.id, dxl_status_packet.length);
					Serial.print(msg);
#endif
					if (dxl_status_packet.param != NULL)	/* check for proper calloc execution */
					{
						uint8_t i = 0;
						while (dxlAvailableData() > 0 && i < dxl_status_packet.length)
						{
							dxl_status_packet.param[i] = dxlReadData();

#ifdef _DEBUG
							char msg_1[16];
							sprintf(msg_1, " 0x%02X", dxl_status_packet.param[i]);
							Serial.print(msg_1);
#endif
							i++;
						}
#ifdef _DEBUG
						Serial.println();
#endif

						if (i == (dxl_status_packet.length - 1) && dxlAvailableData() == 0) /* make sure the whole status return packet has been read and the buffer is empty */
						{
							dxl_status_packet.error = dxl_status_packet.param[0];
							dxl_status_packet.checksum = dxl_status_packet.param[dxl_status_packet.length - 1];
							
							uint16_t tmp = 0;
							for (int j = 0; j < dxl_status_packet.length - 1; j++)
							{
								tmp += dxl_status_packet.param[j];
							}

							if (dxl_status_packet.checksum == (uint8_t)((~(dxl_status_packet.id + dxl_status_packet.length + tmp)) & 0xFF))
							{
								return 0;
							}
							else
							{
								free(dxl_status_packet.param);
								dxl_status_packet.param = NULL;
								dxl_status_packet = { 0 };
								return DXL_ERR_CHECKSUM;  /* checksum error */
							}
						}
						else
						{
							/* error handling? */
						}
					}

					while (dxlAvailableData() > 0) /* make sure buffer is empty */
					{
#ifdef _DEBUG
						Serial.print(dxlReadData(), HEX);
						Serial.print(" ");
#else
						dxlReadData();
#endif
					}

					free(dxl_status_packet.param);
					dxl_status_packet.param = NULL;
					dxl_status_packet = { 0 };

					return DXL_ERR_MEMORY_ALLOCATION /* error while allocating memory */;
				}
			}
			else
			{
				while (dxlAvailableData() > 0) /* no proper Dynamixel packet start (0xFF 0xFF) - clear buffer to prevent unprocessed data in buffer*/
				{
					dxlReadData();
				}
			}
		}
	}
	return DXL_ERR_INVALID_STATUS_DATA; /* invalid data packet */
}

/*
	function to send Dynamixel command packets via half-duplex UART
	id					= id of the Dynamixel servo
	length				= length defined by the Dynamixel protocol
	instruction			= instruction defined by the Dynamixel control table
	param_list[]		= data array containing additional data to send (NULL if no additional data needed)
			  [0]		= length of array!
			  [1]		= Dynamixel register address to be read/ write
			  [1 + n]	= data
							- read instruction: number of bytes to read
							- write instruction: data to write
	return 0			= no error or no status return packet (when using broadcast id)
	return -1			= error, Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlWriteData(uint8_t id, uint8_t instruction, uint8_t *param_list)
{
	uint8_t checksum = 0;		/* checksum calculation defined by Dynamixel protocol -> ~(ID + Length + Instruction + Parameter1 + ... + Parameter N) */
	uint8_t length = 0;
	
	/* calculating checksum if no sync write instruction */
	if (instruction != DXL_INST_SYNC_WRITE)
	{
		uint16_t tmp = 0;
		if (param_list != NULL)
		{
			length = 2 + (param_list[0] - 1);
			for (uint8_t i = 0; i < param_list[0] - 1; i++)
			{
				tmp += param_list[i + 1];
			}
		}
		else
		{
			length = 2;
		}

		checksum = (uint8_t)(~(id + length + instruction + tmp));

		dxlSetComMode(DXL_TX_MODE);

		dxlSendData(DXL_START);
		dxlSendData(DXL_START);
		dxlSendData(id);
		dxlSendData(length);
		dxlSendData(instruction);

#ifdef _DEBUG
		char msg[128];
		sprintf(msg, "dxlWrite(): data sent: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", DXL_START, DXL_START, id, length, instruction);
		Serial.print(msg);
#endif
		if (param_list != NULL)
		{
			for (uint8_t i = 0; i < length - 2; i++)
			{
				dxlSendData(param_list[i + 1]);

#ifdef _DEBUG
				char msg_2[16];
				sprintf(msg_2, " 0x%02X", param_list[i + 1]);
				Serial.print(msg_2);
#endif
			}
		}

#ifdef _DEBUG
		char msg_3[16];
		sprintf(msg_3, " 0x%02X", checksum);
		Serial.println(msg_3);
#endif
		dxlSendData(checksum);
		dxlFlush();

		dxlSetComMode(DXL_RX_MDOE);

		if (id != DXL_BROADCASTING_ID)
		{
			return dxlGetStatusPacket();
		}
		else
		{
			return 0; /* no status return packen when using broadcast id */
		}
	}
	else
	{
		return DXL_ERR_NO_SYNC_WRITE; /* sync write not supported jet */
	}
}

int8_t dxlPing(uint8_t id)
{
	uint8_t *param = NULL;
	return dxlWriteData(id, DXL_INST_PING, param);
}

int8_t dxlReset(uint8_t id)
{
	uint8_t *param = NULL;
	return dxlWriteData(id, DXL_INST_RESET, param);
}

int8_t dxlAction(uint8_t id)
{
	uint8_t *param = NULL;
	return dxlWriteData(id, DXL_INST_ACTION, param);
}

int8_t dxlSyncWrite(void)
{
	SendStatus("dxlSyncWrite(): ", "sync write not supported", SYS_STAT_MSG_TYPE_ERROR);
	return -1;
}

/* 
	Dynamixel write functions
*/
int8_t dxlSetId(uint8_t id, uint8_t new_id)
{
	uint8_t param[] = { 3, DXL_P_ID, new_id };

	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetBaudRate(uint8_t id, uint8_t baud_rate)
{
	uint8_t param[] = { 3, DXL_P_BAUD_RATE, baud_rate };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetReturnDelay(uint8_t id, uint8_t return_delay)
{
	uint8_t param[] = { 3, DXL_P_RETURN_DELAY_TIME, return_delay };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetCWAngleLimit(uint8_t id, uint16_t cw_angle_limit)
{
	uint8_t param[] = { 4, DXL_P_CW_ANGLE_LIMIT_L, (uint8_t)cw_angle_limit, (uint8_t)cw_angle_limit >> 8 };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetCCWAngleLimit(uint8_t id, uint16_t ccw_angle_limit)
{
	uint8_t param[] = { 4, DXL_P_CW_ANGLE_LIMIT_L, (uint8_t)ccw_angle_limit, (uint8_t)ccw_angle_limit >> 8 };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetTempLimit(uint8_t id, uint8_t max_temp_limit)
{
	uint8_t param[] = { 3, DXL_P_LIMIT_TEMPERATURE, max_temp_limit };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetVoltageLimitLow(uint8_t id, uint8_t min_voltage_limit)
{
	uint8_t param[] = { 3,DXL_P_DOWN_LIMIT_VOLTAGE, min_voltage_limit };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetVoltageLimitHigh(uint8_t id, uint8_t max_voltage_limit)
{
	uint8_t param[] = { 3,DXL_P_DOWN_LIMIT_VOLTAGE, max_voltage_limit };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetMaxTorque(uint8_t id, uint16_t max_torque)
{
	uint8_t param[] = { 4, DXL_P_MAX_TORQUE_L, (uint8_t)max_torque, (uint8_t)max_torque >> 8 };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetStatusReturnLevel(uint8_t id, uint8_t return_level)
{
	uint8_t param[] = { 3, DXL_P_RETURN_LEVEL, return_level };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetAlarmLED(uint8_t id, uint8_t alarm_led)
{
	uint8_t param[] = { 3, DXL_P_ALARM_LED, alarm_led };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetAlarmShutdown(uint8_t id, uint8_t alarm_shutdown)
{
	uint8_t param[] = { 3, DXL_P_ALARM_SHUTDOWN, alarm_shutdown };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetTorqueEnable(uint8_t id, uint8_t torque_enable)
{
	uint8_t param[] = { 3, DXL_P_TORQUE_ENABLE, torque_enable };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetLED(uint8_t id, uint8_t led)
{
	uint8_t param[] = { 3, DXL_P_LED, led };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetCWComplianceMargin(uint8_t id, uint8_t cw_compliance_margin)
{
	uint8_t param[] = { 3, DXL_P_CW_COMPLIANCE_MARGIN, cw_compliance_margin };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetCCWComplianceMargin(uint8_t id, uint8_t ccw_compliance_margin)
{
	uint8_t param[] = { 3, DXL_P_CCW_COMPLIANCE_MARGIN, ccw_compliance_margin };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetCWComplianceSlope(uint8_t id, uint8_t cw_compliance_slope)
{
	uint8_t param[] = { 3, DXL_P_CW_COMPLIANCE_SLOPE, cw_compliance_slope };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetCCWComplianceSlope(uint8_t id, uint8_t ccw_compliance_slope)
{
	uint8_t param[] = { 3, DXL_P_CCW_COMPLIANCE_SLOPE, ccw_compliance_slope };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetGoalPosition(uint8_t id, uint16_t goal_position)
{
	uint8_t param[] = { 4, DXL_P_GOAL_POSITION_L, (uint8_t)goal_position, (uint8_t)goal_position >> 8 };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetGoalSpeed(uint8_t id, uint16_t goal_speed)
{
	uint8_t param[] = { 4, DXL_P_GOAL_SPEED_L, (uint8_t)goal_speed, (uint8_t)goal_speed >> 8 };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetTorqueLimit(uint8_t id, uint16_t torque_limit)
{
	uint8_t param[] = { 4, DXL_P_TORQUE_LIMIT_L, (uint8_t)torque_limit, (uint8_t)torque_limit >> 8 };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetRegisteredInstruction(uint8_t id, uint8_t *param_list)
{
	return dxlWriteData(id, DXL_INST_REG_WRITE, param_list);
}

int8_t dxlSetLock(uint8_t id, uint8_t lock)
{
	uint8_t param[] = { 3, DXL_P_LOCK, lock };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetPunch(uint8_t id, uint16_t punch)
{
	uint8_t param[] = { 4, DXL_P_PUNCH_L, (uint8_t)punch, (uint8_t)punch >> 8 };
	return dxlWriteData(id, DXL_INST_WRITE, param);
}

int8_t dxlSetCustomData(uint8_t id, const uint8_t DXL_INST_, uint8_t *param_list)
{
	return dxlWriteData(id, DXL_INST_, param_list);
}

/* 
	Dynamixel read functions
*/
int8_t dxlGetModelNumber(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_MODEL_NUMBER_L, 2 };
	return dxlWriteData(id,  DXL_INST_READ, param);
}

int8_t dxlGetFirmwareVersion(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_VERSION, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetCWAngleLimit(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_CW_ANGLE_LIMIT_L, 2 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetCCWAngleLimit(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_CCW_ANGLE_LIMIT_L, 2 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetTempLimit(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_LIMIT_TEMPERATURE, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetVoltageLimit(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_UP_LIMIT_VOLTAGE, 2 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetMaxTorque(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_MAX_TORQUE_L, 2};
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetStatusReturnLevel(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_RETURN_LEVEL, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetAlarmLED(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_ALARM_LED, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetAlarmShutdown(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_ALARM_LED, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetTorqueEnable(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_TORQUE_ENABLE, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetLED(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_LED, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetCWComplianceMargin(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_CW_COMPLIANCE_MARGIN, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetCCWComplianceMargin(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_CCW_COMPLIANCE_MARGIN, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetCWComplianceSlope(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_CW_COMPLIANCE_SLOPE, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetCCWComplianceSlope(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_CCW_COMPLIANCE_SLOPE, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetGoalPosition(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_GOAL_POSITION_L, 2};
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetMovingSpeed(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_GOAL_SPEED_L, 2 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetTorqueLimit(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_TORQUE_LIMIT_L, 2 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetPresentPosition(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_PRESENT_POSITION_L, 2};
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetPresentSpeed(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_PRESENT_SPEED_L, 2 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetPresentLoad(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_PRESENT_LOAD_L, 2 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetPresentVoltage(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_PRESENT_VOLTAGE, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetPresentTemperature(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_PRESENT_TEMPERATURE, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetRegisteredInstruction(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_REGISTERED_INSTRUCTION, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetMoving(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_MOVING, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetLock(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_LOCK, 1 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetPunch(uint8_t id)
{
	uint8_t param[] = { 3, DXL_P_PUNCH_L, 2 };
	return dxlWriteData(id, DXL_INST_READ, param);
}

int8_t dxlGetCustomData(uint8_t id, uint8_t start_register, uint8_t length)
{
	uint8_t param[] = { 3, start_register, length };
	return dxlWriteData(id, DXL_INST_READ, param);
}

void dxlStartCom(uint32_t baudRate, uint8_t directionPin)
{
	dxlSetDirPin(directionPin);
	dxlBeginCom(baudRate);
	dxlSetComMode(DXL_TX_MODE);
}

void initDynamixel(void)
{
	int tmp = -1;
	int response = 0;

	/* search for servo motors */
	for (int i = 0; i <= 2; i++)
	{
		tmp = dxlPing(i) * (-1);

		if (tmp != 0)
		{
			dxlPrintErrorMessage(tmp, i);
			char msg[32];
			sprintf(msg, "no Dynamixel found @ ID: %d", i);
			SendStatus("InitDynamixel(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		}
		else
		{
			char msg[64];
			sprintf(msg, "Dynamixel found @ ID: %d", i);
			SendStatus("InitDynamixel(): ", msg, SYS_STAT_MSG_TYPE_INFO);
			response++;
		}
	}

	if (response != 3)
	{
		SendStatus("InitDynamixel(): ", "check wiring and ID's of Dynamixel servos and restart the controller", SYS_STAT_MSG_TYPE_ERROR);
		/* if one of the servos could not be found, set the system error state to prevent further operations */
#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR, false);
#endif
		return;
	}
	else
	{
		uint8_t dxl_error = 0;
		int8_t internal_error = 0;
		
		/* read configuration of the Dynamixel servos */
		for (int id = 0; id < 3; id++)
		{
			internal_error = dxlGetCustomData(id, DXL_P_LIMIT_TEMPERATURE, 8);
			
			if (internal_error == 0) /* dxl error? */
			{
				if ((dxl_status_packet.length - 2) >= DXL_SETUP_TOTAL_SIZE) /* check for proper length */
				{ 
					if (dxl_status_packet.param != NULL) /* check for NULL pointer */
					{
						for (int i = 0; i < DXL_SETUP_TOTAL_SIZE; i++)
						{
							if (internal_error == 0 && dxl_error == 0)
							{
								if (dxl_status_packet.param[i] != dxl_setup_params[id][i])
								{
									switch (i)
									{
									case DXL_SETUP_TEMP_LIMIT:
										internal_error = dxlSetTempLimit(id, dxl_setup_params[id][i]);
										if (internal_error == 0)
											dxl_error = dxl_status_packet.error;
										break;

									case DXL_SETUP_VOLTAGE_LIMIT_LOW:
										internal_error = dxlSetVoltageLimitLow(id, dxl_setup_params[id][i]);
										if (internal_error == 0)
											dxl_error = dxl_status_packet.error;
										break;

									case DXL_SETUP_VOLTAGE_LIMIT_HIGH:
										internal_error = dxlSetVoltageLimitHigh(id, dxl_setup_params[id][i]);
										if (internal_error == 0)
											dxl_error = dxl_status_packet.error;
										break;

									case DXL_SETUP_MAX_TORQUE:
										internal_error = dxlSetMaxTorque(id, dxl_setup_params[id][i - 1] | (dxl_setup_params[id][i] << 8));
										if (internal_error == 0)
											dxl_error = dxl_status_packet.error;
										break;

									case DXL_SETUP_STATUS_RETURN_LEVEL:
										internal_error = dxlSetStatusReturnLevel(id, dxl_setup_params[id][i]);
										if (internal_error == 0)
											dxl_error = dxl_status_packet.error;
										break;

									case DXL_SETUP_RETURN_DELAY_TIME:
										internal_error = dxlSetReturnDelay(id, dxl_setup_params[id][i]);
										if (internal_error == 0)
											dxl_error = dxl_status_packet.error;
										break;

									case DXL_SETUP_ALARM_LED:
										internal_error = dxlSetAlarmLED(id, dxl_setup_params[id][i]);
										if (internal_error == 0)
											dxl_error = dxl_status_packet.error;
										break;

									case DXL_SETUP_ALARM_SHUT_DOWN:
										internal_error = dxlSetAlarmShutdown(id, dxl_setup_params[id][i]);
										if (internal_error == 0)
											dxl_error = dxl_status_packet.error;
										break;

									default:
										break;
									}
								}

								if (internal_error != 0)
								{
									dxlPrintErrorMessage(internal_error, id);
									break;
								}

								if (dxl_error != 0)
								{
									dxlPrintErrorMessage(dxl_error, id);
									break;
								}
							}
						}
					}
					else
					{
						/* pointer to parameter list does not exists */
						internal_error = DXL_ERR_NO_PARAM_LIST;
						dxlPrintErrorMessage(internal_error, id);
						break;
					}
				}
				else
				{
					/* not enough data to proceed */
					internal_error = DXL_ERR_STATUS_PACKET_SIZE;
					dxlPrintErrorMessage(internal_error, id);
					break;
				}
			}
			else
			{
				/* configuration could not be read */
				dxlPrintErrorMessage(internal_error, id);
				break;
			}
		}
		
		if (internal_error == 0)
		{
			/* continuous turn & torque enable! */
		}
		else
		{
#ifndef _DEBUG
			SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR, false);
#endif
		}
	}
}

void dxlPrintErrorMessage(int16_t error, uint8_t id)
{
	char msg[128];

	switch (error)
	{
	case DXL_ERR_INPUT_VOLTAGE:
		sprintf(msg, "Dynamixel - Input Voltage Error @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_ANGLE_LIMIT:
		sprintf(msg, "Dynamixel - Angle Limit Error @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_OVERHEATING:
		sprintf(msg, "Dynamixel - Overheating Error @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_RANGE:
		sprintf(msg, "Dynamixel - Range Error @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_CHECKSUM:
		sprintf(msg, "Dynamixel - Checksum Error @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_OVERLOAD:
		sprintf(msg, "Dynamixel - Overload Error @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_INSTRUCTION:
		sprintf(msg, "Dynamixel - Instruction Error @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_NO_RESPONSE:
		sprintf(msg, "Dynamixel - no response @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_TIMEOUT_WAITING_STATUS:
		sprintf(msg, "internal error - timeout while waiting for status packet @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_TIMEOUT_RECEIVING_STATUS:
		sprintf(msg, "internal error - timeout while receiving status packet @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_INVALID_STATUS_DATA:
		sprintf(msg, "internal error - invalid status packet @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_MEMORY_ALLOCATION:
		sprintf(msg, "internal error - error while allocating memory @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_STATUS_PACKET_SIZE:
		sprintf(msg, "internal error - wrong status packet size @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_NO_PARAM_LIST:
		sprintf(msg, "internal error - no parameter list for status packet @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	case DXL_ERR_NO_SYNC_WRITE:
		sprintf(msg, "internal error - sync write not supported @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;

	default:
		sprintf(msg, "unknown error @ ID: %u", id);
		SendStatus("dxlPrintErrorMessage(): ", msg, SYS_STAT_MSG_TYPE_ERROR);
		break;
	}

#ifndef _DEBUG
		SetObjData(OBJ_IDX_SYS_STATUS, GetObjData(OBJ_IDX_SYS_STATUS) | SYS_STAT_DYNAMIXEL_ERROR, false);
#endif
	
}

void handleMove(void) {

	enum minMaxPos {
		MIN_POS,
		MAX_POS
	};

	if (!(GetObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_ERROR))
	{
		uint8_t moving = (uint8_t)GetObjData(OBJ_IDX_MOVING);
		uint8_t start = (uint8_t)GetObjData(OBJ_IDX_START_MOVE);

		float actPos[3] = /* actual position values */
		{
			GetObjData(OBJ_IDX_X_ACTUAL_POS) / 10.0,
			GetObjData(OBJ_IDX_Y_ACTUAL_POS) / 10.0,
			GetObjData(OBJ_IDX_Z_ACTUAL_POS) / 10.0
		};
		float actTargetPos[3] = /* actual target position values */
		{
			GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS) / 10.0
		};
		float newTargetPos[3] = /* new target position values */
		{
			GetObjData(OBJ_IDX_X_NEW_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Y_NEW_TARGET_POS) / 10.0,
			GetObjData(OBJ_IDX_Z_NEW_TARGET_POS) / 10.0,
		};
		float posWindow[3][2] = /* allowed position windows */
		{
			{ actTargetPos[DXL_ID_AXIS_1] - DXL_POS_TOLERANCE, actTargetPos[DXL_ID_AXIS_1] + DXL_POS_TOLERANCE },
			{ actTargetPos[DXL_ID_AXIS_2] - DXL_POS_TOLERANCE, actTargetPos[DXL_ID_AXIS_2] + DXL_POS_TOLERANCE },
			{ actTargetPos[DXL_ID_AXIS_Z] - DXL_POS_TOLERANCE, actTargetPos[DXL_ID_AXIS_Z] + DXL_POS_TOLERANCE },
		};

		if (!start && !moving)
		{
			return;
		}
		else if (start && !moving)
		{
			/*  *  - schreibe neue Zielpos in aktuelle Zielpos
				*  - �bergebe neue Zielpos an Dynamixel
				*  - starte Bewegung der Motoren (sync Start?)
				*  - setze moving
				*  - start Kommando zur�ck setzten
				*  - ende */

				/* goal position changed? */
			if (actPos[DXL_ID_AXIS_1] < posWindow[DXL_ID_AXIS_1][MIN_POS] || actPos[DXL_ID_AXIS_1] > posWindow[DXL_ID_AXIS_1][MAX_POS] ||
				actPos[DXL_ID_AXIS_2] < posWindow[DXL_ID_AXIS_2][MIN_POS] || actPos[DXL_ID_AXIS_2] > posWindow[DXL_ID_AXIS_2][MAX_POS] ||
				actPos[DXL_ID_AXIS_Z] < posWindow[DXL_ID_AXIS_Z][MIN_POS] || actPos[DXL_ID_AXIS_Z] > posWindow[DXL_ID_AXIS_Z][MAX_POS])
			{
				SetObjData(OBJ_IDX_POS_REACHED, 0, false);

				if (newTargetPos[DXL_ID_AXIS_1] != actTargetPos[DXL_ID_AXIS_1])
				{
					SetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_X_NEW_TARGET_POS), false);
				}
				if (newTargetPos[DXL_ID_AXIS_2] != actTargetPos[DXL_ID_AXIS_2])
				{
					SetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Y_NEW_TARGET_POS), false);
				}
				if (newTargetPos[DXL_ID_AXIS_Z] != actTargetPos[DXL_ID_AXIS_Z])
				{
					SetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS, GetObjData(OBJ_IDX_Z_NEW_TARGET_POS), false);

					if (newTargetPos[DXL_ID_AXIS_Z] > actTargetPos[DXL_ID_AXIS_Z])
					{
						//dynamixelTurn(DXL_ID_Z_AXIS, RIGTH, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); /* besser �ber moveRW f�r sync start mit x & y */
					}
					else
					{
						//dynamixelTurn(DXL_ID_Z_AXIS, LEFT, GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_SPEED)); /* besser �ber moveRW f�r sync start mit x & y */
					}
				}
				int16_t val1 = GetObjData(OBJ_IDX_X_ACTUAL_TARGET_POS);
				int16_t val2 = GetObjData(OBJ_IDX_Y_ACTUAL_TARGET_POS);

				/* calculate new target position*/
				CalcAngle(&val1, &val2);

				uint8_t param_list[4][3] = {
					{
						DXL_P_GOAL_SPEED_L,
						(uint8_t)GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_SPEED),		/* low byte */
						(uint8_t)(GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_SPEED) >> 8)	/* high byte */
					},
					{
						DXL_P_GOAL_SPEED_L,
						(uint8_t)GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_SPEED),
						(uint8_t)(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_SPEED) >> 8)
					},
					{
						DXL_P_GOAL_POSITION_L,
						(uint8_t)DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE)),
						(uint8_t)(DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_1_ACTUAL_TARGET_ANGLE)) >> 8)
					},
					{
						DXL_P_GOAL_POSITION_L,
						(uint8_t)DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE)),
						(uint8_t)(DEG_TO_DYNA(GetObjData(OBJ_IDX_AXIS_2_ACTUAL_TARGET_ANGLE)) >> 8)
					}
				};

				dxlSetRegisteredInstruction(DXL_ID_AXIS_1, param_list[0]);
				dxlSetRegisteredInstruction(DXL_ID_AXIS_2, param_list[1]);
				dxlSetRegisteredInstruction(DXL_ID_AXIS_1, param_list[2]);
				dxlSetRegisteredInstruction(DXL_ID_AXIS_2, param_list[3]);
				dxlAction(DXL_BROADCASTING_ID); /* sync start */

				SetObjData(OBJ_IDX_MOVING, 1, true);
				SetObjData(OBJ_IDX_START_MOVE, 0, false);
			}
			else
			{
				SetObjData(OBJ_IDX_START_MOVE, 0, false);
			}
		}
		else if (start && moving)
		{
			SetObjData(OBJ_IDX_START_MOVE, 0, false);
		}
		else /* (!start && moving) */
		{
			/* target position reached? */
			if (actPos[DXL_ID_AXIS_1] >= posWindow[DXL_ID_AXIS_1][MIN_POS] && actPos[DXL_ID_AXIS_1] <= posWindow[DXL_ID_AXIS_1][MAX_POS] &&
				actPos[DXL_ID_AXIS_2] >= posWindow[DXL_ID_AXIS_2][MIN_POS] && actPos[DXL_ID_AXIS_2] <= posWindow[DXL_ID_AXIS_2][MAX_POS] &&
				actPos[DXL_ID_AXIS_Z] >= posWindow[DXL_ID_AXIS_Z][MIN_POS] && actPos[DXL_ID_AXIS_Z] <= posWindow[DXL_ID_AXIS_Z][MAX_POS])
			{
				SetObjData(OBJ_IDX_MOVING, 0, true);
				SetObjData(OBJ_IDX_POS_REACHED, 1, false);
			}
			else
			{
				/* z-Achse Rampe Geschwindigkeit */
			}
		}
	}
	else
	{
		/* stoppe Bewegung, deaktiviere Drehmoment? */
	}
}