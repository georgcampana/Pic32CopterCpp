
#include "mpu-9150.h"
#include "mpu-9150-regs.h"
#include "../kernel/kernel.h"

bool MPU_9150::Reset() {
    // reset 
    bool error = i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_PWR_MGMT_1, BIT_PWR_MGMT_1_DEVICE_RESET);
    Kernel::DelayCurrentTask(100);
    if(error) return error;

    // wake up
    error = i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_PWR_MGMT_1, 0x00);

    return error;
}

bool MPU_9150::checkChipVersion() {
    // get revision

    UInt8 tmp[6];
    bool error = i2cmanager.ReadFromReg(i2caddr, MPU9150_RA_XA_OFFS_H, 6, tmp);
    if(error) return error;

    UInt8 revision = ((tmp[5] & 0x01) << 2) | ((tmp[3] & 0x01) << 1) | (tmp[1] & 0x01);

    if(revision == 0) {
        bool error = i2cmanager.ReadByteFromReg(i2caddr, MPU9150_RA_XA_OFFS_H, tmp);
        if(error) return error;
        revision = tmp[0] & 0x0F;
        if(revision==0) {
            // not suitable device
            error = true;
        }else if(revision==4) {
            half_sensitivity = true;
        }
    }
    else {
        if(revision == 1) {half_sensitivity = true;}
        else {
            if(revision != 2) {
               // unknown device
               error = true;
            }
        }
    }
}

bool MPU_9150::SetGyroFullscale(GYRO_FSR fsvalue) {
   return i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_GYRO_CONFIG, (UInt8)fsvalue);
}

bool MPU_9150::SetAccelFullScale(ACCEL_FSR fsvalue) {
    return i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_ACCEL_CONFIG, (UInt8)fsvalue);
}

bool MPU_9150::SetLowPassFilter(LOW_PASS_FILTER filtervalue) {
    UInt8 regvalue = (UInt8)filtervalue;

    // TODO: consider sync flags to "or" bitwise
    return i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_GYRO_CONFIG, regvalue);
}

bool MPU_9150::SetSampleRate(UInt16 desiredrate) {

}

bool MPU_9150::ConfigFifoData(UInt32 bitmask) {

}