/*
 Name:		DynaDetect.ino
 Created:	09.04.2019 14:03:57
 Author:	Jens Schiller

 Testfunktion um nach Dynamixel Servos zu suchen, das Status Paket zu empfangen und dessen Inhalt als Konsolen-Ausdruck anzuzeigen.
*/

#include <DynamixelSerial2.h>

uint8_t i = 0;
uint32_t time_stamp = 0;
uint32_t timeout = 0;

void setup() {

	pinMode(2, OUTPUT);			/* Pin für tristate buffer */
	Serial.begin(115200);		/* Serial Port für Console */
	Serial2.begin(1000000);		/* Serial Port für Dynamixel*/
}

// the loop function runs over and over again until power down or reset
void loop() {
  
	digitalWrite(2, HIGH);		/* TX Modus */
	Serial2.write(0xFF);
	Serial2.write(0xFF);
	Serial2.write(i);
	Serial2.write(0x02);
	Serial2.write(0x01);
	Serial2.write( (~(i + 0x02 + 0x01)) & 0xFF);
	Serial2.flush();
	digitalWrite(2, LOW);		/* RX Modus */

	timeout = micros() + 500;	/* 500 µs Timeout */
	
	while (micros() < timeout && Serial.available() < 6) {}	/* Warte bis richtige Länge des Status Pakets (6) empfangen oder Timeout erreicht wurde. */
	time_stamp = micros();

	if (time_stamp < timeout) {
		Serial.print("Dynamixel @ ID ");
		Serial.println(i, HEX);
		Serial.print("data received: ");
		
		while (Serial2.available()) {
			Serial.print(Serial2.read(), HEX);
			Serial.print(" ");
		}
		Serial.println();
		Serial.println();
	}
	
	i++;
	if (i >= 254) {		/* Stop, wenn ID-Bereich einmal durchlaufen. ID-Bereich = 0 - 253; 254 = Broadcast ID */
		for (;;) {}
	}
}
