/*
 * File:   main.c
 * Author: georg
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include "include/i2cmanager.h"

I2c* i2c1_ref;

#ifdef	__cplusplus
extern "C" {
#endif

void __ISR(_I2C_1_VECTOR, ipl5) I2cInterruptServiceRoutine(void)
{
    i2c1_ref->handleInterrupt();
    INTClearFlag(INT_I2C1);
}

#ifdef	__cplusplus
}
#endif


bool I2c::handleInterrupt() {
    switch(currentstatus) {
        case START_SENT:
        {
            UINT8 addr_rw = (deviceaddr |= 0x01); // r: bit0=1
            if(currentschema <= REG_BASED_SCHEMA ) {
                // w:  bit0 = 0  r: bit0=1
                addr_rw &= 0xfe ;
            }

            if(I2CSendByte(module, deviceaddr) == I2C_MASTER_BUS_COLLISION)
            {
                return false;
            }
            currentstatus = S_ADDRESS_SENT;
            break;
        }

        case S_ADDRESS_SENT:
        {
            if(I2CSendByte(module, regaddress) == I2C_MASTER_BUS_COLLISION)
            {
                return false;
            }
            currentstatus = REG_ADDR_SENT;
            break;
        }

        case REG_ADDR_SENT:
        {
            if(    currentschema == READ_SINGLE_FROM_REG ||
                   currentschema == READ_MULTI_FROM_REG ) {

                if(I2CRepeatStart(module) == I2C_MASTER_BUS_COLLISION)
                {
                    return false;
                }

                currentstatus = RESTART_SENT;
            }
            else {
                //TODO: reg write accesses
            }

            break;
        }

        case RESTART_SENT:
        {
            if(    currentschema == READ_SINGLE_FROM_REG ||
                   currentschema == READ_MULTI_FROM_REG ) {
                UINT8 addr_rw = (deviceaddr |= 0x01); // r: bit0=1

                if(I2CSendByte(module, addr_rw) == I2C_MASTER_BUS_COLLISION)
                {
                    return false;
                }

                currentstatus = RS_ADDRESS_SENT;
            }

            break;
        }

        case RS_ADDRESS_SENT:
        {
            if(    currentschema == READ_SINGLE_FROM_REG ||
                   currentschema == READ_MULTI_FROM_REG ) {

                if(I2CReceiverEnable(module,true) == I2C_RECEIVE_OVERFLOW)
                {
                    return false;
                }

                currentstatus = DATA_RECEIVE_ENABLED;
            }

            break;
        }

        case DATA_ACK_SENT:
        case DATA_RECEIVE_ENABLED:
        {
            if(    currentschema == READ_SINGLE_FROM_REG ||
                   currentschema == READ_MULTI_FROM_REG ) {

                if(I2CReceivedDataIsAvailable(module) == TRUE) {
                    I2CAcknowledgeByte(module, (datalen<=1)? FALSE:TRUE );
                    if(datalen>0) {
                        *dataptr == I2CGetByte();
                        datalen--;
                    }
                }
                else return false

                currentstatus = (datalen == 0) ? DATA_NACK_SENT : DATA_ACK_SENT ;
            }

            break;
        }


        case DATA_NACK_SENT:
        {
            if( currentschema <= REG_BASED_SCHEMA ) {

                I2CStop();

                currentstatus = BUS_IDLE ;
            }

            break;
        }

        default:
            break;
    }

    return true;
}


void I2c::setupInterrupt() {
    // Configure the interrupt priority, level 5
    INTSetVectorPriority( INT_I2C_1_VECTOR,
                          INT_PRIORITY_LEVEL_5);

    // Configure the interrupt sub-priority, level 2
    INTSetVectorSubPriority( INT_I2C_1_VECTOR,
                             INT_SUB_PRIORITY_LEVEL_2);

    // Clear the interrupt source flag
    INTClearFlag(INT_I2C1B);
    INTClearFlag(INT_I2C1M);

    // Enable the interrupt source
    INTEnable(INT_I2C1B, INT_ENABLED);
    INTEnable(INT_I2C1M, INT_ENABLED);
}


I2c::I2c(I2C_MODULE mod) {

    module = mod;
    currentstatus = NOT_INIT;
    setupInterrupt();
    if(mod == I2C1) {
        i2c1_ref = this;
    }
}


bool I2c::StartReadByteFromReg(UINT8 devaddress, UINT8 regaddress, UINT8* valuedest) {
    if(isBusy()) return false;

    currentschema = READ_SINGLE_FROM_REG;
    this->deviceaddr = devaddress;
    this->regaddress = regaddress;
    this->dataptr = valuedest;
    this->datalen = 1;

    if(I2CStart(module) != I2C_SUCCESS) {
        return false;
    }

    currentstatus = START_SENT;

    return false;
}

bool I2c::StartWriteByteToReg(UINT8 devaddress, UINT8 regaddress, UINT8 value) {

    return false;
}

bool I2c::StartReadFromReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, UINT8* valuesdest) {

    return false;
}

bool I2c::StartWriteToReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, UINT8* values) {
    
    return false;
}