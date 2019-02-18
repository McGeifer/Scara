/*
 Dynamixel.cpp - Ax-12+ Half Duplex USART Communication
 Copyright (c) 2011 Savage Electronics.
 Created by Savage on 27/01/11.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,  
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 *****************************************************************************
 Modifications:
 
 25/07/2011 - Eliminado la modificacion serial para ser modificada dentro del mismo Hardware Serial.
 25/07/2011 - Modificado la funcion SetBD() para aceptar todas la velocidades sin PDF.
 25/07/2011 - Agregada la funcion de Rotacion Continua.
 26/07/2011 - Agregada la funcion begin sin Seteo de Direction_Pin.
 25/07/2011 - Agregada la funcion ReSet.
 26/07/2011 - Agregada la funcion Reg_Write en move y moveSpeed.
 26/07/2011 - Agregada la funcion Action.
 13/12/2011 - Arreglado el manejo y envio de variables.
 22/12/2011 - Compatible con la actualizacion Arduino 1.0.
 10/01/2012 - Utilizacion de Macros y eliminacion codigo no necesario.
 11/01/2012 - Agregadas las funciones:
              int16_t SetTempLimit(uint8_t ID, uint8_t Temperature);
              int16_t SetAngleLimit(uint8_t ID, int16_t CWLimit, int16_t CCWLimit);
              int16_t SetVoltageLimit(uint8_t ID, uint8_t DVoltage, uint8_t UVoltage);
			  int16_t SetMaxTorque(uint8_t ID, int16_t MaxTorque);
              int16_t SetSRL(uint8_t ID, uint8_t SRL);
              int16_t SetRDT(uint8_t ID, uint8_t RDT);
              int16_t SetLEDAlarm(uint8_t ID, uint8_t LEDAlarm);
              int16_t SetShutdownAlarm(uint8_t ID, uint8_t SALARM);
              int16_t SetCMargin(uint8_t ID, uint8_t CWCMargin, uint8_t CCWCMargin);
			  int16_t SetCSlope(uint8_t ID, uint8_t CWCSlope, uint8_t CCWCSlope);
 15/01/2012 - Agregadas las funciones:             
              int16_t SetPunch(uint8_t ID, int16_t Punch);
              int16_t moving(uint8_t ID);
              int16_t lockRegister(uint8_t ID);
			  int16_t RWStatus(uint8_t ID);
              int16_t readSpeed(uint8_t ID);
              int16_t readLoad(uint8_t ID);
 
 TODO:
 
 FUNCION SYNCWRITE.
 
 *****************************************************************************
 
 Contact: savageelectronics@gmail.com 
 Web:     http://savageelectrtonics.blogspot.com/
 Autor:   Josue Alejandro Savage
 
 */

#if defined(ARDUINO) && ARDUINO >= 100  // Arduino IDE Version
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DynamixelSerial2.h"
#include "gpio.h"

// Macro for the selection of the Serial Port

#define sendData(args)  (Serial2.write(args))    // Write Over Serial
#define availableData() (Serial2.available())    // Check Serial Data Available
#define readData()      (Serial2.read())         // Read Serial Data
#define peekData()      (Serial2.peek())         // Peek Serial Data
#define beginCom(args)  (Serial2.begin(args))    // Begin Serial Communication
#define endCom()        (Serial2.end())          // End Serial Communication

// Macro for Timing

#define delayus(args) (delayMicroseconds(args))  // Delay Microseconds

// Macro for Communication Flow Control

#define SetDPin(DirPin,Mode)   (pinMode(DirPin,Mode))       // Select the Switch to TX/RX Mode Pin
#define switchCom(DirPin,Mode) (digitalWrite(DirPin,Mode))  // Switch to TX/RX Mode

uint8_t Checksum;
uint8_t Direction_Pin;
uint8_t Time_Counter;
uint8_t Incoming_Byte;
uint8_t Position_High_Byte;
uint8_t Position_Low_Byte;
uint8_t Speed_High_Byte;
uint8_t Speed_Low_Byte;
uint8_t Load_High_Byte;
uint8_t Load_Low_Byte;

int16_t Moving_Byte;
int16_t RWS_Byte;
int16_t Speed_Long_Byte;
int16_t Load_Long_Byte;
int16_t Position_Long_Byte;
int16_t Temperature_Byte;
int16_t Voltage_Byte;
int16_t Error_Byte;

int16_t readError(void)
{
	Time_Counter = 0;
	while((availableData() < 5) && (Time_Counter < TIME_OUT))  // Wait for Data
	{
		Time_Counter++;
		delayus(1000);
	}
	
	while (availableData() > 0)
	{
		Incoming_Byte = readData();
		if ( (Incoming_Byte == 255) && (peekData() == 255) )
		{
			readData();                                    // Start Bytes
			readData();                                    // Ax-12 ID
			readData();                                    // Length
			Error_Byte = readData();                       // Error
			return (Error_Byte * (-1));						// Negation of the error byte to distinguish normal data packets 
		}
	}
	return (-128);										// No Ax Response
}

void dynamixelBegin(int32_t baud, uint8_t directionPin)
{	
	Direction_Pin = directionPin;
	SetDPin(Direction_Pin,OUTPUT);
	beginCom(baud);
}	

//void dynamixelBegin(int32_t baud)
//{	
//	beginCom(baud);
//}	

void dynamixelEnd()
{
	endCom();
}

int16_t dynamixelReset(uint8_t ID)
{
	Checksum = (~(ID + AX_RESET_LENGTH + AX_RESET))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_RESET_LENGTH);
	sendData(AX_RESET);    
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);

    return (readError());  
}

int16_t dynamixelPing(uint8_t ID)
{
	Checksum = (~(ID + AX_READ_DATA + AX_PING))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_READ_DATA);
	sendData(AX_PING);    
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);

    return (readError());              
}

int16_t dynamixelSetID(uint8_t ID, uint8_t newID)
{    
	Checksum = (~(ID + AX_ID_LENGTH + AX_WRITE_DATA + AX_ID + newID))&0xFF;

	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
	sendData(AX_ID_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_ID);
    sendData(newID);
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
    
    return (readError());                // Return the read error
}

int16_t dynamixelSetBD(uint8_t ID, int32_t baud)
{    
	uint8_t Baud_Rate = (2000000/baud) - 1;
    Checksum = (~(ID + AX_BD_LENGTH + AX_WRITE_DATA + AX_BAUD_RATE + Baud_Rate))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                 // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
	sendData(AX_BD_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_BAUD_RATE);
    sendData(Baud_Rate);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
    
    return (readError());                // Return the read error
}

int16_t dynamixelMove(uint8_t ID, int16_t Position)
{
    int8_t Position_H,Position_L;
    Position_H = Position >> 8;           // 16 bits - 2 x 8 bits variables
    Position_L = Position;
	Checksum = (~(ID + AX_GOAL_LENGTH + AX_WRITE_DATA + AX_GOAL_POSITION_L + Position_L + Position_H))&0xFF;
    
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                 // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_GOAL_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_GOAL_POSITION_L);
    sendData(Position_L);
    sendData(Position_H);
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);

    return (readError());                 // Return the read error
}

int16_t dynamixelMoveSpeed(uint8_t ID, int16_t Position, int16_t Speed)
{
    int8_t Position_H,Position_L,Speed_H,Speed_L;
    Position_H = Position >> 8;    
    Position_L = Position;                // 16 bits - 2 x 8 bits variables
    Speed_H = Speed >> 8;
    Speed_L = Speed;                      // 16 bits - 2 x 8 bits variables
	Checksum = (~(ID + AX_GOAL_SP_LENGTH + AX_WRITE_DATA + AX_GOAL_POSITION_L + Position_L + Position_H + Speed_L + Speed_H))&0xFF;
 
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_GOAL_SP_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_GOAL_POSITION_L);
    sendData(Position_L);
    sendData(Position_H);
    sendData(Speed_L);
    sendData(Speed_H);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
    
    return (readError());               // Return the read error
}

int16_t dynamixelSetEndless(uint8_t ID, bool Status)
{
 if ( Status )
 {	
	  int8_t AX_CCW_AL_LT = 0;     // Changing the CCW Angle Limits for Full Rotation.
	  Checksum = (~(ID + AX_GOAL_LENGTH + AX_WRITE_DATA + AX_CCW_ANGLE_LIMIT_L))&0xFF;
	
	  switchCom(Direction_Pin,Tx_MODE);
      sendData(AX_START);                // Send Instructions over Serial
      sendData(AX_START);
      sendData(ID);
      sendData(AX_GOAL_LENGTH);
      sendData(AX_WRITE_DATA);
      sendData(AX_CCW_ANGLE_LIMIT_L );
      sendData(AX_CCW_AL_LT);
      sendData(AX_CCW_AL_LT);
      sendData(Checksum);
      delayus(TX_DELAY_TIME);
	  switchCom(Direction_Pin,Rx_MODE);

	  return(readError());
 }
 else
 {
	 dynamixelTurn(ID,0,0);
	 Checksum = (~(ID + AX_GOAL_LENGTH + AX_WRITE_DATA + AX_CCW_ANGLE_LIMIT_L + AX_CCW_AL_L + AX_CCW_AL_H))&0xFF;
	
	 switchCom(Direction_Pin,Tx_MODE);
	 sendData(AX_START);                 // Send Instructions over Serial
	 sendData(AX_START);
	 sendData(ID);
	 sendData(AX_GOAL_LENGTH);
	 sendData(AX_WRITE_DATA);
	 sendData(AX_CCW_ANGLE_LIMIT_L);
	 sendData(AX_CCW_AL_L);
	 sendData(AX_CCW_AL_H);
	 sendData(Checksum);
	 delayus(TX_DELAY_TIME);
	 switchCom(Direction_Pin,Rx_MODE);
	 
	 return (readError());                 // Return the read error
  }
 } 

int16_t dynamixelTurn(uint8_t ID, bool SIDE, int16_t Speed)
{		
	if (SIDE == 0) // Move Left///////////////////////////
	{                          
		int8_t Speed_H,Speed_L;
		Speed_H = Speed >> 8;
		Speed_L = Speed;                     // 16 bits - 2 x 8 bits variables
		Checksum = (~(ID + AX_SPEED_LENGTH + AX_WRITE_DATA + AX_GOAL_SPEED_L + Speed_L + Speed_H))&0xFF;
			
		switchCom(Direction_Pin,Tx_MODE);
		sendData(AX_START);                // Send Instructions over Serial
		sendData(AX_START);
		sendData(ID);
		sendData(AX_SPEED_LENGTH);
		sendData(AX_WRITE_DATA);
		sendData(AX_GOAL_SPEED_L);
		sendData(Speed_L);
		sendData(Speed_H);
		sendData(Checksum);
		delayus(TX_DELAY_TIME);
		switchCom(Direction_Pin,Rx_MODE);
			
		return(readError());               // Return the read error		
	}
	else
	{                                            // Move Right////////////////////
		int8_t Speed_H,Speed_L;
		Speed_H = (Speed >> 8) + 4;
		Speed_L = Speed;                     // 16 bits - 2 x 8 bits variables
		Checksum = (~(ID + AX_SPEED_LENGTH + AX_WRITE_DATA + AX_GOAL_SPEED_L + Speed_L + Speed_H))&0xFF;
			
		switchCom(Direction_Pin,Tx_MODE);
		sendData(AX_START);                // Send Instructions over Serial
		sendData(AX_START);
		sendData(ID);
		sendData(AX_SPEED_LENGTH);
		sendData(AX_WRITE_DATA);
		sendData(AX_GOAL_SPEED_L);
		sendData(Speed_L);
		sendData(Speed_H);
		sendData(Checksum);
		delayus(TX_DELAY_TIME);
		switchCom(Direction_Pin,Rx_MODE);
			
		return(readError());                // Return the read error	
	}
}

int16_t dynamixelMoveRW(uint8_t ID, int16_t Position)
{
    int8_t Position_H,Position_L;
    Position_H = Position >> 8;           // 16 bits - 2 x 8 bits variables
    Position_L = Position;
    Checksum = (~(ID + AX_GOAL_LENGTH + AX_REG_WRITE + AX_GOAL_POSITION_L + Position_L + Position_H))&0xFF;

	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                 // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_GOAL_LENGTH);
    sendData(AX_REG_WRITE);
    sendData(AX_GOAL_POSITION_L);
    sendData(Position_L);
    sendData(Position_H);
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    return (readError());                 // Return the read error
}

int16_t dynamixelMoveSpeedRW(uint8_t ID, int16_t Position, int16_t Speed)
{
    int8_t Position_H,Position_L,Speed_H,Speed_L;
    Position_H = Position >> 8;    
    Position_L = Position;                // 16 bits - 2 x 8 bits variables
    Speed_H = Speed >> 8;
    Speed_L = Speed;                      // 16 bits - 2 x 8 bits variables
    Checksum = (~(ID + AX_GOAL_SP_LENGTH + AX_REG_WRITE + AX_GOAL_POSITION_L + Position_L + Position_H + Speed_L + Speed_H))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_GOAL_SP_LENGTH);
    sendData(AX_REG_WRITE);
    sendData(AX_GOAL_POSITION_L);
    sendData(Position_L);
    sendData(Position_H);
    sendData(Speed_L);
    sendData(Speed_H);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
    
    return (readError());               // Return the read error
}

void dynamixelAction()
{	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(BROADCAST_ID);
    sendData(AX_ACTION_LENGTH);
    sendData(AX_ACTION);
    sendData(AX_ACTION_CHECKSUM);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
}

int16_t dynamixelTorqueStatus( uint8_t ID, bool Status)
{
    Checksum = (~(ID + AX_TORQUE_LENGTH + AX_WRITE_DATA + AX_TORQUE_ENABLE + Status))&0xFF;

	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);              // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_TORQUE_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_TORQUE_ENABLE);
    sendData(Status);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
    
    return (readError());              // Return the read error
}

int16_t dynamixelLedStatus(uint8_t ID, bool Status)
{    
    Checksum = (~(ID + AX_LED_LENGTH + AX_WRITE_DATA + AX_LED + Status))&0xFF;

	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);              // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_LED_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_LED);
    sendData(Status);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);

    return (readError());              // Return the read error
}

int16_t dynamixelReadTemperature(uint8_t ID)
{	
    Checksum = (~(ID + AX_TEM_LENGTH  + AX_READ_DATA + AX_PRESENT_TEMPERATURE + AX_BYTE_READ))&0xFF;
    
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);
    sendData(AX_START);
    sendData(ID);
    sendData(AX_TEM_LENGTH);
    sendData(AX_READ_DATA);
    sendData(AX_PRESENT_TEMPERATURE);
    sendData(AX_BYTE_READ);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    Temperature_Byte = -1;
    Time_Counter = 0;
    while((availableData() < 6) & (Time_Counter < TIME_OUT))
	{
		Time_Counter++;
		delayus(1000);
    }
	
    while (availableData() > 0)
	{
		Incoming_Byte = readData();
		if ( (Incoming_Byte == 255) & (peekData() == 255) )
		{
			readData();                            // Start Bytes
			readData();                            // Ax-12 ID
			readData();                            // Length
			if( (Error_Byte = readData()) != 0 )   // Error
				return (Error_Byte*(-1));
			Temperature_Byte = readData();         // Temperature
		}
    }
	return (Temperature_Byte);               // Returns the read temperature
}

int16_t dynamixelReadPosition(uint8_t ID)
{	
    Checksum = (~(ID + AX_POS_LENGTH  + AX_READ_DATA + AX_PRESENT_POSITION_L + AX_BYTE_READ_POS))&0xFF;
  
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);
    sendData(AX_START);
    sendData(ID);
    sendData(AX_POS_LENGTH);
    sendData(AX_READ_DATA);
    sendData(AX_PRESENT_POSITION_L);
    sendData(AX_BYTE_READ_POS);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);

	while (1)
	{
		if (availableData() > 0)
		{
			Serial.println(readData());
		}
	}

	
    Position_Long_Byte = -1;
	Time_Counter = 0;

	//while (Time_Counter < TIME_OUT)
	//{
	//	if (availableData() >= 7)
	//	{
	//		Incoming_Byte = readData();
	//		if ((Incoming_Byte == AX_START) & (peekData() == AX_START))
	//		{
	//			readData();                            // Start Bytes
	//			readData();                            // Ax-12 ID
	//			readData();                            // Length
	//			if ((Error_Byte = readData()) != 0)   // Error
	//				return (Error_Byte*(-1));

	//			Position_Low_Byte = readData();            // Position Bytes
	//			Position_High_Byte = readData();
	//			Position_Long_Byte = Position_High_Byte << 8;
	//			Position_Long_Byte = Position_Long_Byte + Position_Low_Byte;
	//			return (Position_Long_Byte);     // Returns the read position
	//		}
	//	}
	//	else
	//	{
	//		Time_Counter++;
	//		delayus(500);
	//	}
	//}
	//return -128;
    while((availableData() < 7) & (Time_Counter < TIME_OUT))
	{
		Time_Counter++;
		delayus(1000);
    }
    while (availableData() > 0 )
	{
		Incoming_Byte = readData();
		if ( (Incoming_Byte == AX_START) & (peekData() == AX_START) )
		{
			readData();                            // Start Bytes
			readData();                            // Ax-12 ID
			readData();                            // Length
			if( (Error_Byte = readData()) != 0 )   // Error
				return (Error_Byte*(-1));
    
			Position_Low_Byte = readData();            // Position Bytes
			Position_High_Byte = readData();
			Position_Long_Byte = Position_High_Byte << 8; 
			Position_Long_Byte = Position_Long_Byte + Position_Low_Byte;
		}
    }
	return (Position_Long_Byte);     // Returns the read position
}

int16_t dynamixelReadVoltage(uint8_t ID)
{    
    Checksum = (~(ID + AX_VOLT_LENGTH  + AX_READ_DATA + AX_PRESENT_VOLTAGE + AX_BYTE_READ))&0xFF;
    
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);
    sendData(AX_START);
    sendData(ID);
    sendData(AX_VOLT_LENGTH);
    sendData(AX_READ_DATA);
    sendData(AX_PRESENT_VOLTAGE);
    sendData(AX_BYTE_READ);
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    Voltage_Byte = -1;
	Time_Counter = 0;
    while((availableData() < 6) & (Time_Counter < TIME_OUT))
	{
		Time_Counter++;
		delayus(1000);
    }
	
    while (availableData() > 0){
		Incoming_Byte = readData();
		if ( (Incoming_Byte == 255) & (peekData() == 255) )
		{
			readData();                            // Start Bytes
			readData();                            // Ax-12 ID
			readData();                            // Length
			if( (Error_Byte = readData()) != 0 )   // Error
				return (Error_Byte*(-1));
			Voltage_Byte = readData();             // Voltage
		}
    }
	return (Voltage_Byte);               // Returns the read Voltage
}

int16_t dynamixelSetTempLimit(uint8_t ID, int16_t Temperature)
{
	Checksum = (~(ID + AX_TL_LENGTH +AX_WRITE_DATA+ AX_LIMIT_TEMPERATURE + Temperature))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_TL_LENGTH);
	sendData(AX_WRITE_DATA);
	sendData(AX_LIMIT_TEMPERATURE);
    sendData(Temperature);
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    return (readError()); 
}

int16_t dynamixelSetLowVoltageLimit(uint8_t ID, int16_t DVoltage)
{
	Checksum = (~(ID + AX_VL_LENGTH + AX_WRITE_DATA + AX_DOWN_LIMIT_VOLTAGE + DVoltage)) & 0xFF;

	switchCom(Direction_Pin, Tx_MODE);
	sendData(AX_START);
	sendData(AX_START);
	sendData(ID);
	sendData(AX_VL_LENGTH);
	sendData(AX_WRITE_DATA);
	sendData(AX_DOWN_LIMIT_VOLTAGE);
	sendData(DVoltage);
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin, Rx_MODE);

	return (readError());
}

int16_t dynamixelSetHighVoltageLimit(uint8_t ID, int16_t UVoltage)
{
	Checksum = (~(ID + AX_VL_LENGTH +AX_WRITE_DATA+ AX_DOWN_LIMIT_VOLTAGE + UVoltage))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_VL_LENGTH);
	sendData(AX_WRITE_DATA);
	sendData(AX_UP_LIMIT_VOLTAGE);
    sendData(UVoltage);
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    return (readError()); 
}

int16_t dynamixelSetCWAngleLimit(uint8_t ID, int16_t CWLimit)
{
	int8_t CW_H,CW_L;
    CW_H = CWLimit >> 8;    
    CW_L = CWLimit;                // 16 bits - 2 x 8 bits variables
	Checksum = (~(ID + AX_VL_LENGTH +AX_WRITE_DATA+ AX_CW_ANGLE_LIMIT_L + CW_H + CW_L))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_CCW_CW_LENGTH);
	sendData(AX_WRITE_DATA);
	sendData(AX_CW_ANGLE_LIMIT_L);
    sendData(CW_L);
	sendData(CW_H);
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    return (readError()); 
}

int16_t dynamixelSetCCWAngleLimit(uint8_t ID, int16_t CCWLimit)
{
	int8_t CCW_H, CCW_L;
	CCW_H = CCWLimit >> 8;
	CCW_L = CCWLimit;			// 16 bits - 2 x 8 bits variables
	Checksum = (~(ID + AX_VL_LENGTH + AX_WRITE_DATA + AX_CCW_ANGLE_LIMIT_L + CCW_H + CCW_L)) & 0xFF;

	switchCom(Direction_Pin, Tx_MODE);
	sendData(AX_START);
	sendData(AX_START);
	sendData(ID);
	sendData(AX_CCW_CW_LENGTH);
	sendData(AX_WRITE_DATA);
	sendData(AX_CCW_ANGLE_LIMIT_L);
	sendData(CCW_L);
	sendData(CCW_H);
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin, Rx_MODE);

	return (readError());
}

int16_t dynamixelSetMaxTorque(uint8_t ID, int16_t MaxTorque)
{
	int8_t MaxTorque_H,MaxTorque_L;
    MaxTorque_H = MaxTorque >> 8;           // 16 bits - 2 x 8 bits variables
    MaxTorque_L = MaxTorque;
	Checksum = (~(ID + AX_MT_LENGTH + AX_WRITE_DATA + AX_MAX_TORQUE_L + MaxTorque_L + MaxTorque_H))&0xFF;
    
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                 // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_MT_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_MAX_TORQUE_L);
    sendData(MaxTorque_L);
    sendData(MaxTorque_H);
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    return (readError());                 // Return the read error
}

int16_t dynamixelSetSRL(uint8_t ID, int16_t SRL)
{    
	Checksum = (~(ID + AX_SRL_LENGTH + AX_WRITE_DATA + AX_RETURN_LEVEL + SRL))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
	sendData(AX_SRL_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_RETURN_LEVEL);
    sendData(SRL);
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
    
    return (readError());                // Return the read error
}

int16_t dynamixelSetRDT(uint8_t ID, int16_t RDT)
{    
	Checksum = (~(ID + AX_RDT_LENGTH + AX_WRITE_DATA + AX_RETURN_DELAY_TIME + (RDT/2)))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
	sendData(AX_RDT_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_RETURN_DELAY_TIME);
    sendData((RDT/2));
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
    
    return (readError());                // Return the read error
}

int16_t dynamixelSetLEDAlarm(uint8_t ID, int16_t LEDAlarm)
{    
	Checksum = (~(ID + AX_LEDALARM_LENGTH + AX_WRITE_DATA + AX_ALARM_LED + LEDAlarm))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
	sendData(AX_LEDALARM_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_ALARM_LED);
    sendData(LEDAlarm);
    sendData(Checksum);
	delayus(100);
	switchCom(Direction_Pin,Rx_MODE);

    return (readError());                // Return the read error
}

int16_t dynamixelSetShutdownAlarm(uint8_t ID, int16_t SALARM)
{    
	Checksum = (~(ID + AX_SALARM_LENGTH + AX_ALARM_SHUTDOWN + AX_ALARM_LED + SALARM))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
	sendData(AX_SALARM_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_ALARM_SHUTDOWN);
    sendData(SALARM);
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
    
    return (readError());                // Return the read error
}

int16_t dynamixelSetCWCMargin(uint8_t ID, int16_t CWCMargin)
{
	Checksum = (~(ID + AX_CM_LENGTH +AX_WRITE_DATA+ AX_CW_COMPLIANCE_MARGIN + CWCMargin))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_CM_LENGTH);
	sendData(AX_WRITE_DATA);
	sendData(AX_CW_COMPLIANCE_MARGIN);
    sendData(CWCMargin);
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    return (readError()); 
}

int16_t dynamixelSetCCWCMargin(uint8_t ID, int16_t CCWCMargin)
{
	Checksum = (~(ID + AX_CM_LENGTH + AX_WRITE_DATA + AX_CCW_COMPLIANCE_MARGIN + CCWCMargin)) & 0xFF;

	switchCom(Direction_Pin, Tx_MODE);
	sendData(AX_START);
	sendData(AX_START);
	sendData(ID);
	sendData(AX_CM_LENGTH);
	sendData(AX_WRITE_DATA);
	sendData(AX_CCW_COMPLIANCE_MARGIN);
	sendData(CCWCMargin);
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin, Rx_MODE);

	return (readError());
}

int16_t dynamixelSetCWCSlope(uint8_t ID, int16_t CWCSlope)
{
	Checksum = (~(ID + AX_CS_LENGTH +AX_WRITE_DATA+ AX_CW_COMPLIANCE_SLOPE + CWCSlope))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
	sendData(AX_START);                     
	sendData(AX_START);
	sendData(ID);
	sendData(AX_CS_LENGTH);
	sendData(AX_WRITE_DATA);
	sendData(AX_CW_COMPLIANCE_SLOPE);
    sendData(CWCSlope);
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    return (readError()); 
}

int16_t dynamixelSetCCWCSlope(uint8_t ID, int16_t CCWCSlope)
{
	Checksum = (~(ID + AX_CS_LENGTH + AX_WRITE_DATA + AX_CCW_COMPLIANCE_SLOPE + CCWCSlope)) & 0xFF;

	switchCom(Direction_Pin, Tx_MODE);
	sendData(AX_START);
	sendData(AX_START);
	sendData(ID);
	sendData(AX_CS_LENGTH);
	sendData(AX_WRITE_DATA);
	sendData(AX_CCW_COMPLIANCE_SLOPE);
	sendData(CCWCSlope);
	sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin, Rx_MODE);

	return (readError());
}

int16_t dynamixelSetPunch(uint8_t ID, int16_t Punch)
{
	int8_t Punch_H,Punch_L;
    Punch_H = Punch >> 8;           // 16 bits - 2 x 8 bits variables
    Punch_L = Punch;
	Checksum = (~(ID + AX_PUNCH_LENGTH + AX_WRITE_DATA + AX_PUNCH_L + Punch_L + Punch_H))&0xFF;
    
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                 // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
    sendData(AX_PUNCH_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_PUNCH_L);
    sendData(Punch_L);
    sendData(Punch_H);
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    return (readError());                 // Return the read error
}

int16_t dynamixelMoving(uint8_t ID)
{	
    Checksum = (~(ID + AX_MOVING_LENGTH  + AX_READ_DATA + AX_MOVING + AX_BYTE_READ))&0xFF;
    
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);
    sendData(AX_START);
    sendData(ID);
    sendData(AX_MOVING_LENGTH);
    sendData(AX_READ_DATA);
    sendData(AX_MOVING);
    sendData(AX_BYTE_READ);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    Moving_Byte = -1;
    Time_Counter = 0;
    while((availableData() < 6) & (Time_Counter < TIME_OUT))
	{
		Time_Counter++;
		delayus(1000);
    }
	
    while (availableData() > 0)
	{
		Incoming_Byte = readData();
		if ( (Incoming_Byte == 255) & (peekData() == 255) )
		{
			readData();                            // Start Bytes
			readData();                            // Ax-12 ID
			readData();                            // Length
			if( (Error_Byte = readData()) != 0 )   // Error
				return (Error_Byte*(-1));
			Moving_Byte = readData();         // Temperature
		}
    }
	return (Moving_Byte);               // Returns the read temperature
}

int16_t dynamixelLockRegister(uint8_t ID)
{    
	Checksum = (~(ID + AX_LR_LENGTH + AX_WRITE_DATA + AX_LOCK + LOCK))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);                // Send Instructions over Serial
    sendData(AX_START);
    sendData(ID);
	sendData(AX_LR_LENGTH);
    sendData(AX_WRITE_DATA);
    sendData(AX_LOCK);
    sendData(LOCK);
    sendData(Checksum);
	delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
    
    return (readError());                // Return the read error
}

int16_t dynamixelRWStatus(uint8_t ID)
{	
    Checksum = (~(ID + AX_RWS_LENGTH  + AX_READ_DATA + AX_REGISTERED_INSTRUCTION + AX_BYTE_READ))&0xFF;
    
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);
    sendData(AX_START);
    sendData(ID);
    sendData(AX_RWS_LENGTH);
    sendData(AX_READ_DATA);
    sendData(AX_REGISTERED_INSTRUCTION);
    sendData(AX_BYTE_READ);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    RWS_Byte = -1;
    Time_Counter = 0;
    while((availableData() < 6) & (Time_Counter < TIME_OUT))
	{
		Time_Counter++;
		delayus(1000);
    }
	
    while (availableData() > 0)
	{
		Incoming_Byte = readData();
		if ( (Incoming_Byte == 255) & (peekData() == 255) )
		{
			readData();                            // Start Bytes
			readData();                            // Ax-12 ID
			readData();                            // Length
			if( (Error_Byte = readData()) != 0 )   // Error
				return (Error_Byte*(-1));
			RWS_Byte = readData();         // Temperature
		}
    }
	return (RWS_Byte);               // Returns the read temperature
}

int16_t dynamixelReadSpeed(uint8_t ID)
{	
    Checksum = (~(ID + AX_POS_LENGTH  + AX_READ_DATA + AX_PRESENT_SPEED_L + AX_BYTE_READ_POS))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);
    sendData(AX_START);
    sendData(ID);
    sendData(AX_POS_LENGTH);
    sendData(AX_READ_DATA);
    sendData(AX_PRESENT_SPEED_L);
    sendData(AX_BYTE_READ_POS);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    Speed_Long_Byte = -1;
	Time_Counter = 0;
    while((availableData() < 7) & (Time_Counter < TIME_OUT))
	{
		Time_Counter++;
		delayus(1000);
    }
	
    while (availableData() > 0)
	{
		Incoming_Byte = readData();
		if ( (Incoming_Byte == 255) & (peekData() == 255) )
		{
			readData();                            // Start Bytes
			readData();                            // Ax-12 ID
			readData();                            // Length
			if( (Error_Byte = readData()) != 0 )   // Error
				return (Error_Byte*(-1));
			
			Speed_Low_Byte = readData();            // Position Bytes
			Speed_High_Byte = readData();
			Speed_Long_Byte = Speed_High_Byte << 8; 
			Speed_Long_Byte = Speed_Long_Byte + Speed_Low_Byte;
		}
    }
	return (Speed_Long_Byte);     // Returns the read position
}

int16_t dynamixelReadLoad(uint8_t ID)
{	
    Checksum = (~(ID + AX_POS_LENGTH  + AX_READ_DATA + AX_PRESENT_LOAD_L + AX_BYTE_READ_POS))&0xFF;
	
	switchCom(Direction_Pin,Tx_MODE);
    sendData(AX_START);
    sendData(AX_START);
    sendData(ID);
    sendData(AX_POS_LENGTH);
    sendData(AX_READ_DATA);
    sendData(AX_PRESENT_LOAD_L);
    sendData(AX_BYTE_READ_POS);
    sendData(Checksum);
    delayus(TX_DELAY_TIME);
	switchCom(Direction_Pin,Rx_MODE);
	
    Load_Long_Byte = -1;
	Time_Counter = 0;
    while((availableData() < 7) & (Time_Counter < TIME_OUT))
	{
		Time_Counter++;
		delayus(1000);
    }
	
    while (availableData() > 0)
	{
		Incoming_Byte = readData();
		if ( (Incoming_Byte == 255) & (peekData() == 255) )
		{
			readData();                            // Start Bytes
			readData();                            // Ax-12 ID
			readData();                            // Length
			if( (Error_Byte = readData()) != 0 )   // Error
				return (Error_Byte*(-1));
			
			Load_Low_Byte = readData();            // Position Bytes
			Load_High_Byte = readData();
			Load_Long_Byte = Load_High_Byte << 8; 
			Load_Long_Byte = Load_Long_Byte + Load_Low_Byte;
		}
    }
	return (Load_Long_Byte);     // Returns the read position
}