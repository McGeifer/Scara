// 
// 
// 

#include "status.h"
#include "objdir.h"

extern bool run;

static void SendStatus(char* optionalDebugMessage, char* message, uint8_t statusType) {

	char statusString[10];
	char finalMsgString[128];
	
	uint8_t i = 0x02; // for testing
	switch (i/*GetObjStructData(0xFE)*/)
	{
	case OP_MODE_SCARA:
	case OP_MODE_RAPID:

		switch (statusType)
		{
		case STATUS_TYPE_NOTYPE:
			sprintf(statusString, "         ");
			break;

		case STATUS_TYPE_INFO:
			sprintf(statusString, "Info:    ");
			break;

		case STATUS_TYPE_WARNING:
			sprintf(statusString, "Warning: ");
			break;

		case STATUS_TYPE_ERROR:
			sprintf(statusString, "Error:   ");
			break;

		case STATUS_TYPE_DEBUG:
			if (GetObjStructData(0xFF) & SYS_STAT_DEBUG) {
				sprintf(statusString, "Debug:   ");
				sprintf(finalMsgString, "%c %c %c", statusString, optionalDebugMessage, message);
				Serial.println(finalMsgString);
			}
			return;

		default:
			sprintf(statusString, "         ");
			break;
		}
		break;

	case OP_MODE_MODBUS:
		// no status message available, possible implementation via modbus exeption codes -> https://en.wikipedia.org/wiki/Modbus#Main_Modbus_exception_codes
		break;

	default:
		break;
	}
	sprintf(finalMsgString, "%c %c", statusString, message);
	Serial.println(finalMsgString);
}

void SystemStatus() {

	switch (GetObjStructData(0xFF))
	{
	case 0:
		// no error
		break;

	case SYS_STAT_DYNAMIXEL_ERROR:
		SendStatus(NULL, "Not all Dynamixel servos could be found, check wiring and restart the system.", STATUS_TYPE_ERROR);
		run = false;
		break;

	case SYS_STAT_UNKOWN_ERROR:
		SendStatus(NULL, "Unknown error, check and restart the system.", STATUS_TYPE_ERROR);
		run = false;
		break;

	default:
		break;
	}
}