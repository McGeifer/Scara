/*
 Name:		DynaPacketHandler.ino
 Created:	14.02.2019 09:03:04
 Author:	me
*/

#include "Stream.h"

#define DIRECTIO_PIN		2
#define TX_MODE				HIGH
#define RX_MODE				LOW

#define AX_START			0xFF
#define AX_ID				0x00
#define AX_LENGTH			0x02 
#define AX_PING				0x01

#define AX_TIMEOUT			300

uint32_t cycle_time;
uint32_t cycle_count;
uint32_t cnt;
uint32_t cnt_offs;
uint8_t a, b, c, d, e, f;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(1000000);
	Serial2.begin(1000000);
	pinMode(DIRECTIO_PIN, OUTPUT);
	delay(1000);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	//StartStopwatch();
	int8_t ret = SendData();
	//StopStopwatch();
	if (ret != 0)
	{
		Serial.print("return val: ");
		Serial.println(ret);
		Serial.flush();
	}
}

int8_t SendData()
{
	//StartStopwatch();
	digitalWrite(2, TX_MODE);

	Serial2.write(AX_START);
	Serial2.write(AX_START);
	Serial2.write(AX_ID);
	Serial2.write(AX_LENGTH);
	Serial2.write(AX_PING);
	Serial2.write((uint8_t)(~(AX_ID + AX_LENGTH + AX_PING)) & 0xFF);
	Serial2.flush(); /* make sure data has been send */
	//StopStopwatch();

	digitalWrite(2, RX_MODE);
	return ReadError();
}

int8_t ReadError()
{
	/* micros will overflow after approximately 70 minutes, this may cause trouble */
	uint32_t timeout = micros() + AX_TIMEOUT;

	//StartStopwatch();
	while (micros() < timeout && Serial2.available() < 6) {} /* wait for data until timeout */
	timeout = micros() + AX_TIMEOUT;
	//StopStopwatch();

	//StartStopwatch();
	if (micros() >= timeout && Serial2.available() > 0)
	{
		while (Serial2.available() > 0)
		{
			Serial.print(Serial2.read(), HEX);
			Serial.flush();
		}
		return -1; /* timeout with data */
	}
	else if (micros() >= timeout && Serial2.available() == 0)
	{
		return -2; /* timeout without data */
	}
	else
	{
		while (Serial2.available() > 0)
		{
			if (Serial2.peek() == AX_START)
			{
				a = Serial2.read();
				/*Serial.print(a, HEX);
				Serial.print(" ");*/

				if (Serial2.peek() == AX_START) /* valid packet start? */
				{
					//StartStopwatch();
					b = Serial2.read();
					c = Serial2.read();
					d = Serial2.read();
					e = Serial2.read();
					f = Serial2.read();
					//StopStopwatch();

					while (Serial2.available() > 0) /* make sure buffer is empty */
					{
						Serial.print(Serial2.read(), HEX);
						Serial.print(" ");
						Serial.flush();
					}
					/*Serial.print(a, HEX);
					Serial.print(b, HEX);
					Serial.print(c, HEX);
					Serial.print(d, HEX);
					Serial.print(e, HEX);
					Serial.print(f, HEX);
					Serial.println();*/

					return e;
				}
				else
				{
					while (Serial2.available() > 0) /* no proper Dynamixel packet start (0xFF 0xFF) - clear buffer to prevent unprocessed data in buffer*/
					{
						Serial2.read();
					}
				}
			}
			Serial2.read(); /* get next byte */
		}
	}
	//StopStopwatch();

	while (Serial2.available() > 0)
	{
		Serial2.read();
	}
	return -3; /* invalid data packet */
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