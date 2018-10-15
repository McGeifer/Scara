/*
 Name:		Scara.ino
 Created:	06.09.2018 14:50:44
 Author:	Jens Schiller
*/

#include "gpio.h"
#include "status.h"
#include "dynamixel.h"
#include "SimpleModbusSlave.h"
#include "DynamixelSerial2.h"
#include "handler.h"
#include "objdir.h"
#include "sio.h"

void setup() {

	InitSio();
	InitGPIO();
	InitDynamixel();
}

void loop() {

	HandleSio();
	delay(1000);
}

