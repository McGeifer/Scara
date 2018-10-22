// 
// 
// Setup for serial/ USB connection

#include "Stream.h"

#include "sio.h"
#include "objdir.h"
#include "status.h"

#include "DynamixelSerial2.h"
#include "SimpleModbusSlave.h"
#include "FastCRC.h"

// serial buffer
static const uint8_t bufferLength = 64;
static uint8_t buffer[bufferLength];
static char cBuffer[bufferLength];
static uint8_t ndx;


FastCRC8 CRC8;


void InitSio() {

	// Setup for USB connection;
	Serial.begin(115200);
	Serial.setTimeout(500);

	// Setup for Modbus connection (Serial1)
	modbus_configure(115200, 1, 0, TOTAL_REGS_SIZE_MDB, 0);

	// Setup for Dynamixel connection (Serial2)
	Dynamixel.begin(1000000, 2);
}

// Choose the correct handler to process the received data (selected by operation mode).
void HandleSIO() {

	// get system error state - abort if system is in error state
	int16_t tmp = GetObjStructData(0xFF);

	if (tmp == 0) {

		switch (GetObjStructData(0xFE))	{

		case OP_MODE_MODBUS:
			HandleModbusData();

		case OP_MODE_RAPID:
			HandleRapidString();

		case OP_MODE_SCARA:
			HandleScaraData();

		default:
			// ???
			break;
		}
	}
}

// Receive a Modbus data package via RS232, plausibility check and error response done by the SimpleModbusClient library.
static void HandleModbusData() {

	// function 3 and 16 register array
	static unsigned int holdingRegs[TOTAL_REGS_SIZE_MDB];

	// returns the total error count since the slave started
	holdingRegs[TOTAL_ERRORS_MDB] = modbus_update(holdingRegs);

	// write modbus packet to object
	if (SetObjStructData(holdingRegs[INDEX_MDB], holdingRegs[DATA_MDB]) == 0) {
		ObjStruct *recvP = LocateObj(holdingRegs[INDEX_MDB]);
		
		if (recvP->pFunction != NULL) {
			// execute handler function if available
			recvP->pFunction(&recvP->idx, &recvP->props, &recvP->data);
		}
	}
}

// Receive a Rapid comand string via UART, check the data for plausibility and store it in the object dictionary.
static void HandleRapidString() {
	if (Serial.available() > 0) {

		if (ndx <= bufferLength - 1) {
			cBuffer[ndx] = Serial.read();
			ndx++;

			if (cBuffer[ndx] == ';') {

				char inputString[bufferLength];	
				char* outputString[bufferLength];
				char* part;
				const char delimiter[] = " :=][(),_";
				int i, j = 0;
				bool offsetMode = false;

				cBuffer[ndx + 1] = '\\0'; // terminate string
				strcpy(inputString, cBuffer);	// creat copy of the receiveBuffer to prevent change of data by strtok
				part = strtok(inputString, delimiter);	// split the inputString into multiple tokens

				while (part != NULL) {
					// create tokens
					outputString[i] = part;
					i++;
					part = strtok(NULL, delimiter);
				}

				// for example parse -> CONST robtarget p1:=[[1, 119, 100],,,];
				// outputString	index:	  0		 1	     2	  3	  4	   5
				if (strstr(outputString[0], "CONST")) 
				{
					if (strstr(outputString[1], "robtarget"))
					{
						if (strchr(outputString[2], 'p')) {
							// get point number
							int16_t pointIdx = (int16_t)strtol(outputString[2] + 1, NULL, 10);

							// get offset values
							int16_t posXTmp = (int16_t)strtol(outputString[3], NULL, 10);
							int16_t posYTmp = (int16_t)strtol(outputString[4], NULL, 10);
							int16_t posZTmp = (int16_t)strtol(outputString[5], NULL, 10);

							// set rapidPosReg !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							// set point idx
							
						}
						else 
							SendStatus("in function HandleRapidString(): point index expected", STATUS_TYPE_ERROR);
					}
					else
						SendStatus("in function HandleRapidString(): robtarget expected", STATUS_TYPE_ERROR);
				}

				// switch the magnetic gripper on or off
				// for example parse -> Magnet_on();
				// outputstring index:	  0     1
				else if (strstr(outputString[0], "Magnet"))
				{
					if (strstr(outputString[1], "on")) {
						digitalWrite(MAGNET_PIN, HIGH);
						SendStatus("in function HandleRapidString(): Magnet: on", STATUS_TYPE_INFO);
					}
					else if (strstr(outputString[1], "off")) {
						digitalWrite(MAGNET_PIN, LOW);
						SendStatus("in function HandleRapidString(): Magnet: off", STATUS_TYPE_INFO);
					}
					else {
						SendStatus("in function HandleRapidString(): Magnet_on / off expected", STATUS_TYPE_ERROR);
					}
				}

				// check for MoveC, MoveJ or MoveL
				// for example parse -> MoveJ Offs(p2,   -9,   109,   90), vmax, fine, tool0;
				// or				 -> MoveJ p1, vmax, fine, tool0;
				// outputString	index:	  0		1   2	  3		4	   5	6	   7	8
				else if (strstr(outputString[0], "Move"))
				{
					if (strcmp(outputString[0], "MoveC") == 0)
						SendStatus("in function HandleRapidString(): MoveC not supported", STATUS_TYPE_ERROR);
					
					else if (strcmp(outputString[0], "MoveL") == 0)
						SendStatus("in function HandleRapidString(): MoveL not supported", STATUS_TYPE_ERROR);
					
					else if (strcmp(outputString[0], "MoveJ") == 0) {
						// setzte betriebsart ??? gibt ja momentan eh nur eine
					}
					else
						SendStatus("in function HandleRapidString(): No proper Move{C, L, J} comamnd detected", STATUS_TYPE_ERROR);

					// check for Offs and/ or point
					if (strcmp(outputString[1], "Offs") == 0) {
						offsetMode = true;

						if (strchr(outputString[2], 'p')) {
							int16_t pointIdx = (int16_t)strtol(outputString[2] + 1, NULL, 10);



							// hier gehts weiter !





							if (points[point][3] == 1) {
								j += 2;
								pointMode = true;
								setPoint = point;
							}
							else
							{
								Serial.print("Punkt p");
								Serial.print(point);
								Serial.println(" nicht definiert");
								return 0;
							}
						}
						else
							Serial.println("Punkt erwartet fuer Offset-Bewegung");
					}
					else if (strchr(outputString[j + 1], 'p'))
					{
						long point = strtol(outputString[j + 1] + 1, &tmp, 10);

						if (points[point][3] == 1)
						{
							j -= 2;
							pointMode = true;
							setPoint = point;
#ifdef DEBUG
							Serial.println("Punkt erkannt");
							Serial.print("Punkt bool: ");
							Serial.println(pointMode);
#endif
						}
						else
						{
							Serial.print("Punkt p");
							Serial.print(point);
							Serial.println(" nicht definiert");
							return 0;
						}
					}

					// set temporary position values
					if (offsetMode == true && pointMode == true)
					{
						offsX = strtol(outputString[j + 1], NULL, 0);
						offsY = strtol(outputString[j + 2], NULL, 0);
						offsZ = strtol(outputString[j + 3], NULL, 0);

#ifdef DEBUG
						Serial.print("offsX: ");
						Serial.println(offsX);
						Serial.print("offsY: ");
						Serial.println(offsY);
						Serial.print("offsZ: ");
						Serial.println(offsZ);
#endif
						posXTmp = points[setPoint][xValue] + offsX;
						posYTmp = points[setPoint][yValue] + offsY;
						posZTmp = points[setPoint][zValue] + offsZ;
					}
					else if (!offsetMode && pointMode)
					{
						posXTmp = points[setPoint][xValue];
						posYTmp = points[setPoint][yValue];
						posZTmp = points[setPoint][zValue];
					}
					else if (!offsetMode && !pointMode)
					{
						posXTmp = strtol(outputString[j + 1], NULL, 0);
						posYTmp = strtol(outputString[j + 2], NULL, 0);
						posZTmp = strtol(outputString[j + 3], NULL, 0);

#ifdef DEBUG
						Serial.print("posXTmp: ");
						Serial.println(posXTmp);
						Serial.print("posYTmp: ");
						Serial.println(posYTmp);
						Serial.print("posZTmp: ");
						Serial.println(posZTmp);
#endif 

					}
					else
					{
#ifdef DEBUG
						Serial.println("Fehler - Keine tmp-Werte");
#endif
					}


					// check temporary position values
					// check for x position value
					if (posXTmp >= MIN_COORDINATE_X && posXTmp <= MAX_COORDINATE_X)
					{
						// check for y position value
						if (posYTmp >= MIN_COORDINATE_Y && posYTmp <= MAX_COORDINATE_Y)
						{
							// check for z position value
							if (posZTmp >= MIN_COORDINATE_Z && posZTmp <= MAX_COORDINATE_Z)
							{
								// set position values
								*positionXTarget = DoubleToScaraControl(posXTmp);
								*positionYTarget = DoubleToScaraControl(posYTmp);
								*positionZTarget = posZTmp;

#ifdef DEBUG
								Serial.print("positionXTarget: ");
								Serial.println(*positionXTarget);
								Serial.print("positionYTarget: ");
								Serial.println(*positionYTarget);
								Serial.print("positionZTarget: ");
								Serial.println(*positionZTarget);
#endif
							}
							else
							{
								Serial.println("Z-Koordinate ungueltig!");
								return 0;
							}
						}
						else
						{
							Serial.println("Y-Koordinate ungueltig!");
							return 0;
						}
					}
					else
					{
						Serial.println("X-Koordinate ungueltig!!");
						return 0;
					}


					// check for speed value
					if (strstr(outputString[j + 4], "vmax"))
					{
						*speed_A = 1023;
						*speed_B = 1023;
					}
					else if (strchr(outputString[j + 4], 'v'))
					{
						long speed = strtol(outputString[j + 4] + 1, NULL, 0);

#ifdef DEBUG
						Serial.print("Speed: ");
						Serial.println(speed);
#endif
						if (speed >= MIN_SPEED_Z_AXIS && speed <= MAX_SPEED_Z_AXIS)
							if (speed >= MIN_SPEED_A_B_AXIS && speed <= MAX_SPEED_A_B_AXIS)
							{
								*speed_A = speed;
								*speed_B = speed;
							}
							else
							{
								Serial.println("Geschwindigkeit Achse A/B nicht zulässig");
								return 0;
							}
						else
						{
							Serial.println("Geschwindigkeit Z-Achse nicht zulässig");
							return 0;
						}
					}
					else
					{
						Serial.println("Geschwindigkeit erwartet");
						return 0;
					}


					// check for zone value
					if (strstr(outputString[j + 5], "fine"))
					{
						// mache weiter :)
					}
					else if (strchr(outputString[j + 5], 'z'))
					{
						Serial.println("Ueberschleifen nicht unterstuetzt -> fine");
						return 0;
					}
					else
					{
						Serial.println("fine erwartet");
						return 0;
					}


					// check for tool
					if (strstr(outputString[j + 6], "tool"))
					{
						long tool = strtol(outputString[j + 6] + 4, NULL, 0);

#ifdef DEBUG
						Serial.print("Tool: ");
						Serial.println(tool);
						Serial.print("X: ");
						Serial.println(tools[tool][0]);
						Serial.print("Y: ");
						Serial.println(tools[tool][1]);
						Serial.print("Z: ");
						Serial.println(tools[tool][2]);
						Serial.print("aktiv: ");
						Serial.println(tools[tool][3]);
#endif 

						if (tools[tool][3] == 1)
						{
							posXTmp = ScaraControlToDouble(*positionXTarget) + tools[tool][xOffset];
							posYTmp = ScaraControlToDouble(*positionYTarget) + tools[tool][yOffset];
							posZTmp = *positionZTarget + tools[tool][zOffset];

#ifdef DEBUG
							Serial.print("posXTmp: ");
							Serial.println(posXTmp);
							Serial.print("posYTmp: ");
							Serial.println(posYTmp);
							Serial.print("posZTmp: ");
							Serial.println(posZTmp);
#endif 
							// check for x position value
							if (posXTmp >= MIN_COORDINATE_X && posXTmp <= MAX_COORDINATE_X)
							{
								// check for y position value
								if (posYTmp >= MIN_COORDINATE_Y && posYTmp <= MAX_COORDINATE_Y)
								{
									// check for z position value
									if (posZTmp >= MIN_COORDINATE_Z && posZTmp <= MAX_COORDINATE_Z)
									{
										// set tool offset
										*positionXTarget = DoubleToScaraControl(ScaraControlToDouble(*positionXTarget) + tools[tool][xOffset]);
										*positionYTarget = DoubleToScaraControl(ScaraControlToDouble(*positionYTarget) + tools[tool][yOffset]);
										*positionZTarget = *positionZTarget + tools[tool][zOffset];

										// werte mit der Calculate Angle überprüfen !?
										Serial.println("Befehl akzeptiert");
										return 1;
									}
									else
									{
										Serial.println("Werkzeug Offset Z nicht zulässig");
										return 0;
									}
								}
								else
								{
									Serial.println("Werkzeug Offset Y nicht zulässig");
									return 0;
								}
							}
							else
							{
								Serial.println("Werkzeug Offset X nicht zulässig");
								return 0;
							}
						}
						else
						{
							Serial.println("Werkzeug nicht aktiv");
							return 0;
						}
					}
					else
					{
						return 0;
					}
				}
				else
					SendStatus("in function HandleRapidString(): Move{C, L, J} command expected", STATUS_TYPE_ERROR);
			}
		}
		else {
			memset(cBuffer, 0, ndx);
			ndx = 0;
			SendStatus("in function HandleScaraData(): input string to long", STATUS_TYPE_ERROR);
		}
	}
}

// Receive a Scara data package via UART, check the data package and store it in the object dictionary.
static void HandleScaraData() {

	if (Serial.available() > 0) {

		if (ndx <= bufferLength) {
			buffer[ndx] = Serial.read();
			ndx++;

			if (ndx >= SCARA_PACKET_LENGTH) {

				for (uint8_t i = 0; i < (bufferLength - 3); i++) {

					// check crc for packet
					uint8_t crc = CRC8.smbus(&buffer[ndx - SCARA_PACKET_LENGTH], 3);

					if (crc == buffer[ndx - 1]) {
						uint8_t recvIndex = buffer[ndx - SCARA_PACKET_LENGTH + 1];
						uint16_t recvData = (buffer[ndx - SCARA_PACKET_LENGTH + 2] << 8) | buffer[ndx - SCARA_PACKET_LENGTH + 3];

						// save data in object
						if (SetObjStructData(recvIndex, recvData) == 0) {
							ObjStruct *recvP = LocateObj(recvIndex);
							memset(buffer, 0, ndx);
							ndx = 0;

							if (recvP->pFunction != NULL) {
								// execute handler function if available
								recvP->pFunction(&recvP->idx, &recvP->props, &recvP->data);
							}
						}
					}
					else
						SendStatus("in function HandleScaraData(): crc checksum invalid", STATUS_TYPE_WARNING);
				}
			}
		}
		else {
			memset(buffer, 0, ndx);
			ndx = 0;
			SendStatus("in function HandleScaraData(): input data to long", STATUS_TYPE_ERROR);
		}
	}
}