/*
 * File:   mpu-6050.cpp
 *
 * Work based on Jeff Rowberg's <jeff@rowberg.net> i2cdevlib
 * see at https://github.com/jrowberg/i2cdevlib
 *
 * Credits also to Noah Zerkin for the reverse enginering work
 * see at https://github.com/bzerk/MPU6050_DMP_6_axis_demo_
 *
 * Attempt is to adapt the code to the Microchip PIC32 platform
 * using an interrupt based i2c base class
 * and being more c++ish using the first Microchip C++ compiler
 * Author: georg
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include "include/MPU-6050.h"
#include "include/mpu-6050-regs.h"
#include "include/i2cmanager.h"

MPU_6050::MPU_6050(I2c& busmanager, UINT8 busaddress ): i2cmanager(busmanager), i2caddr(busaddress) {

}

bool MPU_6050::Init() {

    bool i2cerror = false;

    //reset the device
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_PWR_MGMT_1, BIT_PWR_MGMT_1_DEVICE_RESET);
    // we should wait a while here (100 ms should be more than enough
    System::DelayMS(100);

    // Set Gyro Z as clock source and switch off sleep bit
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_PWR_MGMT_1, BIT_PWR_MGMT_1_CLK_ZGYRO);

    // switch off standby of gyros and accel
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_PWR_MGMT_2, 0x00);

    // the code below could probably be disabled
    if(1){
        // in order to read the hw revision we switch to a special bank
        // switch to bank 0x10 type=user(0x20)with pefetching on(0x40) (whatever this means)
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_BANK_SEL, (0x10 | 0x20 | 0x40) ) ;

        // now we point to the right location in that bank (0x06)
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_MEM_START_ADDR, 0x06) ;

        // time to read the hw version/revision
        UINT8 hwrevision = 0xff;
        i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_MEM_R_W, &hwrevision);

        // back to the default memory page (bank=0
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_BANK_SEL, 0) ;

        // now we read if the OTP flag is valid (this is also undocumented and a mistery)
        UINT8 otpvalid = 0xff;
        i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_XG_OFFS_TC, &otpvalid);
    }


    // we read out the offsets of the gyros (6 bits starting from bit 6)
    UINT8 gyro_x_offset = 0;
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_XG_OFFS_TC, &gyro_x_offset);
    gyro_x_offset &= 0x3f;
    gyro_x_offset >>= 1;

    UINT8 gyro_y_offset = 0;
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_YG_OFFS_TC, &gyro_y_offset);
    gyro_y_offset &= 0x3f;
    gyro_y_offset >>= 1;

    UINT8 gyro_z_offset = 0;
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_ZG_OFFS_TC, &gyro_z_offset);
    gyro_z_offset &= 0x3f;
    gyro_z_offset >>= 1;


    return i2cerror;
}


void MPU_6050::setSampleRate(UINT8 rate) {
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_SMPLRT_DIV, rate);
}