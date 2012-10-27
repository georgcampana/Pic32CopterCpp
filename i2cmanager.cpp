/*
 * File:   main.c
 * Author: georg
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include "include/i2cmanager.h"

#ifdef	__cplusplus
extern "C" {
#endif

void __ISR(_I2C_1_VECTOR, ipl5) I2cInterruptServiceRoutine(void)
{
    INTClearFlag(INT_I2C1);
}

#ifdef	__cplusplus
}
#endif

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

}


bool I2c::StartReadByteFromReg(UINT8 devaddreess, UINT8 regaddress, UINT8* valuedest) {
    if(isBusy()) return false;

    if(I2CStart(module) != I2C_SUCCESS) {

        return false;
    }

    currentstatus = START_SENT;

    return false;
}

bool I2c::StartWriteByteToReg(UINT8 devaddreess, UINT8 regaddress, UINT8 value) {

    return false;
}

bool I2c::StartReadFromReg(UINT8 devaddreess, UINT8 regaddress, UINT16 len, UINT8* valuesdest) {

    return false;
}

bool I2c::StartWriteToReg(UINT8 devaddreess, UINT8 regaddress, UINT16 len, UINT8* values) {
    
    return false;
}