/*
 Name:		Scara.ino
 Created:	06.09.2018 14:50:44
 Author:	Jens Schiller
*/


#include "objdir.h"
#include "gpio.h"
#include "status.h"
#include "dynamixel.h"
#include "SimpleModbusSlave.h"
#include "DynamixelSerial2.h"
#include "handler.h"
#include "sio.h"

bool run = false;

void setup() {

	InitSio();
	Serial.println("1");
	InitGPIO();
	Serial.println("2");
	InitOperationMode();
	Serial.println("3");
	InitDynamixel();
	Serial.println("4");
	run = true;
}

void loop() {

	while (run) {
		SystemStatus();
		HandleSIO();
	}
}