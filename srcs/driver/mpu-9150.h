/* 
 * File:   mpu-9150.h
 * Author: georg
 *
 * Created on 24 ottobre 2013, 11.39
 */

#ifndef MPU_9150_H
#define	MPU_9150_H

#include "i2cmanager.h"

enum MPU9150_I2C_ADDR  {
    MPU9150_ADDRESS_AD0_LOW = 0x68,
    MPU9150_ADDRESS_AD0_HIGH = 0x69,
    MPU9150_DEFAULT_ADDRESS = MPU9150_ADDRESS_AD0_LOW
};




class MPU_9150 : public SingleAccessDevice {

    UINT8 i2caddr;
    I2c& i2cmanager;

    const static UINT8 dmpMemory[];
    const static UINT8 dmpConfig[];
    const static UINT8 dmpUpdates[];

    UINT8 tempworkbuffer[128];

    // State machine for interrupt transfers using the i2c
    enum I2cStateMachine {
        I2C_NONE = 0,
        I2C_CTRL_READ,
        I2C_CTRL_WRITTEN
    } i2cstate;
    enum I2cTransType {
        I2C_TRANS_NONE = 0,
        I2C_DMP_ON,
        I2C_DMP_OFF
    } i2ctrans;
    // used across i2c transfers
    UINT8 tmpregvalue;

    bool dmp_enabled;
    bool half_sensitivity;
  public:

    MPU_9150(I2c& busmanager, UINT8 busaddress = MPU9150_DEFAULT_ADDRESS );
    bool Init();

  protected:
    bool Reset();
    bool checkChipVersion();
};




#endif	/* MPU_9150_H */

