/*
 Name:		Scara.ino
 Created:	06.09.2018 14:50:44
 Author:	Jens Schiller
*/

#include "SimpleModbusSlave.h"
#include "DynamixelSerial2.h"

#include "objdir.h"
#include "gpio.h"
#include "status.h"
#include "dynamixel.h"
#include "sio.h"
#include "calc.h"

bool run = false;

void setup() {

	InitSio();
	InitGPIO();
	InitOperationMode();
	InitDynamixel();
	run = true;

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

	uint16_t xPos = 3066;
	int16_t yPos = -649;

	CalcAngle(xPos, yPos);
}

void loop() {

	while (run) {
		//SystemStatus();
		HandleSIO();
		//HandleMove();
	}
}