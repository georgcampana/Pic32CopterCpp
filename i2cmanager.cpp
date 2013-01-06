/*
 * File:   i2cmanager.c
 * Author: Georg Campana
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include "include/i2cmanager.h"

#include <p32xxxx.h>
#include <plib.h>

I2c* i2c1_ref;

#ifdef	__cplusplus
extern "C" {
#endif

void __ISR(_I2C_1_VECTOR, ipl5) I2cInterruptServiceRoutine(void)
{
//    INTClearFlag(INT_I2C1);
    INTClearFlag(INT_I2C1B);
    INTClearFlag(INT_I2C1M);
    i2c1_ref->handleInterrupt();
}

#ifdef	__cplusplus
}
#endif


void I2c::handleInterrupt() {
    switch(currentstatus) {
        case START_SENT:
        {   //Now we send the slave Device address + R/W bit
            UINT8 addr_rw = ((deviceaddr<<1) | 0x01); // r: bit0=1
            if(currentschema <= REG_BASED_SCHEMA ) {
                // w:  bit0 = 0  r: bit0=1
                addr_rw &= 0xfe ;
            }
            if(I2CSendByte(module, addr_rw) != I2C_MASTER_BUS_COLLISION)
            {
                currentstatus = S_ADDRESS_SENT;
            }
            else { setBusError(); }
            
            break;
        }

        case S_ADDRESS_SENT:
        {
            // Let's see if we found someone and got an ACK
            if(I2CByteWasAcknowledged(module) == TRUE) {
                if(I2CSendByte(module, regaddress) != I2C_MASTER_BUS_COLLISION)
                {
                    currentstatus = REG_ADDR_SENT;
                }
                else { setBusError(); }
            }
            else { setBusError(); }
            break;
        }

        case REG_ADDR_SENT:
        {
            // We send a restart in case of read procedure
            // but first we check if the sent reg_address was ACKed
            if(I2CByteWasAcknowledged(module) == TRUE) {

                if( currentschema == READ_REG ) {

                    if(I2CRepeatStart(module) != I2C_MASTER_BUS_COLLISION)
                    {
                        currentstatus = RESTART_SENT;
                    }
                    else { setBusError(); }
                }
                else { //Let's assume Write "schema" in this else
                    if(datalen>0) {
                        //We start to write data
                        if(I2CSendByte(module, *datatxptr) != I2C_MASTER_BUS_COLLISION) {
                            datatxptr++;
                            datalen--;
                        }
                        else { setBusError(); }
                    }

                    currentstatus = DATA_SENT;
                }
            }
            else { setBusError(); }

            break;
        }

        case RESTART_SENT:
        {
            //OK ready to send again the slave address in read mode this time
            if( currentschema == READ_REG ) {
                UINT8 addr_rw = ((deviceaddr<<1) | 0x01); // r: bit0=1

                if(I2CSendByte(module, addr_rw) != I2C_MASTER_BUS_COLLISION)
                {
                    currentstatus = RS_ADDRESS_SENT;
                }
                else { setBusError(); }
            }

            break;
        }

        case RS_ADDRESS_SENT:
        {
            // Let's see if got the ACK then it's time to read out data
            if(I2CByteWasAcknowledged(module) == TRUE) {

                if( currentschema == READ_REG ) {

                    if(I2CReceiverEnable(module,TRUE) != I2C_RECEIVE_OVERFLOW)
                    {
                        currentstatus = DATA_RECEIVE_ENABLED;
                    }
                    else { setBusError(); }

                }
            }
            else { setBusError(); }
            break;
        }

        case DATA_ACK_SENT:
        case DATA_RECEIVE_ENABLED:
        {
            // we read out one byte (or more) and send our ACK or NACK for the last
            if( currentschema == READ_REG ) {

                if(I2CReceivedDataIsAvailable(module) == TRUE) {
                    if(datalen>0) {
                        if(datarxptr != NULL) {
                            *datarxptr++ = I2CGetByte(module);
                        }
                        else { // in case of no dest just waste the read byte
                            I2CGetByte(module);
                        }
                        datalen--;
                    }
                    I2CAcknowledgeByte(module, (datalen==0)? FALSE:TRUE );
                    currentstatus = (datalen == 0) ? DATA_NACK_SENT : DATA_ACK_SENT ;
                }
                else { setBusError(); }
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
            currentstatus = BUS_IDLE ;
            break;
        }

        case DATA_SENT:
        {
            // Let's see if got the ACK and we continue to readout data
            if(I2CByteWasAcknowledged(module) == TRUE) {
                if( currentschema == WRITE_REG ) {

                    if(datalen>0) {
                        // Another byte to send
                        if(I2CSendByte(module, *datatxptr) != I2C_MASTER_BUS_COLLISION) {
                            datatxptr++;
                            datalen--;
                            // status is the same DATA_SENT ( we need to wait for the next ack)
                        }
                        else { setBusError(); }
                    }
                    else {
                        // there was nothing to send: we stop here
                        I2CStop(module);
                        currentstatus = STOP_SENT ;
                    }

                }
            }
            else { setBusError(); }

            break;
        }

        default:
            break;
    }

    return;
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
    buserror = false;

    if(mod == I2C1) {
        i2c1_ref = this;
    }

    // Set the I2C baudrate
    I2CSetFrequency(module, perif_freq, i2c_freq);
    
    I2CEnable(module,TRUE);

    while( !I2CBusIsIdle(module) );
    I2CClearStatus(module, I2C_ARBITRATION_LOSS );
    currentstatus = BUS_IDLE;
    
    setupInterrupt();

}


bool I2c::StartReadByteFromReg(UINT8 devaddress, UINT8 regaddress, UINT8* valuedest) {
    return StartReadFromReg(devaddress, regaddress, 1, valuedest);
}

bool I2c::StartWriteByteToReg(UINT8 devaddress, UINT8 regaddress, UINT8 value) {
    return StartWriteToReg(devaddress,regaddress, 1, &value);
}

bool I2c::StartReadFromReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, UINT8* valuesdest) {
    if(isBusy()) return true; // not ready

    resetAnyBusError();
    currentschema = READ_REG;
    this->deviceaddr = devaddress;
    this->regaddress = regaddress;
    this->datarxptr = valuesdest;
    this->datatxptr = 0;
    this->datalen = len;

    if(len==0) return false; // read zero bytes is done ;) --> not an error

    currentstatus = START_SENT; //better to set immediately so that an interrupt gets the right status
    if(I2CStart(module) != I2C_SUCCESS) {
        currentstatus = BUS_IDLE;
        setBusError();
    }

    return buserror;
}

bool I2c::StartWriteToReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, const UINT8* values) {
    if(isBusy()) return true; // not ready

    resetAnyBusError();
    currentschema = WRITE_REG;
    this->deviceaddr = devaddress;
    this->regaddress = regaddress;
    this->datatxptr = values;
    this->datarxptr = 0;
    this->datalen = len;

    if(len==0) return false; // write zero bytes is done ;) --> not an error

    currentstatus = START_SENT; //better to set immediately so that an interrupt gets the right status
    if(I2CStart(module) != I2C_SUCCESS) { // new status set in the line above
        currentstatus = BUS_IDLE;
        setBusError();
    }

    return buserror;
}

bool I2c::ReadByteFromReg(UINT8 devaddress, UINT8 regaddress, UINT8* valuedest) {
    StartReadByteFromReg(devaddress, regaddress, valuedest);
    while(isBusy() && buserror==false);
    return buserror;
}
bool I2c::WriteByteToReg(UINT8 devaddress, UINT8 regaddress, UINT8 value){
    StartWriteByteToReg(devaddress, regaddress, value);
    while(isBusy() && buserror==false);
    return buserror;
}

bool I2c::ReadFromReg(UINT8 devaddress, UINT8 regaddress, UINT16 len, UINT8* valuesdest) {
    StartReadFromReg(devaddress, regaddress, len, valuesdest);
    while(isBusy() && buserror==false);
    return buserror;
}

bool I2c::WriteToReg(UINT8 devaddreess, UINT8 regaddress, UINT16 len, const UINT8* values){
    StartWriteToReg(devaddreess, regaddress, len, values);
    while(isBusy() && buserror==false);
    return buserror;
}