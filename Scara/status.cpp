// 
// 
// 

#include "status.h"
#include "objdir.h"

void SendStatus(char* message, uint8_t statusType) {

	switch (GetObjStructData(0xFE))
	{
	case OP_MODE_SCARA:
	case OP_MODE_RAPID:

		switch (statusType)
		{
		case STATUS_TYPE_INFO:
			Serial.print("Info: ");
			break;

		case STATUS_TYPE_WARNING:
			Serial.print("Warning: : ");
			break;

		case STATUS_TYPE_ERROR:
			Serial.print("Error: ");
			break;

		/*case STATUS_TYPE_NOTYPE:
			Serial.print(message);
			break;*/

		default:
			Serial.print("no statusType: ");
			break;
		}
		Serial.println(message);
		break;

	case OP_MODE_MODBUS:
		// no status message available, possible implementation via modbus exeption code -> https://en.wikipedia.org/wiki/Modbus#Main_Modbus_exception_codes
		break;

	default:
		break;
	}
}