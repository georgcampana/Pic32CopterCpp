
#include "mpu-9150.h"
#include "mpu-9150-regs.h"
#include "../kernel/kernel.h"

MPU_9150::MPU_9150(I2c& busmanager, UINT8 busaddress ) :
                        i2cmanager(busmanager), i2caddr(busaddress),
                        half_sensitivity(false), dmp_enabled(false),
                        current_dlpf(CONFIG_DLPF_BW_256) {
    
    
}

bool MPU_9150::Init() {

    if(Reset()) return true;

    if(CheckChipVersion()) return true;

    if(SetGyroFullscale(GYRO_FS_1000DPS))return true ;
    
    if(SetAccelFullScale(ACCEL_FS_2G))return true ;

    if(SetLowPassFilter(CONFIG_DLPF_BW_98))return true ;

    if(SetSampleRate(100))return true;

    if(dmp_enabled == false) {
        if(ConfigFifoData(0)) return true;
    }
}


bool MPU_9150::Reset() {
    // reset 
    bool error = i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_PWR_MGMT_1, BIT_PWR_MGMT_1_DEVICE_RESET);
    Kernel::DelayCurrentTask(100);
    if(error) return error;

    // wake up
    error = i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_PWR_MGMT_1, 0x00);

    return error;
}

bool MPU_9150::CheckChipVersion() {
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
    if(i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_GYRO_CONFIG, regvalue)){
        return true; // error
    }
    current_dlpf = filtervalue;
    
    return false; // OK
}

bool MPU_9150::SetSampleRate(UInt16 desiredrate) {
    UInt32 output_rate = 1000; // 1kHz
    if(current_dlpf == CONFIG_DLPF_BW_256 || current_dlpf == CONFIG_DLPF_2100HZ_NOLPF) {
        output_rate = 8000; // 8kHz (default)
        if(desiredrate < 32) desiredrate=32;
        else if(desiredrate > 8000) desiredrate=8000;
    }
    else {
        if(desiredrate < 4) desiredrate=4;
        else if(desiredrate > 1000) desiredrate=1000;
    }

    UInt8 regvalue;
    regvalue = (UInt8)((output_rate/desiredrate) -1); // range: 0-249
    //UInt16 actualrate = output_rate/(regvalue+1);

    return i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_SMPLRT_DIV, regvalue);
}

bool MPU_9150::ConfigFifoData(UInt32 bitmask) {

    if(dmp_enabled) return true; // fifo is filled by dmp not by mpu



}