// 
// 
// 

#include "status.h"
#include "objdir.h"

extern bool run;

static void SendStatus(char* optionalDebugMessage, char* message, uint8_t statusType) {
	
	if (!(GetObjStructData(0xFF) & SYS_STAT_SILENT)) { // no messages will be send if system ist in silent mode

		char statusString[10];
		char finalMsgString[128];

		switch (GetObjStructData(OBJ_IDX_OP_MODE))
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
				sprintf(statusString, "Debug:   ");
				break;

			default:
				sprintf(statusString, "         ");
				break;
			}

			if (GetObjStructData(0xFF) & SYS_STAT_DEBUG) { // check if system ist in debug mode
				if (optionalDebugMessage == NULL) {
					sprintf(finalMsgString, "%s %s", statusString, message);
					Serial.println(finalMsgString);
				}
				else {
					sprintf(finalMsgString, "%s %s %s", statusString, optionalDebugMessage, message);
					Serial.println(finalMsgString);
				}
			}
			else {
				if (statusType != STATUS_TYPE_DEBUG) {
					sprintf(finalMsgString, "%s %s", statusString, message);
					Serial.println(finalMsgString);
				}
			}
			break;

		case OP_MODE_MODBUS:
			// no status message available, possible implementation via modbus exeption codes -> https://en.wikipedia.org/wiki/Modbus#Main_Modbus_exception_codes
			break;

		default:
			break;
		}
	}
}

void SystemStatus() {

	switch (GetObjStructData(OBJ_IDX_SYS_STATUS))
	{
	case 0:
		// no error
		break;

	case SYS_STAT_DYNAMIXEL_ERROR:
		SendStatus(NULL, "Not all Dynamixel servos could be found, check wiring and restart the system.", STATUS_TYPE_ERROR);
		run = false;
		break;

	case SYS_STAT_UNKOWN_ERROR:
		SendStatus(NULL, "Unknown error, check wiring and restart the system.", STATUS_TYPE_ERROR);
		run = false;
		break;

	default:
		break;
	}
}