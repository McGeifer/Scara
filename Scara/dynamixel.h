/* dynamixel.h */

#ifndef _DYNAMIXEL_h
#define _DYNAMIXEL_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "arduino.h"
#else
    #include "WProgram.h"
#endif

#include "Stream.h"

//#define DYNA_TO_DEG(val) (round((val * 300.0) / 1023.0)) /* convert a dynamixel position value (0 - 1023) into an angle (degree) */
//#define DEG_TO_DYNA(val) (round((val / 1023.0) * 300.0)) /* convert an angle (degree) into a dynamixel position value */

/* Dynamixel control table parameters */
/* EEPROM area */
#define DXL_P_MODEL_NUMBER_L                    0       /* Model Number(L) */
#define DXL_P_MODEL_NUMBER_H                    1       /* Model Number(H) */
#define DXL_P_VERSION                           2       /* Version of Firmware */
#define DXL_P_ID                                3       /* ID */
#define DXL_P_BAUD_RATE                         4       /* Baud Rate */
#define DXL_P_RETURN_DELAY_TIME                 5       /* Return Delay Time */
#define DXL_P_CW_ANGLE_LIMIT_L                  6       /* CW Angle Limit(L) */
#define DXL_P_CW_ANGLE_LIMIT_H                  7       /* CW Angle Limit(H) */
#define DXL_P_CCW_ANGLE_LIMIT_L                 8       /* CCW Angle Limit(L) */
#define DXL_P_CCW_ANGLE_LIMIT_H                 9       /* CCW Angle Limit(H) */
#define DXL_P_SYSTEM_DATA2                      10      /* Reserved) */
#define DXL_P_LIMIT_TEMPERATURE                 11      /* the Highest Limit Temperature */
#define DXL_P_DOWN_LIMIT_VOLTAGE                12      /* the Lowest Limit Voltage */
#define DXL_P_UP_LIMIT_VOLTAGE                  13      /* the Highest Limit Voltage */
#define DXL_P_MAX_TORQUE_L                      14      /* Max Torque(L) */
#define DXL_P_MAX_TORQUE_H                      15      /* Max Torque(H) */
#define DXL_P_RETURN_LEVEL                      16      /* Status Return Level */
#define DXL_P_ALARM_LED                         17      /* Alarm LED */
#define DXL_P_ALARM_SHUTDOWN                    18      /* Alarm Shutdown */
#define DXL_P_OPERATING_MODE                    19      /* (Reserved) */
#define DXL_P_DOWN_CALIBRATION_L                20      /* Down Calibration(L) */
#define DXL_P_DOWN_CALIBRATION_H                21      /* Down Calibration(H) */
#define DXL_P_UP_CALIBRATION_L                  22      /* Up Calibration(L) */
#define DXL_P_UP_CALIBRATION_H                  23      /* Up Calibration(H) */

/* RAM area */
#define DXL_P_TORQUE_ENABLE                     24      /* Torque Enable */
#define DXL_P_LED                               25      /* LED */
#define DXL_P_CW_COMPLIANCE_MARGIN              26      /* CW Compliance Margin */
#define DXL_P_CCW_COMPLIANCE_MARGIN             27      /* CCW Compliance Margin */
#define DXL_P_CW_COMPLIANCE_SLOPE               28      /* CW Compliance Slope */
#define DXL_P_CCW_COMPLIANCE_SLOPE              29      /* CCW Compliance Slope */
#define DXL_P_GOAL_POSITION_L                   30      /* Goal Position(L) */
#define DXL_P_GOAL_POSITION_H                   31      /* Goal Position(H) */
#define DXL_P_GOAL_SPEED_L                      32      /* Moving Speed(L) */
#define DXL_P_GOAL_SPEED_H                      33      /* Moving Speed(H) */
#define DXL_P_TORQUE_LIMIT_L                    34      /* Torque Limit(L) */
#define DXL_P_TORQUE_LIMIT_H                    35      /* Torque Limit(H) */
#define DXL_P_PRESENT_POSITION_L                36      /* Present Position(L) */
#define DXL_P_PRESENT_POSITION_H                37      /* Present Position(H) */
#define DXL_P_PRESENT_SPEED_L                   38      /* Present Speed(L) */
#define DXL_P_PRESENT_SPEED_H                   39      /* Present Speed(H) */
#define DXL_P_PRESENT_LOAD_L                    40      /* Present Load(L) */
#define DXL_P_PRESENT_LOAD_H                    41      /* Present Load(H) */
#define DXL_P_PRESENT_VOLTAGE                   42      /* Present Voltage */
#define DXL_P_PRESENT_TEMPERATURE               43      /* Present Temperature */
#define DXL_P_REGISTERED_INSTRUCTION            44      /* Registered Instruction */
#define DXL_P_PAUSE_TIME                        45      /* (Reserved) */
#define DXL_P_MOVING                            46      /* Moving */
#define DXL_P_LOCK                              47      /* Lock */
#define DXL_P_PUNCH_L                           48      /* Punch(L) */
#define DXL_P_PUNCH_H                           49      /* Punch(H) */

/* instructions */
#define DXL_INST_PING                           0x01    /* No action. Used for obtaining a Status Packet  */
#define DXL_INST_READ                           0x02    /* Reading values in the Control Table  */
#define DXL_INST_WRITE                          0x03    /* Writing values to the Control Table  */
#define DXL_INST_REG_WRITE                      0x04    /* Similar to WRITE, but stays in standby mode until the ACION instruction is given  */
#define DXL_INST_ACTION                         0x05    /* Triggers the action registered by the REG_WRITE instruction  */
#define DXL_INST_RESET                          0x06    /* Changes the control table values of the Dynamixel actuator to the Factory Default Value  */
#define DXL_INST_SYNC_WRITE                     0x83    /* Used for controlling many Dynamixel actuators at the same time  */

/* setup parameters for the Dynamixel servos */
#define DXL_INIT_TOTAL_SIZE                     10      /* total size of setup parameters (in byte) + 1 */

#define DXL_INIT_AXIS_1_TEMP_LIMIT              70      /* max temperature 70°C */
#define DXL_INIT_AXIS_1_VOLTAGE_LIMIT_LOW       110     /* min voltage 11V */
#define DXL_INIT_AXIS_1_VOLTAGE_LIMIT_HIGH      130     /* max voltage 13V */
#define DXL_INIT_AXIS_1_MAX_TORQUE              1023    /* max torque -> max value */
#define DXL_INIT_AXIS_1_STATUS_RETURN_LEVEL     2       /* return level 0=none, 1=only for read command, 2= always */
#define DXL_INIT_AXIS_1_RETURN_DELAY_TIME       5       /* return delay time = 2µs * value -> 10µs */
#define DXL_INIT_AXIS_1_ALARM_LED               127     /* LED blink for all error types */
#define DXL_INIT_AXIS_1_ALARM_SHUT_DOWN         37      /* turn of torque for: overload, overheating and input voltage error */

#define DXL_INIT_AXIS_2_TEMP_LIMIT              70
#define DXL_INIT_AXIS_2_VOLTAGE_LIMIT_LOW       110
#define DXL_INIT_AXIS_2_VOLTAGE_LIMIT_HIGH      130
#define DXL_INIT_AXIS_2_MAX_TORQUE              1023
#define DXL_INIT_AXIS_2_STATUS_RETURN_LEVEL     2
#define DXL_INIT_AXIS_2_RETURN_DELAY_TIME       5
#define DXL_INIT_AXIS_2_ALARM_LED               127
#define DXL_INIT_AXIS_2_ALARM_SHUT_DOWN         37

#define DXL_INIT_Z_AXIS_TEMP_LIMIT              70
#define DXL_INIT_Z_AXIS_VOLTAGE_LIMIT_LOW       110
#define DXL_INIT_Z_AXIS_VOLTAGE_LIMIT_HIGH      130
#define DXL_INIT_Z_AXIS_MAX_TORQUE              1023
#define DXL_INIT_Z_AXIS_STATUS_RETURN_LEVEL     2
#define DXL_INIT_Z_AXIS_RETURN_DELAY_TIME       5
#define DXL_INIT_Z_AXIS_ALARM_LED               127
#define DXL_INIT_Z_AXIS_ALARM_SHUT_DOWN         37
#define DXL_INIT_Z_ASIS_CW_ANGLE_LIMIT          0
#define DXL_INIT_Z_ASIS_CCW_ANGLE_LIMIT         0

/* various */
#define DXL_POS_TOLERANCE                       0.5     /* max position tolerance for a Dynamixel */
#define DXL_TORQUE_ON                           1
#define DXL_TORQUE_OFF                          0
#define DXL_START                               0xFF    /* start byte of Dynamixel protocol */

/* dynamixel axis id's */
#define DXL_ID_AXIS_1                           0x00    /* Dynamixel id for axis 1 */
#define DXL_ID_AXIS_2                           0x01    /* Dynamixel id for axis 2 */
#define DXL_ID_AXIS_Z                           0x02    /* Dynamixel is for z axis */
#define DXL_ID_TOTAL_NUMBER_OF_AXIS             0x03    /* The total number of Dynamixel servos connected to the System */
#define DXL_BROADCASTING_ID                     0xFE    /* Dynamixel broadcast id */

/* connection settings */
#define DXL_DIRECTION_PIN                       2       /* GPIO for switching the tristate buffer (switching between Tx and Rx mode) */
#define DXL_TIMEOUT                             300     /* timeout for return packet in µs */

/* error states - triggered by dynamixel */
#define DXL_ERR_INPUT_VOLTAGE                   1       /* 0x01 - Dynamixel - Input Voltage Error */
#define DXL_ERR_ANGLE_LIMIT                     2       /* 0x02 - Dynamixel - Angle Limit Error */ 
#define DXL_ERR_OVERHEATING                     4       /* 0x04 - Dynamixel - Overheating Error */ 
#define DXL_ERR_RANGE                           8       /* 0x08 - Dynamixel - Range Error */ 
#define DXL_ERR_CHECKSUM                        16      /* 0x10 - Dynamixel - Checksum Error */ 
#define DXL_ERR_OVERLOAD                        32      /* 0x20 - Dynamixel - Overload Error */ 
#define DXL_ERR_INSTRUCTION                     64      /* 0x40 - Dynamixel - Instruction Error */ 
#define DXL_ERR_NO_RESPONSE                     128     /* 0x80 - Dynamixel - no response */ 

/* error states - triggered internally */
#define DXL_ERR_TIMEOUT_WAITING_STATUS          -1      /* timeout while waiting for status packet  */
#define DXL_ERR_TIMEOUT_RECEIVING_STATUS        -2      /* timeout while receiving status packet */
#define DXL_ERR_INVALID_STATUS_DATA             -3      /* invalid status packet */
#define DXL_ERR_MEMORY_ALLOCATION               -4      /* error while allocating memory */
#define DXL_ERR_STATUS_PACKET_SIZE              -5      /* wrong status packet size */
#define DXL_ERR_NO_PARAM_LIST                   -6      /* no parameter list for status packet */
#define DXL_ERR_NO_SYNC_WRITE                   -10     /* sync write not supported */

/* macros for serial port functions */
#define DXL_SERIAL_PORT     Serial2                         /* serial port used by the Dynamixel */
#define dxlBeginCom(args)   (DXL_SERIAL_PORT.begin(args))   /* begin serial communication */
#define dxlSendData(args)   (DXL_SERIAL_PORT.write(args))   /* write byte over serial */
#define dxlAvailableData()  (DXL_SERIAL_PORT.available())   /* check serial data available */
#define dxlReadData()       (DXL_SERIAL_PORT.read())        /* read byte over serial data */
#define dxlPeekData()       (DXL_SERIAL_PORT.peek())        /* peek serial data */
#define dxlFlush()          (DXL_SERIAL_PORT.flush())       /* flush serial buffer */

/* macros for switching between Rx & Tx mode */
#define DXL_RX_MDOE LOW
#define DXL_TX_MODE HIGH
#define dxlSetDirPin(DirPin)    (pinMode(DirPin,OUTPUT))                    /* select the GPIO for switching the tristate buffer */
#define dxlSetComMode(Mode)     (digitalWrite(DXL_DIRECTION_PIN, Mode))     /* switch between Rx- or Tx-Mode */

/* data structure to store dynamixel status return packet defined by the Dynamixel protocol */
typedef struct
{
    uint8_t id;         /* dynamixel ID */
    uint8_t length;     /* length of the status packet */
    uint8_t error;      /* dynamixel error byte */
    uint8_t *param;     /* additional data */
    uint8_t checksum;   /* status packet check sum */
} dxlStatusPacket_t;

/*
    Setup function for the Dynamixel servos
    more information !!!!!!!!!!!!!!!!!!!!!!!
 */
void initDynamixel(void);

/*
    Handler function for Dynamixel-specific error codes. If an error code is reported to
    the function it will issue an error message. In addition the execution of the main loop
    is stopped to prevent inadvertent behavior of the system.
 */
void dxlPrintErrorMessage(int16_t error, uint8_t id);

/*
    Cyclic function for detecting the move status of the system and if necessary, executing
    move commands for the Dynamixel servos.
 */
void handleMove(void);

/*
    Open serial port for Dynamixel communication and set direction pin to control the half duplex UART.
 */
void dxlStartCom(uint32_t baudRate, uint8_t directionPin);

/*
    #################################################
    ####### Dynamixel - status return packet ########
    #################################################
*/

/*
    This function returns the data of the actual Dynamixel status return packet.
*/
const dxlStatusPacket_t dxlGetStatusPacket(void);

/*
    The dxlSetStatusPacket() function stores the given Dynamixel status packet for further processing by
    other functions. To access the stored data use the dxlGetStatusPacket() function.
*/
void dxlSetStatusPacket(dxlStatusPacket_t args);

/*
    #################################################
    ########## Dynamixel - basic functions ##########
    #################################################
*/

/*
    No action. Used for obtaining a Status Packet. Dynamixel servos will always return a status packet
    no matter how they are configured or if the broadcasting id is used.

    return  0 = OK - Dynamixel found
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlPing(uint8_t id);

/*
    Changes the control table values of the Dynamixel actuator to the factory default value settings.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlReset(uint8_t id);

/*
    Triggers the action registered by the "dxlSetRegisteredInstruction" function.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlAction(uint8_t);

/*
    Used for controlling many Dynamixel actuators at the same time.

    ! NOT SUPPORTED JET !
    always returns -1
*/
int8_t dxlSyncWrite(void);

/*
    #################################################
    ########## Dynamixel - write functions ##########
    #################################################
*/

/*
    Set new device ID.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetId(uint8_t id, uint8_t new_id);

/*
    Set new Baudrate -> Speed (BPS) = 2000000 / (Address4 + 1) see manual for further information

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetBaudRate(uint8_t id, uint8_t baud_rate);

/*
    The time it takes for the Status Packet to return after the Instruction
    Packet is sent. The delay time is given by 2 µs * return_delay value.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetReturnDelay(uint8_t id, uint8_t return_delay);

/*
    Sets the Dynamixel actuator’s operating angle range. The Goal Position needs to be within the
    range of: CW Angle Limit <= Goal Position <= CCW Angle Limit. An Angle Limit Error will occur
    if the Goal Position is set outside this range set by the operating angle limits.

    If both values for the CW Angle Limit and the CCW Angle Limit are set to 0, an Endless Turn mode
    can be implemented by setting the Goal Speed. This feature can be used for implementing a
    continuously rotating wheel.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetCWAngleLimit(uint8_t id, uint16_t cw_angle_limit);

/*
    Sets the Dynamixel actuator’s operating angle range. The Goal Position needs to be within the
    range of: CW Angle Limit <= Goal Position <= CCW Angle Limit. An Angle Limit Error will occur
    if the Goal Position is set outside this range set by the operating angle limits.

    If both values for the CW Angle Limit and the CCW Angle Limit are set to 0, an Endless Turn mode
    can be implemented by setting the Goal Speed. This feature can be used for implementing a
    continuously rotating wheel.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetCCWAngleLimit(uint8_t id, uint16_t ccw_angle_limit);

/*
    The upper limit of the Dynamixel actuator’s operating temperature. If the internal temperature
    of the Dynamixel actuator gets higher than this    value, the Over Heating Error Bit (Bit 2 of the
    Status Packet) will return the value 1, and    an alarm will be set by Address 17, 18. The values
    are in Degrees Celsius.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetTempLimit(uint8_t id, uint8_t max_temp_limit);

/*
    The upper and lower limits of the Dynamixel actuator’s operating voltage. If the present voltage
    (Address 42) is out of the specified range, a Voltage Range Error Bit (Bit 0 of the Status
    Packet) will return the value 1, and an alarm will be set by Address 17, 18. The values are 10
    times the actual voltage value. For example, if the Address 12 value is 80, then the lower
    voltage limit is set to 8V.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetVoltageLimitLow(uint8_t id, uint8_t min_voltage_limit);

/*
    The upper and lower limits of the Dynamixel actuator’s operating voltage. If the present voltage
    (Address 42) is out of the specified range, a Voltage Range Error Bit (Bit 0 of the Status
    Packet) will return the value 1, and an alarm will be set by Address 17, 18. The values are 10
    times the actual voltage value. For example, if the Address 12 value is 80, then the lower
    voltage limit is set to 8V.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetVoltageLimitHigh(uint8_t id, uint8_t max_voltage_limit);

/*
    The maximum torque output for the Dynamixel actuator. When this value is set to 0, the Dynamixel
    actuator enters the Free Run mode. There are two locations where this maximum torque limit is
    defined; in the EEPROM (Address 0X0E, 0x0F) and in the RAM (Address 0x22, 0x23). When the power
    is turned on, the maximum torque limit value defined in the EEPROM is copied to the location in
    the RAM. The torque of the Dynamixel actuator is limited by the values located in the RAM (Address
    0x22, 0x23).

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetMaxTorque(uint8_t id, uint16_t max_torque);

/*
    Determines whether the Dynamixel actuator will return a Status Packet after receiving an
    Instruction Packet. In the case of an instruction which uses the Broadcast ID (0XFE) the Status
    Packet will not be returned regardless of the Address 0x10 value.

    Address16 | Returning the Status Packet
    --------------------------------------------------
            0 | Do not respond to any instructions
            1 | Respond only to READ_DATA instructions
            2 | Respond to all instructions

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetStatusReturnLevel(uint8_t id, uint8_t return_level);

/*
    If the corresponding Bit is set to 1, the LED blinks when an Error occurs. This function
    operates following the “OR” logical operation of all bits. For example, if the value is
    set to 0X05, the LED will blink when an Input Voltage Error occurs or when an Overheating
    Error occurs. Upon returning to a normal condition from an error state, the LED stops blinking
    after 2 seconds.

    Bit   | Function
    ----------------------------------------------------------------------
    Bit 7 |    0
    Bit 6 | If set to 1, the LED blinks when an Instruction Error occurs
    Bit 5 | If set to 1, the LED blinks when an Overload Error occurs
    Bit 4 | If set to 1, the LED blinks when a Checksum Error occurs
    Bit 3 | If set to 1, the LED blinks when a Range Error occurs
    Bit 2 | If set to 1, the LED blinks when an Overheating Error occurs
    Bit 1 | If set to 1, the LED blinks when an Angle Limit Error occurs
    Bit 0 | If set to 1, the LED blinks when an Input Voltage Error occurs

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetAlarmLED(uint8_t id, uint8_t alarm_led);

/*
    If the corresponding Bit is set to a 1, the Dynamixel actuator’s torque will be turned off when an
    error occurs. This function operates following the “OR” logical operation of all bits. However,
    unlike the Alarm LED, after returning to a normal condition, it maintains the torque off status.
    To recover, the Torque Enable (Address0X18) needs to be reset to 1.

    Bit   | Function
    ------------------------------------------------------------------
    Bit 7 | 0
    Bit 6 | If set to 1, torque off when an Instruction Error occurs
    Bit 5 | If set to 1, torque off when an Overload Error occurs
    Bit 4 | If set to 1, torque off when a Checksum Error occurs
    Bit 3 | If set to 1, torque off when a Range Error occurs
    Bit 2 | If set to 1, torque off when an Overheating Error occurs
    Bit 1 | If set to 1, torque off when an Angle Limit Error occurs
    Bit 0 | If set to 1, torque off when an Input Voltage Error occurs

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetAlarmShutdown(uint8_t id, uint8_t alarm_shutdown);

/*
    When the power is first turned on, the Dynamixel actuator enters the Torque Free Run condition
    (zero torque). Setting the value in Address 0x18 to 1 enables the torque.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetTorqueEnable(uint8_t id, uint8_t torque_enable);

/*
    The LED turns on when set to 1 and turns off if set to 0.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetLED(uint8_t id, uint8_t led);

/*
    The compliance of the Dynamixel actuator is defined by setting the compliance Margin and Slope.
    This feature can be utilized for absorbing shocks at the output shaft. The following graph shows
    how each compliance value (length of A, B, C & D) is defined by the Position Error and applied
    torque. See manual for further informations.

    return   0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetCWComplianceMargin(uint8_t id, uint8_t cw_compliance_margin);

/*
    The compliance of the Dynamixel actuator is defined by setting the compliance Margin and Slope.
    This feature can be utilized for absorbing shocks at the output shaft. The following graph shows
    how each compliance value (length of A, B, C & D) is defined by the Position Error and applied
    torque. See manual for further informations.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetCCWComplianceMargin(uint8_t id, uint8_t ccw_compliance_margin);

/*
    The compliance of the Dynamixel actuator is defined by setting the compliance Margin and Slope.
    This feature can be utilized for absorbing shocks at the output shaft. The following graph shows
    how each compliance value (length of A, B, C & D) is defined by the Position Error and applied
    torque. See manual for further informations.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetCWComplianceSlope(uint8_t id, uint8_t cw_compliance_slope);

/*
    The compliance of the Dynamixel actuator is defined by setting the compliance Margin and Slope.
    This feature can be utilized for absorbing shocks at the output shaft. The following graph shows
    how each compliance value (length of A, B, C & D) is defined by the Position Error and applied
    torque. See manual for further informations.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetCCWComplianceSlope(uint8_t id, uint8_t ccw_compliance_slope);

/*
    Requested angular position for the Dynamixel actuator output to move to. Setting this value to
    0x3ff moves the output shaft to the position at 300°. See manual for further informations.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetGoalPosition(uint8_t id, uint16_t goal_position);

/*
    Sets the angular velocity of the output moving to the Goal Position. Setting this value to
    its maximum value of 0x3ff moves the output with an angular velocity of 114 RPM, provided
    that there is enough power supplied (The lowest velocity is when this value is set to 1. When
    set to 0, the velocity is the largest possible for the supplied voltage, e.g. no velocity
    control is applied.)

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetGoalSpeed(uint8_t id, uint16_t goal_speed);

/*
    The maximum torque output for the Dynamixel actuator. When this value is set to 0, the
    Dynamixel actuator enters the Free Run mode. There are two locations where this maximum torque
    limit is defined; in the EEPROM (Address 0X0E, 0x0F) and in the RAM (Address 0x22, 0x23). When
    the power is turned on, the maximum torque limit value defined in the EEPROM is copied to the
    location in the RAM. The torque of the Dynamixel actuator is limited by the values located in
    the RAM (Address 0x22, 0x23).

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetTorqueLimit(uint8_t id, uint16_t torque_limit);

/*
    Similar to normal "WriteData" (dxlSet), but stays in standby mode until the ACION instruction
    is given. This function can only be used for dxlSet_ instructions NOT FOR: Ping, Read Data
    (dxlGet), Action, Reset or Sync Write.

    param_list[0]        = length of array
    param_list[1]        = DXL_P_ (Dynamixel parameter)                                                            Mehr Infos!!!!!!!
    param_list[0 + n]    = additional data

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetRegisteredInstruction(uint8_t id, uint8_t *param_list);

/*
    If set to 1, only Address 0x18 to 0x23 can be written to and other areas cannot. Once locked,
    it can only be unlocked by turning the power off.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetLock(uint8_t id, uint8_t lock);

/*
    The minimum current supplied to the motor during operation. The initial value is set to 0x20
    and its maximum value is 0x3ff.

    return  0 = OK
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlSetPunch(uint8_t id, uint16_t punch);

/*
    Function for writing user defined data into the Dynamixel control table. The data must follow
    the rules of the transmission protocol of the Dynamixel servo. The DXL_INST_ represents the
    Dynamixel instruction set followed by the parameter list.
    param_list[]        = data array containing additional data to send (NULL if no additional data needed)
              [0]        = length of array!
              [1]        = Dynamixel register address to be write
              [1 + n]    = data to write
*/
int8_t dxlSetCustomData(uint8_t id, const uint8_t DXL_INST_, uint8_t *param_list);

/*
    ################################################
    ########## Dynamixel - read functions ##########
    ################################################
*/

/*
    Read model number - For AX-12, this value is 0X000C (12).

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetModelNumber(uint8_t id);

/*
    Read Firmware version.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetFirmwareVersion(uint8_t id);

/*
    Read the Dynamixel actuator’s operating angle range. The Goal Position needs to be within the
    range of: CW Angle Limit <= Goal Position <= CCW Angle Limit. An Angle Limit Error will occur
    if the Goal Position is set outside this range set by the operating angle limits.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetCWAngleLimit(uint8_t id);

/*
    Read the Dynamixel actuator’s operating angle range. The Goal Position needs to be within the
    range of: CW Angle Limit <= Goal Position <= CCW Angle Limit. An Angle Limit Error will occur
    if the Goal Position is set outside this range set by the operating angle limits.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetCCWAngleLimit(uint8_t id);

/*
    Read the upper limit of the Dynamixel actuator’s operating temperature. If the internal
    temperature of the Dynamixel actuator gets higher than this value, the Over Heating Error Bit
    (Bit 2 of the Status Packet) will return the value 1, and an alarm will be set by Address 17,
    18. The values are in Degrees Celsius.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetTempLimit(uint8_t id);

/*
    Read the upper and lower limits of the Dynamixel actuator’s operating voltage. If the
    present voltage (Address 42) is out of the specified range, a Voltage Range Error Bit (Bit
    0 of the Status Packet) will return the value 1, and an alarm will be set by Address 17, 18.
    The values are 10 times the actual voltage value. For example, if the Address 12 value is 80,
    then the lower voltage limit is set to 8V.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetVoltageLimit(uint8_t id);

/*
    Read the maximum torque output for the Dynamixel actuator. When this value is set to 0, the
    Dynamixel actuator enters the Free Run mode. There are two locations where this maximum torque
    limit is defined; in the EEPROM (Address 0X0E, 0x0F) and in the RAM (Address 0x22, 0x23). When
    the power is turned on, the maximum torque limit value defined in the EEPROM is copied to the
    location in the RAM. The torque of the Dynamixel actuator is limited by the values located in the
    RAM (Address 0x22, 0x23).

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetMaxTorque(uint8_t id);

/*
    Read the Status Return Level. Determines whether the Dynamixel actuator will return a Status
    Packet after receiving an Instruction Packet. In the case of an instruction which uses the
    Broadcast ID (0XFE) the Status Packet will not be returned regardless of the Address 0x10 value.

    Address16 | Returning the Status Packet
    --------------------------------------------------
            0 | Do not respond to any instructions
            1 |    Respond only to READ_DATA instructions
            2 | Respond to all instructions

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetStatusReturnLevel(uint8_t id);

/*
    Read the configuration of the Alarm LED settings. If the corresponding Bit is set to 1, the LED
    blinks when an Error occurs. This function operates following the “OR” logical operation of all
    bits. For example, if the value is set to 0X05, the LED will blink when an Input Voltage Error
    occurs or when an Overheating Error occurs. Upon returning to a normal condition from an error
    state, the LED stops blinking after 2 seconds.

    Bit   | Function
    ----------------------------------------------------------------------
    Bit 7 |    0
    Bit 6 | If set to 1, the LED blinks when an Instruction Error occurs
    Bit 5 | If set to 1, the LED blinks when an Overload Error occurs
    Bit 4 | If set to 1, the LED blinks when a Checksum Error occurs
    Bit 3 | If set to 1, the LED blinks when a Range Error occurs
    Bit 2 | If set to 1, the LED blinks when an Overheating Error occurs
    Bit 1 | If set to 1, the LED blinks when an Angle Limit Error occurs
    Bit 0 | If set to 1, the LED blinks when an Input Voltage Error occurs

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetAlarmLED(uint8_t id);

/*
    Read the configuration of the Alarm Shutdown settings. If the corresponding Bit is set to a 1, the
    Dynamixel actuator’s torque will be turned off when an error occurs. This function operates
    following the “OR” logical operation of all bits. However, unlike the Alarm LED, after returning
    to a normal condition, it maintains the torque off status. To recover, the Torque Enable
    (Address0X18) needs to be reset to 1.

    Bit   | Function
    ------------------------------------------------------------------
    Bit 7 | 0
    Bit 6 | If set to 1, torque off when an Instruction Error occurs
    Bit 5 | If set to 1, torque off when an Overload Error occurs
    Bit 4 | If set to 1, torque off when a Checksum Error occurs
    Bit 3 | If set to 1, torque off when a Range Error occurs
    Bit 2 | If set to 1, torque off when an Overheating Error occurs
    Bit 1 | If set to 1, torque off when an Angle Limit Error occurs
    Bit 0 | If set to 1, torque off when an Input Voltage Error occurs

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetAlarmShutdown(uint8_t id);

/*
    Read the Torque Enable setting. When the power is first turned on, the Dynamixel actuator
    enters the Torque Free Run condition (zero torque). Setting the value in Address 0x18 to 1
    enables the torque.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetTorqueEnable(uint8_t id);

/*
    Read the status of the Dynamixel LED.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetLED(uint8_t id);

/*
    Read the Compliance Margin setting of the Dynamixel. The compliance of the Dynamixel actuator
    is defined by setting the compliance Margin and Slope. This feature can be utilized for
    absorbing shocks at the output shaft. The following graph shows how each compliance value
    (length of A, B, C & D) is defined by the Position Error and applied torque.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetCWComplianceMargin(uint8_t id);

/*
    Read the Compliance Margin setting of the Dynamixel. The compliance of the Dynamixel actuator
    is defined by setting the compliance Margin and Slope. This feature can be utilized for
    absorbing shocks at the output shaft. The following graph shows how each compliance value
    (length of A, B, C & D) is defined by the Position Error and applied torque.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetCCWComplianceMargin(uint8_t id);

/*
    Read the Compliance Slope setting of the Dynamixel. The compliance of the Dynamixel actuator
    is defined by setting the compliance Margin and Slope. This feature can be utilized for
    absorbing shocks at the output shaft. The following graph shows how each compliance value
    (length of A, B, C & D) is defined by the Position Error and applied torque.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetCWComplianceSlope(uint8_t id);

/*
    Read the Compliance Slope setting of the Dynamixel. The compliance of the Dynamixel actuator
    is defined by setting the compliance Margin and Slope. This feature can be utilized for
    absorbing shocks at the output shaft. The following graph shows how each compliance value
    (length of A, B, C & D) is defined by the Position Error and applied torque.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetCCWComplianceSlope(uint8_t id);

/*
    Get the

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetGoalPosition(uint8_t id);

/*
    Read requested angular position for the Dynamixel actuator output to move to. Setting this value
    to 0x3ff moves the output shaft to the position at 300°. See manual for further informations.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetMovingSpeed(uint8_t id);

/*
    Reads the angular velocity of the output moving to the Goal Position.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetTorqueLimit(uint8_t id);

/*
    Read current angular position of the Dynamixel actuator output.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetPresentPosition(uint8_t id);

/*
    Read current angular velocity of the Dynamixel actuator output.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetPresentSpeed(uint8_t id);

/*
    Read the magnitude of the load on the operating Dynamixel actuator. Bit 10 is the direction of
    the load.

    BIT   | 15~11 |       10       | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
    ------------------------------------------------------------------------
    Value |   0   | Load Direction |              Load Value               |

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetPresentLoad(uint8_t id);

/*
    Read the voltage currently applied to the Dynamixel actuator. The value is 10 times the actual
    voltage. For example, 10V is represented as 100 (0x64).

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetPresentVoltage(uint8_t id);

/*
    Read the internal temperature of the Dynamixel actuator in Degrees Celsius.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetPresentTemperature(uint8_t id);

/*
    Set to 1 when an instruction is assigned by the REG_WRITE command. Set to 0 after it completes
    the assigned instruction by the Action command.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetRegisteredInstruction(uint8_t id);

/*
    Set to 1 when the Dynamixel actuator is moving by its own power.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetMoving(uint8_t id);

/*
    If set to 1, only Address 0x18 to 0x23 can be written to and other areas cannot. Once locked,
    it can only be unlocked by turning the power off.

    return  0 = OK -> additional return values stored in *dxl_return_data
    return -1 = error -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetLock(uint8_t id);

/*
    The minimum current supplied to the motor during operation. The initial value is set to 0x20 and
    its maximum value is 0x3ff.

    return  0 = OK        -> additional return values stored in *dxl_return_data[1 + n]
    return -1 = error    -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetPunch(uint8_t id);

/*
    Function to read a custom data area of the Dynamixel control table. The "start_register" defines
    the start address of where to start reading data out of the Dynamixel control table. The "length"
    parameter defines the amount of data (in bytes) to be read (maximum length is 50 (0x32)).

    return  0 = OK        -> additional return values stored in *dxl_return_data[1 + n]
    return -1 = error    -> Dynamixel error bit stored in *dxl_return_data
*/
int8_t dxlGetCustomData(uint8_t id, uint8_t start_register, uint8_t length);

#endif