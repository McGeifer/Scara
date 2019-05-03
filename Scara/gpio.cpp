/*

*/

#include "gpio.h"
#include "objdir.h"
#include "status.h"

void initGPIO(void)
{
    /* set pin modes */
    pinMode(GPIO_PIN_MODBUS, INPUT);        /* pin to enable Modbus communication */
    pinMode(GPIO_PIN_RAPID, INPUT);         /* pin to enable "Rapid" mode */
    pinMode(GPIO_PIN_MAGNET, OUTPUT);       /* pin to switch the lifting magnet on/off */
    pinMode(GPIO_PIN_INTERRUPT, INPUT);     /* interrupt pin for the light barrier */

    /* interrupts */
    attachInterrupt(digitalPinToInterrupt(3), lightBarrierISR, CHANGE); /* attach interrupt for light barrier impulse counter */
}

void initOperationMode(void)
{
    /* modbus protocol selected */
    if (!digitalRead(GPIO_PIN_RAPID) && digitalRead(GPIO_PIN_MODBUS))
    {
        if (setObjData(OBJ_IDX_OP_MODE, OP_MODE_MODBUS, false) == 0)
        {
            return;
        }
    }
    /* rapid protocol selected */
    else if (digitalRead(GPIO_PIN_RAPID) && !digitalRead(GPIO_PIN_MODBUS))
    {
        if (setObjData(OBJ_IDX_OP_MODE, OP_MODE_RAPID, false) == 0)
        {
            Serial.println("");
            Serial.println("");
            SendStatus("initOperationMode(): ", "Rapid protocol selected", SYS_STAT_MSG_TYPE_INFO);
            return;
        }
    }
    /* scara protocol selected */
    else
    {
        if (setObjData(OBJ_IDX_OP_MODE, OP_MODE_SCARA, false) == 0)
        {
            SendStatus("initOperationMode(): ", "Scara protocol selected", SYS_STAT_MSG_TYPE_INFO);
            return;
        }
    }
    SendStatus("initOperationMode(): ", "error selecting protocol", SYS_STAT_MSG_TYPE_ERROR);
}

void lightBarrierISR(void)
{
    if (getObjData(OBJ_IDX_Z_ACTUAL_POS) > getObjData(OBJ_IDX_Z_ACTUAL_TARGET_POS))
    {
        setObjData(OBJ_IDX_Z_POS_COUNT, getObjData(OBJ_IDX_Z_POS_COUNT) - 1, false);
    }
    else
    {
        setObjData(OBJ_IDX_Z_POS_COUNT, getObjData(OBJ_IDX_Z_POS_COUNT) + 1, false);
    }
}