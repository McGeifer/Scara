// 
// 
// 

#include "gpio.h"
#include "objdir.h"
#include "status.h"

void initGPIO(void)
{
	/* set pin modes */
	pinMode(MODBUS_PIN, INPUT);		/* pin to enable Modbus communication */
	pinMode(RAPID_PIN, INPUT);		/* pin to enable "Rapid" mode */
	pinMode(MAGNET_PIN, OUTPUT);	/* pin to switch the lifting magnet on/off */
	pinMode(INTERRUPT_PIN, INPUT);	/* interrupt pin for the light barrier */

	/* interrupts */
	attachInterrupt(digitalPinToInterrupt(3), lightBarrierISR, CHANGE);	/* attach interrupt for light barrier impulse counter */
}

void initOperationMode(void)
{
	if (!digitalRead(RAPID_PIN) && digitalRead(MODBUS_PIN))
	{
		if (SetObjData(OBJ_IDX_OP_MODE, OP_MODE_MODBUS, false) == 0)
		{
			/* modbus protocol selected */
			return;
		}
	}
	else if (digitalRead(RAPID_PIN) && !digitalRead(MODBUS_PIN))
	{
		if (SetObjData(OBJ_IDX_OP_MODE, OP_MODE_RAPID, false) == 0)
		{
			Serial.println("");
			Serial.println("");
			SendStatus("initOperationMode(): ", "Rapid protocol selected", STATUS_MSG_TYPE_INFO);
			/* rapid protocol selected */
			return;
		}
	}
	else {
		if (SetObjData(OBJ_IDX_OP_MODE, OP_MODE_SCARA, false) == 0)
		{
			SendStatus("initOperationMode(): ", "Scara protocol selected", STATUS_MSG_TYPE_INFO);
			/* scara protocol selected */
			return;
		}
	}
	SendStatus("initOperationMode(): ", "error selecting protocol", STATUS_MSG_TYPE_ERROR);
}

void lightBarrierISR(void)
{
	if (GetObjData(OBJ_IDX_Z_ACTUAL_POS) > GetObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS))
	{
		SetObjData(OBJ_IDX_Z_POS_COUNT, GetObjData(OBJ_IDX_Z_POS_COUNT) - 1, false);
	}
	else
	{
		SetObjData(OBJ_IDX_Z_POS_COUNT, GetObjData(OBJ_IDX_Z_POS_COUNT) + 1, false);
	}
}