/*
 *  Name:		Scara.ino
 *  Created:	06.09.2018 14:50:44
 *  Author:		Jens Schiller
 */

#include "SimpleModbusSlave.h"
#include "objdir.h"
#include "gpio.h"
#include "status.h"
#include "sio.h"
#include "calc.h"
#include "dynamixel.h"

uint32_t cycle_time;
uint32_t cycle_count;

/* extern void InitDynamixel(void);
extern void UpdateObjDir(void);
extern void handleMove(void); */

void setup()
{
	initGPIO();
	initSio();
	initOperationMode();
	//initDynamixel();
	
	/*uint8_t idx1 = 0x01;
	uint8_t idx2 = 0x02;
	int16_t x1 = 25;
	int16_t y1 = 50;
	int16_t z1 = 75;
	int16_t x2 = 0;
	int16_t y2 = -50;
	int16_t z2 = 10;
	SetPosRegData(&idx1, &x1, &y1, &z1);
	SetPosRegData(&idx2, &x2, &y2, &z2);*/

	/*int16_t angle1 = 1735;
	int16_t angle2 = 680;
	float *tmp = CalcPosistion(&angle1, &angle2);
	Serial.print("val1 :");
	Serial.println(tmp[0]);
	Serial.print("val2 :");
	Serial.println(tmp[1]);*/

	/*int16_t xPos = 2180;
	int16_t yPos = -264;
	int8_t tmp = CalcAngle(&xPos, &yPos);
	Serial.println(tmp);*/
}

void loop()
{
	while (!(GetObjData(OBJ_IDX_SYS_STATUS) & SYS_STAT_ERROR))
	{
		int8_t tmp = dxlSetLED(DXL_BROADCASTING_ID, 0);

		if (tmp != 0)
		{
			Serial.println(tmp);
		}
		else
		{
			if (dxl_return_data != NULL)
			{
				Serial.print("dxl_retun_data[length]    = ");
				Serial.println(dxl_return_data[0], HEX);
				Serial.print("dxl_retun_data[dxl_error] = ");
				Serial.println(dxl_return_data[1], HEX);

				if (dxl_return_data[0] > 2)
				{
					Serial.print("return value: ");
					for (uint8_t i = 0; i < dxl_return_data[0] - 2; i++)
					{
						char msg_1[32];
						sprintf(msg_1, " %i", dxl_return_data[i + 2]);
						Serial.print(msg_1);
					}
				}
				Serial.println();
				Serial.println();
			}
		}

		/*UpdateObjDir();
		handleSIO();
		handleMove();*/

		/* measure cycle time */
		 cycle_count++;
		if (cycle_count >= 1000)
		{
			char msg[64];
			uint32_t tmp = (micros() - cycle_time) / cycle_count;
			sprintf(msg, "Cycle Time : %lu us", tmp);
			SendStatus("StopWatch (loop): ", msg, STATUS_MSG_TYPE_INFO);
			cycle_count = 0;
			cycle_time = micros();
		} 
		delay(1000);
	}
}