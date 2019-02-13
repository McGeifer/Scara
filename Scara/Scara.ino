/*
 *  Name:		Scara.ino
 *  Created:	06.09.2018 14:50:44
 *  Author:		Jens Schiller
 */

#include "SimpleModbusSlave.h"
#include "DynamixelSerial2.h"

#include "objdir.h"
#include "gpio.h"
#include "status.h"
#include "sio.h"
#include "calc.h"

uint32_t cycleTime;
uint32_t cycleCount;

extern void InitDynamixel(void);
extern void UpdateObjDir(void);
extern void HandleMove(void);

void setup()
{
	InitGPIO();
	InitSio();
	InitOperationMode();
	InitDynamixel();
	
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
		//SendStatus(NULL, "loop", STATUS_TYPE_DEBUG);
		//UpdateObjDir();
		//HandleSIO();
		//HandleMove();

		//delay(2000);

		/* measure cycle time */
		cycleCount++;
		if (cycleCount >= 1000)
		{
			char msg[64];
			uint32_t tmp = (micros() - cycleTime) / cycleCount;
			sprintf(msg, "Cycle Time : %lu us", tmp);
			SendStatus("StopWatch (loop): ", msg, STATUS_TYPE_INFO);
			/*Serial.print("StopWatch (loop): ");
			Serial.print(tmp);
			Serial.println(" us");*/
			cycleCount = 0;
			cycleTime = micros();
		}
	}
}