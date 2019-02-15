/*
 Name:		ZeitmessungDynamixel.ino
 Created:	15.02.2019 10:28:07
 Author:	Schilly
*/

#include <DynamixelSerial2.h>
#include "Stream.h"

#define DIRECTIO_PIN		2
#define TX_MODE				HIGH
#define RX_MODE				LOW

#define AX_RETURN_DELAY		500

#define AX_START			0xFF
#define AX_ID				0x00

#define AX_PING				0x01
#define AX_LENGTH			0x02


uint32_t cnt;
uint32_t cnt_offs;
uint8_t a, b, c, d, e, f;
uint16_t time_cnt;
const uint16_t timeout = 10;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(1000000);
	Serial2.begin(1000000);
	pinMode(DIRECTIO_PIN, OUTPUT);
	digitalWrite(2, TX_MODE);

	delay(500);
	Serial2.write(0xFF);
	Serial2.write(0xFF);
	Serial2.write(0x00);
	Serial2.write(0x04);
	Serial2.write(0x03);
	Serial2.write(0x05);
	Serial2.write(0x01);
	Serial2.write((uint8_t)(~(0x00 + 0x04 + 0x03 + 0x05 + 0x01)) & 0xFF);
	Serial2.flush();
	digitalWrite(2, RX_MODE);
	while (Serial2.available() < 5) {}
	Serial2.read();
	Serial2.read();
	Serial2.read();
	Serial2.read();
	uint8_t tmp = Serial2.read();
	Serial2.read();

	Serial.println(tmp);
	delay(2000);
}


// the loop function runs over and over again until power down or reset
void loop() {
  
	StartStopwatch();
	digitalWrite(2, TX_MODE);
	Serial2.write(AX_START);
	Serial2.write(AX_START);
	Serial2.write(AX_ID);
	Serial2.write(AX_LENGTH);
	Serial2.write(AX_PING);
	Serial2.write((uint8_t)(~(AX_ID + AX_LENGTH + AX_PING)) & 0xFF);
	Serial2.flush(); /* make sure data has been send */
	digitalWrite(2, RX_MODE);
	//StopStopwatch();

	//StartStopwatch();
	while (Serial2.available() < 5 && time_cnt < timeout)
	{
		time_cnt++;
		delayMicroseconds(10);
	}
	//StopStopwatch();





	//StartStopwatch();
	a = Serial2.read();
	b = Serial2.read();
	c = Serial2.read();
	d = Serial2.read();
	e = Serial2.read();
	f = Serial2.read();
	StopStopwatch();

	Serial.print(a, HEX);
	Serial.print(b, HEX);
	Serial.print(c, HEX);
	Serial.print(d, HEX);
	Serial.print(e, HEX);
	Serial.print(f, HEX);
	Serial.println();
}

void StartStopwatch(void)
{
	cnt_offs = micros();
}

void StopStopwatch(void)
{
	cnt = micros();
	//Serial.print("time: ");
	Serial.println(cnt - cnt_offs);
	//Serial.println(" us");
}
