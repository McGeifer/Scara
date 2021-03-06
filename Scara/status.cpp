/*
 * 
 */ 

#include "status.h"
#include "objdir.h"

void SendStatus(const char *optionalDebugMessage, const char *message, uint8_t statusType) {
	
	if (!(getObjData(0xFF) & SYS_STAT_SILENT)) /* no messages will be send if system is in silent mode */
	{ 
		char statusString[10];
		char finalMsgString[128];

		switch (getObjData(OBJ_IDX_OP_MODE))
		{
		case OP_MODE_SCARA:
		case OP_MODE_RAPID:

			switch (statusType)
			{
			case SYS_STAT_MSG_TYPE_NONE:
				sprintf(statusString, "         ");
				break;

			case SYS_STAT_MSG_TYPE_INFO:
				sprintf(statusString, "Info:    ");
				break;

			case SYS_STAT_MSG_TYPE_WARNING:
				sprintf(statusString, "Warning: ");
				break;

			case SYS_STAT_MSG_TYPE_ERROR:
				sprintf(statusString, "Error:   ");
				break;

			case SYS_STAT_MSG_TYPE_DEBUG:
				sprintf(statusString, "Debug:   ");
				break;

			default:
				sprintf(statusString, "         ");
				break;
			}

			if (getObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_DEBUG) { /* check if system ist in debug mode */
				if (optionalDebugMessage == NULL)
				{
					sprintf(finalMsgString, "%s %s", statusString, message);
					Serial.println(finalMsgString);
				}
				else
				{
					sprintf(finalMsgString, "%s %s %s", statusString, optionalDebugMessage, message);
					Serial.println(finalMsgString);
				}
			}
			else
			{
				if (statusType != SYS_STAT_MSG_TYPE_DEBUG)
				{
					sprintf(finalMsgString, "%s %s", statusString, message);
					Serial.println(finalMsgString);
				}
			}
			break;

		case OP_MODE_MODBUS:
			/* no status message available, possible implementation via modbus exception codes -> https://en.wikipedia.org/wiki/Modbus#Main_Modbus_exception_codes */
			break;

		default:
			break;
		}
	}
}