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
//    INTClearFlag(INT_I2C1);
    INTClearFlag(INT_I2C1M);
    i2c1_ref->handleInterrupt();
}

#ifdef	__cplusplus
}
#endif


bool I2c::handleInterrupt() {
    switch(currentstatus) {
        case START_SENT:
        {   //Now we send the slave Device address + R/W bit
            UINT8 addr_rw = (deviceaddr |= 0x01); // r: bit0=1
            if(currentschema <= REG_BASED_SCHEMA ) {
                // w:  bit0 = 0  r: bit0=1
                addr_rw &= 0xfe ;
            }
            if(I2CSendByte(module, addr_rw) == I2C_MASTER_BUS_COLLISION)
            {
                return false;
            }
            currentstatus = S_ADDRESS_SENT;
            break;
        }

        case S_ADDRESS_SENT:
        {
            // Let's see if we found someone and got an ACK
            if(I2CByteWasAcknowledged(module) == FALSE) return false;

            if(I2CSendByte(module, regaddress) == I2C_MASTER_BUS_COLLISION)
            {
                return false;
            }
            currentstatus = REG_ADDR_SENT;
            break;
        }

        case REG_ADDR_SENT:
        {
            // We send a restart in case of read procedure
            // but first we check if the sent reg_address was ACKed
            if(I2CByteWasAcknowledged(module) == FALSE) return false;

            if( currentschema == READ_REG ) {

                if(I2CRepeatStart(module) == I2C_MASTER_BUS_COLLISION)
                {
                    return false;
                }
                currentstatus = RESTART_SENT;
            }
            else { //Let's assume Write "schema" in this else
                if(datalen>0) {
                    //We start to write data
                    if(I2CSendByte(module, *dataptr) == I2C_MASTER_BUS_COLLISION) {
                        return false;
                    }
                    dataptr++;
                    datalen--;
                }

                currentstatus = DATA_SENT;
            }

            break;
        }

        case RESTART_SENT:
        {
            //OK ready to send again the slave address in read mode this time
            if( currentschema == READ_REG ) {
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
            // Let's see if got the ACK then it's time to read out data
            if(I2CByteWasAcknowledged(module) == FALSE) return false;

            if( currentschema == READ_REG ) {

                if(I2CReceiverEnable(module,TRUE) == I2C_RECEIVE_OVERFLOW)
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
            // we read out one byte (or more) and send our ACK or NACK for the last
            if( currentschema == READ_REG ) {

                if(I2CReceivedDataIsAvailable(module) == TRUE) {
                    if(datalen>0) {
                        *dataptr = I2CGetByte(module);
                        datalen--;
                    }
                }
                else return false;

                I2CAcknowledgeByte(module, (datalen==0)? FALSE:TRUE );
                currentstatus = (datalen == 0) ? DATA_NACK_SENT : DATA_ACK_SENT ;
            }

            break;
        }


        case DATA_NACK_SENT:
        {
            if( currentschema <= REG_BASED_SCHEMA ) {

                I2CStop(module);
                currentstatus = STOP_SENT ;
            }

            break;
        }

        case STOP_SENT:
        {
            // Transaction completed Bus is idle now
            mPORTDToggleBits(BIT_1);
            currentstatus = BUS_IDLE ;
            break;
        }

        case DATA_SENT:
        {
            // Let's see if got the ACK and we continue to readout data
            if(I2CByteWasAcknowledged(module) == FALSE) return false;

            if( currentschema == WRITE_REG ) {

                if(datalen>0) {
                    // Another byte to send
                    if(I2CSendByte(module, *dataptr) == I2C_MASTER_BUS_COLLISION) {
                        return false;
                    }
                    dataptr++;
                    datalen--;
                    // status is the same DATA_SENT ( we need to wait for the next ack)
                }
                else {
                    // there was nothing to send: we stop here
                    I2CStop(module);
                    currentstatus = STOP_SENT ;
                }

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
    //INTEnable(INT_I2C1B, INT_ENABLED);
    INTEnable(INT_I2C1M, INT_ENABLED);
}


I2c::I2c(I2C_MODULE mod, UINT32 perif_freq, UINT32 i2c_freq) {

    module = mod;
    currentstatus = NOT_INIT;

    // Set the I2C baudrate
    I2CSetFrequency(module, perif_freq, i2c_freq);
    
    I2CEnable(module,TRUE);
    currentstatus = BUS_IDLE;

    //while( !I2CBusIsIdle(module) );

    setupInterrupt();
    if(mod == I2C1) {
        i2c1_ref = this;
    }
}


bool I2c::StartReadByteFromReg(UINT8 devaddress, UINT8 regaddress, UINT8* valuedest) {
    return StartReadFromReg(devaddress, regaddress, 1, valuedest);
}

bool I2c::StartWriteByteToReg(UINT8 devaddress, UINT8 regaddress, UINT8 value) {

    return false;
}

bool I2c::StartReadFromReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, UINT8* valuesdest) {
    if(isBusy()) return false;

    currentschema = READ_REG;
    this->deviceaddr = devaddress;
    this->regaddress = regaddress;
    this->dataptr = valuesdest;
    this->datalen = len;

    currentstatus = START_SENT; //better to set immediately so that an interrupt gets the right status
    if(I2CStart(module) != I2C_SUCCESS) {
        currentstatus = BUS_IDLE;
        return false;
    }


    return true;
}

bool I2c::StartWriteToReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, UINT8* values) {
    
    return false;
}