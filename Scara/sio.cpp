// 
// 
// Setup for serial/ USB connection

#include "sio.h"
#include "objdir.h"
#include "Stream.h"
#include "DynamixelSerial2.h"
#include "SimpleModbusSlave.h"

void InitSio() {

	// Setup for USB connection;
	Serial.begin(115200);
	Serial.setTimeout(500);

	// Setup for Modbus connection (Serial1)
	modbus_configure(115200, 1, 0, TOTAL_REGS_SIZE_MODBUS, 0);

	// Setup for Dynamixel connection (Serial2)
	Dynamixel.begin(1000000, 2);
}

void HandleSio() {

	// get system error state
	int16_t tmp = GetObjStructData(0xFF);

	if (tmp == 0) {

		switch (GetObjStructData(0xFE))	{

		case OP_MODE_MODBUS:
			HandleModbus();

		case OP_MODE_RAPID:
			//

		case OP_MODE_SCARA:
			//

		default:

			break;
		}
	}
}

// Receive a Modbus data package via UART
void HandleModbus() {

	// Temporary variables to get the proper type for further processing.
	int regSize_tmp = TOTAL_REGS_SIZE_MODBUS - 1;
	uint8_t holdingRegs_tmp[TOTAL_REGS_SIZE_MODBUS - 1];

	while (Serial1.available() > 0)
	{
		// returns the total error count since the slave started.
		holdingRegs[TOTAL_ERRORS_MODBUS] = modbus_update(holdingRegs);

		// get the low bytes from the register values
		for (int i = 0; i < TOTAL_REGS_SIZE_MODBUS - 1; i++)
		{
			holdingRegs_tmp[i] = holdingRegs[i];
		}

		//// Search for valid byte sequences
		//if (readBufferBytes(holdingRegs_tmp, &regSize_tmp, &opModeAngle,
		//	&startMove, &angleATarget, &angleBTarget, &positionXTarget,
		//	&positionYTarget, &positionZTargetTmp, &positionZTarget))
		//{
		//	sendAcknowledge();
		//	if (startMove)
		//		executeByteSequence(&opModeAngle, &startMove, &startMoveZ, &angleATarget,
		//			&angleBTarget, &positionXTarget, &positionYTarget, &speed_A, &speed_B);
		//}
		//else
		//{
		//	Serial.println("Fehler in readBufferBytes");
		//}
	}
}

void HandleRapid() {


}

void HandleScara() {


}