/*
 Name:		Scara.ino
 Created:	06.09.2018 14:50:44
 Author:	Jens Schiller
*/

#include "handler.h"
#include "objdir.h"
#include "DynamixelSerial2.h"
#include "sio.h"

void setup(){

	InitSio();
	//InitGPOI();
}

void loop() {

	HandleSio();
	delay(1000);
}