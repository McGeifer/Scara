// 
// 
// 

#include "status.h"
#include "objdir.h"

extern bool run;

static void SendStatus(char* message, uint8_t statusType) {

	uint8_t i = 0x02; // for testing
	switch (i/*GetObjStructData(0xFE)*/)
	{
	case OP_MODE_SCARA:
	case OP_MODE_RAPID:

		switch (statusType)
		{
		case STATUS_TYPE_NOTYPE:
			Serial.print(message);
			break;

		case STATUS_TYPE_INFO:
			Serial.print("Info:    ");
			Serial.println(message);
			break;

		case STATUS_TYPE_WARNING:
			Serial.print("Warning: ");
			Serial.println(message);
			break;

		case STATUS_TYPE_ERROR:
			Serial.print("Error:   ");
			Serial.println(message);
			break;

		case STATUS_TYPE_DEBUG:
			if (GetObjStructData(0xFF) & SYS_STAT_DEBUG) {
				Serial.print("Debug:   ");
				Serial.println(message);
			}
			break;

		default:
			Serial.print("no statusType: ");
			Serial.println(message);
			break;
		}
		break;

	case OP_MODE_MODBUS:
		// no status message available, possible implementation via modbus exeption codes -> https://en.wikipedia.org/wiki/Modbus#Main_Modbus_exception_codes
		break;

	default:
		break;
	}
}

void SystemStatus() {

	switch (GetObjStructData(0xFF))
	{
	case 0:
		// no error
		break;

	case SYS_STAT_DYNAMIXEL_ERROR:
		SendStatus("Not all Dynamixel servos could be found, check wiring and restart the system.", STATUS_TYPE_ERROR);
		run = false;
		break;

	case SYS_STAT_UNKOWN_ERROR:
		SendStatus("Unknown error, check and restart the system.", STATUS_TYPE_ERROR);
		run = false;
		break;

	default:
		break;
	}
}