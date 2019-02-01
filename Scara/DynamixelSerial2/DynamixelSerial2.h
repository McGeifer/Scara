/*
 Dynamixel.cpp - Ax-12+ Half Duplex USART Comunication
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
 25/07/2011 - Modificado la funcion setBD() para aceptar todas la velocidades sin PDF.
 25/07/2011 - Agregada la funcion de Rotacion Continua.
 26/07/2011 - Agregada la funcion begin sin seteo de Direction_Pin.
 25/07/2011 - Agregada la funcion Reset.
 26/07/2011 - Agregada la funcion Reg_Write en move y moveSpeed.
 26/07/2011 - Agregada la funcion Action.
 13/12/2011 - Arreglado el manejo y envio de variables.
 22/12/2011 - Compatible con la actualizacion Arduino 1.0.
 10/01/2012 - Utilizacion de Macros y eliminacion codigo no necesario.
 11/01/2012 - Agregadas las funciones:
              int16_t setTempLimit(uint8_t ID, uint8_t Temperature);
              int16_t setAngleLimit(uint8_t ID, int16_t CWLimit, int16_t CCWLimit);
              int16_t setVoltageLimit(uint8_t ID, uint8_t DVoltage, uint8_t UVoltage);
              int16_t setMaxTorque(uint8_t ID, int16_t MaxTorque);
			  int16_t setSRL(uint8_t ID, uint8_t SRL);
              int16_t setRDT(uint8_t ID, uint8_t RDT);
              int16_t setLEDAlarm(uint8_t ID, uint8_t LEDAlarm);
			  int16_t setShutdownAlarm(uint8_t ID, uint8_t SALARM);
              int16_t setCMargin(uint8_t ID, uint8_t CWCMargin, uint8_t CCWCMargin);
              int16_t setCSlope(uint8_t ID, uint8_t CWCSlope, uint8_t CCWCSlope);
 15/01/2012 - Agregadas las funciones:
              int16_t setPunch(uint8_t ID, int16_t Punch);
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

#ifndef _DYNAMIXELSERIAL2_H
#define _DYNAMIXELSERIAL2_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

	// EEPROM AREA  ///////////////////////////////////////////////////////////
#define AX_MODEL_NUMBER_L           0
#define AX_MODEL_NUMBER_H           1
#define AX_VERSION                  2
#define AX_ID                       3
#define AX_BAUD_RATE                4
#define AX_RETURN_DELAY_TIME        5
#define AX_CW_ANGLE_LIMIT_L         6
#define AX_CW_ANGLE_LIMIT_H         7
#define AX_CCW_ANGLE_LIMIT_L        8
#define AX_CCW_ANGLE_LIMIT_H        9
#define AX_SYSTEM_DATA2             10
#define AX_LIMIT_TEMPERATURE        11
#define AX_DOWN_LIMIT_VOLTAGE       12
#define AX_UP_LIMIT_VOLTAGE         13
#define AX_MAX_TORQUE_L             14
#define AX_MAX_TORQUE_H             15
#define AX_RETURN_LEVEL             16
#define AX_ALARM_LED                17
#define AX_ALARM_SHUTDOWN           18
#define AX_OPERATING_MODE           19
#define AX_DOWN_CALIBRATION_L       20
#define AX_DOWN_CALIBRATION_H       21
#define AX_UP_CALIBRATION_L         22
#define AX_UP_CALIBRATION_H         23

	// RAM AREA  //////////////////////////////////////////////////////////////
#define AX_TORQUE_ENABLE            24
#define AX_LED                      25
#define AX_CW_COMPLIANCE_MARGIN     26
#define AX_CCW_COMPLIANCE_MARGIN    27
#define AX_CW_COMPLIANCE_SLOPE      28
#define AX_CCW_COMPLIANCE_SLOPE     29
#define AX_GOAL_POSITION_L          30
#define AX_GOAL_POSITION_H          31
#define AX_GOAL_SPEED_L             32
#define AX_GOAL_SPEED_H             33
#define AX_TORQUE_LIMIT_L           34
#define AX_TORQUE_LIMIT_H           35
#define AX_PRESENT_POSITION_L       36
#define AX_PRESENT_POSITION_H       37
#define AX_PRESENT_SPEED_L          38
#define AX_PRESENT_SPEED_H          39
#define AX_PRESENT_LOAD_L           40
#define AX_PRESENT_LOAD_H           41
#define AX_PRESENT_VOLTAGE          42
#define AX_PRESENT_TEMPERATURE      43
#define AX_REGISTERED_INSTRUCTION   44
#define AX_PAUSE_TIME               45
#define AX_MOVING                   46
#define AX_LOCK                     47
#define AX_PUNCH_L                  48
#define AX_PUNCH_H                  49

    // Status Return Levels ///////////////////////////////////////////////////////////////
#define AX_RETURN_NONE              0
#define AX_RETURN_READ              1
#define AX_RETURN_ALL               2

    // Instruction Set ///////////////////////////////////////////////////////////////
#define AX_PING                     1
#define AX_READ_DATA                2
#define AX_WRITE_DATA               3
#define AX_REG_WRITE                4
#define AX_ACTION                   5
#define AX_RESET                    6
#define AX_SYNC_WRITE               131

	// Specials ///////////////////////////////////////////////////////////////
#define OFF                         0
#define ON                          1
#define LEFT						0
#define RIGTH                       1
#define AX_BYTE_READ                1
#define AX_BYTE_READ_POS            2
#define AX_RESET_LENGTH				2
#define AX_ACTION_LENGTH			2
#define AX_ID_LENGTH                4
#define AX_LR_LENGTH                4
#define AX_SRL_LENGTH               4
#define AX_RDT_LENGTH               4
#define AX_LEDALARM_LENGTH          4
#define AX_SALARM_LENGTH            4
#define AX_TL_LENGTH                4
#define AX_VL_LENGTH                4
#define AX_CM_LENGTH                4
#define AX_CS_LENGTH                4
#define AX_CCW_CW_LENGTH            8
#define AX_BD_LENGTH                4
#define AX_TEM_LENGTH               4
#define AX_MOVING_LENGTH            4
#define AX_RWS_LENGTH               4
#define AX_VOLT_LENGTH              4
#define AX_LED_LENGTH               4
#define AX_TORQUE_LENGTH            4
#define AX_POS_LENGTH               4
#define AX_GOAL_LENGTH              5
#define AX_MT_LENGTH                5
#define AX_PUNCH_LENGTH             5
#define AX_SPEED_LENGTH             5
#define AX_GOAL_SP_LENGTH           7
#define AX_ACTION_CHECKSUM			250
#define BROADCAST_ID                254
#define AX_START                    255
#define AX_CCW_AL_L                 255 
#define AX_CCW_AL_H                 3
#define TIME_OUT                    10         // Este parametro depende de la velocidad de transmision
#define TX_DELAY_TIME				400        // Este parametro depende de la velocidad de transmision - pero pueden ser cambiados para mayor velocidad.
#define Tx_MODE                     1
#define Rx_MODE                     0
#define LOCK                        1

void dynamixelBegin(int32_t baud, uint8_t directionPin);
void dynamixelEnd(void);
	
int16_t dynamixelReset(uint8_t ID);
int16_t dynamixelPing(uint8_t ID);
	
int16_t dynamixelSetID(uint8_t ID, uint8_t newID);
int16_t dynamixelSetBD(uint8_t ID, int32_t baud);

int16_t dynamixelMove(uint8_t ID, int16_t Position);
int16_t dynamixelMoveSpeed(uint8_t ID, int16_t Position, int16_t Speed);
int16_t dynamixelSetEndless(uint8_t ID,bool Status);
int16_t dynamixelTurn(uint8_t ID, bool SIDE, int16_t Speed);
int16_t dynamixelMoveRW(uint8_t ID, int16_t Position);
int16_t dynamixelMoveSpeedRW(uint8_t ID, int16_t Position, int16_t Speed);
	
void dynamixelAction(void);
	
int16_t dynamixelSetTempLimit(uint8_t ID, int16_t Temperature, const char **funcName);
int16_t dynamixelSetCWAngleLimit(uint8_t ID, int16_t CWLimit, const char **funcName);
int16_t dynamixelSetCCWAngleLimit(uint8_t ID, int16_t CCWLimit, const char **funcName);
int16_t dynamixelSetLowVoltageLimit(uint8_t ID, int16_t DVoltage, const char **funcName);
int16_t dynamixelSetHighVoltageLimit(uint8_t ID, int16_t UVoltage, const char **funcName);
int16_t dynamixelSetMaxTorque(uint8_t ID, int16_t MaxTorque, const char **funcName);
int16_t dynamixelSetSRL(uint8_t ID, int16_t SRL, const char **funcName);
int16_t dynamixelSetRDT(uint8_t ID, int16_t RDT, const char **funcName);
int16_t dynamixelSetLEDAlarm(uint8_t ID, int16_t LEDAlarm, const char **funcName);
int16_t dynamixelSetShutdownAlarm(uint8_t ID, int16_t SALARM, const char **funcName);
int16_t dynamixelSetCWCMargin(uint8_t ID, int16_t CWCMargin, const char **funcName);
int16_t dynamixelSetCCWCMargin(uint8_t ID, int16_t CCWCMargin, const char **funcName);
int16_t dynamixelSetCWCSlope(uint8_t ID, int16_t CWCSlope, const char **funcName);
int16_t dynamixelSetCCWCSlope(uint8_t ID, int16_t CCWCSlope, const char **funcName);
int16_t dynamixelSetPunch(uint8_t ID, int16_t Punch, const char **funcName);
	
int16_t dynamixelMoving(uint8_t ID);
int16_t dynamixelLockRegister(uint8_t ID);
int16_t dynamixelRWStatus(uint8_t ID);
	
int16_t dynamixelReadTemperature(uint8_t ID);
int16_t dynamixelReadVoltage(uint8_t ID);
int16_t dynamixelReadPosition(uint8_t ID);
int16_t dynamixelReadSpeed(uint8_t ID);
int16_t dynamixelReadLoad(uint8_t ID);
	
int16_t dynamixelTorqueStatus(uint8_t ID, bool Status);
int16_t dynamixelLedStatus(uint8_t ID, bool Status);

#endif
