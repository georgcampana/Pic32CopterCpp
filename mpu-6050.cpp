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
 * Author: Georg Campana
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include "include/MPU-6050.h"
#include "include/mpu-6050-regs.h"
#include "include/i2cmanager.h"

// this header could be expanded here but i prefer to have it separate
// to have the mainly the code here
#include "include/mpu-6050-dmpfw.h"



#define MPU6050_BANK_BURST_LEN 16  // if changed ammend the mask below too
#define MPU6050_BANK_BURST_LEN_MASK 0x0f // see above

MPU_6050::MPU_6050(I2c& busmanager, UINT8 busaddress ): i2cmanager(busmanager), i2caddr(busaddress) {

}

bool MPU_6050::Init() {

    bool i2cerror = false;

    //reset the device
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_PWR_MGMT_1, BIT_PWR_MGMT_1_DEVICE_RESET);
    // we should wait a while here (40 ms should be enough)
    System::DelayMS(40);

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

    // we use 6 axis only --> the external compass i2c interface is not needed
    // weird way to exclude it
    {
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_I2C_SLV0_ADDR, 0x7F);
        // switch off i2c master interface (set bypass from host i2c lines)
        UINT8 user_ctrl = 0x55;
        i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_USER_CTRL, &user_ctrl);
        user_ctrl &= (~BIT_USER_CTRL_I2C_MST_EN);
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);
        // set i2c slave address to myself (0x68)
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_I2C_SLV0_ADDR, i2caddr);
        // reset i2c master interface
        user_ctrl &= BIT_USER_CTRL_I2C_MST_RESET;
        i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);
        System::DelayMS(20);
    }

    // write DMP firmware into the bank mem
    writeMemory(dmpMemory, sizeof(dmpMemory));

    // write DMP config into bank memory
    writeDmpConfigData(dmpConfig, sizeof(dmpConfig));

    // set clock source to gyro z (done also earlier)
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_PWR_MGMT_1, BIT_PWR_MGMT_1_CLK_ZGYRO);

    // set interrupt on for DMP and FIFO overflow
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_INT_ENABLE, BIT_FIFO_OFLOW_INT | BIT_DMP_INT );

    // set sampling rate to 200Hz --> 4  in fact 1khz / (1 + 4) = 200 Hz
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_SMPLRT_DIV, 4 );

    // set external frame sync to TEMP (disabled) and DLPF (low filter) to 42Hz
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_CONFIG, /*MPUREG_CONFIG_EXT_SYNC_TEMP | */ MPUREG_CONFIG_DLPF_BW_42);

    return i2cerror;
}

void MPU_6050::writeMemory(const UINT8* dataptr, UINT16 datalen, UINT8 frombank, UINT8 fromaddr) {

    UINT8 burstlen;

    while(datalen>0) {
        // switch to bank the right bank
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_BANK_SEL, frombank ) ;
        // now we point to the right location in that bank 
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_MEM_START_ADDR, fromaddr) ;
        
        // let's burst until we are aligned with a chunck bundary
        burstlen = MPU6050_BANK_BURST_LEN - (fromaddr & MPU6050_BANK_BURST_LEN_MASK);
        // in case of less data than chunck lenght the chunk is the data lenght
        if(datalen < burstlen) burstlen = datalen;
        
        i2cmanager.WriteToReg(i2caddr, MPU6050_RA_MEM_R_W, burstlen, dataptr);
        dataptr += burstlen;
        datalen -= burstlen;
        
        fromaddr+=burstlen; // it's an octet so it will rotate every 256 (bank size)
        if(fromaddr==0) frombank++;
    }
}

void MPU_6050::writeDmpConfigData(const UINT8* configdata, UINT16 cfgdatalen) {

    UINT8 bank,address,len;

    while(cfgdatalen>0) {
        bank = *configdata++;
        address = *configdata++;
        len = *configdata++;
        writeMemory(configdata, len, bank, address);
        configdata += len;
        cfgdatalen -= (len + 3); // 3 = bank, address, len + [data]
    }
}

void MPU_6050::setSampleRate(UINT8 rate) {
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_SMPLRT_DIV, rate);
}


