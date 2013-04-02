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
    // we should wait a while here (50 ms should be enough)
    System::DelayMS(50);

    // more check code power mngmt1 should be 0x40 after a reset (sleep enabled)
    UINT8 powermngmt1;
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_PWR_MGMT_1, &powermngmt1);

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

        // back to the default memory page (bank=0)
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_BANK_SEL, 0) ;

        // now we read if the OTP flag is valid (this is also undocumented and a mistery)
        UINT8 otpvalid = 0xff;
        i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_XG_OFFS_TC, &otpvalid);
        if(otpvalid & MPU6050_TC_OTP_BNK_VLD_BIT) DBPRINTF("OTP bank is valid") ;
    }


    // we read out the offsets of the gyros (6 bits starting from bit 6)
    UINT8 current_xg_offs; // we need this value later when setting OTP to false
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_XG_OFFS_TC, &current_xg_offs);
    UINT8 gyro_x_offset = current_xg_offs;
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
        user_ctrl |= BIT_USER_CTRL_I2C_MST_RESET;
        i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);
        System::DelayMS(20);
    }

    // write DMP firmware into the bank mem
    bool verified = writeMemory(dmpMemory, sizeof(dmpMemory));
    verified = true;

    // write DMP config into bank memory
    writeDmpConfigData(dmpConfig, sizeof(dmpConfig));

    // set clock source to gyro z (done also earlier)
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_PWR_MGMT_1, BIT_PWR_MGMT_1_CLK_ZGYRO);

    // set interrupt on for DMP and FIFO overflow
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_INT_ENABLE, BIT_FIFO_OFLOW_EN | BIT_DMP_INT_EN );

    // set sampling rate to 200Hz --> 4 ; in fact 1khz / (1 + 4) = 200 Hz
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_SMPLRT_DIV, 4 );

    // set external frame sync to TEMPerature  (disabled) and DLPF (low filter) to 42Hz
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_CONFIG, /*MPUREG_CONFIG_EXT_SYNC_TEMP | */ MPUREG_CONFIG_DLPF_BW_42);

    // set full range gyro scale +/+2000 degrees/sec
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_GYRO_CONFIG, BITS_GYRO_FS_2000DPS );

    // set DMP configs, who knows what this is :(
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_DMP_CFG_1, 0x03);
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_DMP_CFG_2, 0x00);

    // clear OTP flag & restore XG_offset (did we ever change it ?)
    current_xg_offs &= (~MPU6050_TC_OTP_BNK_VLD_BIT); // clear OTP flag
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_XG_OFFS_TC, current_xg_offs);

    // restore also YG and ZG offsets (realigned to right bit in the reg)
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_YG_OFFS_TC, gyro_y_offset<<1);
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_ZG_OFFS_TC, gyro_z_offset<<1);

    // set user offsets to 0
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_XG_OFFS_USRH, 0);
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_YG_OFFS_USRH, 0);
    i2cerror = i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_ZG_OFFS_USRH, 0);


    // set DMP update 1 of 7
    const UINT8* updatedata = dmpUpdates;
    writeDmpConfigData(updatedata, updatedata[2]+3);
    updatedata += updatedata[2]+3;

    // set DMP update 2 of 7
    writeDmpConfigData(updatedata, updatedata[2]+3);
    updatedata += updatedata[2]+3;

    // reset FIFO
    UINT8 user_ctrl = 0x55;
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_USER_CTRL, &user_ctrl);
    user_ctrl |= BIT_USER_CTRL_FIFO_RESET;
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);

    // get FIFO counter ;
    UINT16 fifocounter = getFifoCount();

    // we flush by wasting the bytes in the FIFO buffer passing a null dest
    i2cmanager.ReadFromReg(i2caddr, MPU6050_RA_FIFO_R_W, fifocounter,NULL);

    // set motion detection threshold to 2
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_MOT_THR, 2);

    // set zero motion detection to 156
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_ZRMOT_THR, 156);

    // set motion detection duration to 80
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_MOT_DUR, 80);

    // set zero motion detection duration to 0
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_ZRMOT_DUR, 0);

    // reset FIFO
    user_ctrl = 0x55;
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_USER_CTRL, &user_ctrl);
    user_ctrl |= BIT_USER_CTRL_FIFO_RESET;
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);

    // enable FIFO
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_USER_CTRL, &user_ctrl);
    user_ctrl |= BIT_USER_CTRL_FIFO_EN;
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);

    // enable DMP
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_USER_CTRL, &user_ctrl);
    user_ctrl |= BIT_USER_CTRL_DMP_EN;
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);

    // reset DMP
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_USER_CTRL, &user_ctrl);
    user_ctrl |= BIT_USER_CTRL_DMP_RESET;
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);

    // set DMP update 3 of 7
    writeDmpConfigData(updatedata, updatedata[2]+3);
    updatedata += updatedata[2]+3;

    // set DMP update 4 of 7
    writeDmpConfigData(updatedata, updatedata[2]+3);
    updatedata += updatedata[2]+3;

    // set DMP update 5 of 7
    writeDmpConfigData(updatedata, updatedata[2]+3);
    updatedata += updatedata[2]+3;

    // wait to get at least 3 octets (assuming we have less than 256)
    for(fifocounter = 0; fifocounter<3; ) {
        fifocounter = getFifoCount();
    }

    // we flush by wasting the bytes in the FIFO buffer passing a null dest
    i2cerror = i2cmanager.ReadFromReg(i2caddr, MPU6050_RA_FIFO_R_W, fifocounter,NULL);

    // get the Interrupt status to clear pending INTS
    UINT8 intstatus;
    i2cerror = i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_INT_STATUS, &intstatus);

    // set DMP update 6 of 7 (actually a strange read )
    writeDmpConfigData(updatedata, updatedata[2]+3);
    updatedata += updatedata[2]+3;

    // wait to get at least 3 octets (assuming we have less than 256)
    for(fifocounter = 0; fifocounter<3; ) {
        fifocounter = getFifoCount();
    }

    // we flush by wasting the bytes in the FIFO buffer passing a null dest
    i2cerror = i2cmanager.ReadFromReg(i2caddr, MPU6050_RA_FIFO_R_W, fifocounter, NULL);

    // get the Interrupt status to clear pending INTS
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_INT_STATUS, &intstatus);

    // set DMP update 7 of 7
    writeDmpConfigData(updatedata, updatedata[2]+3);
    updatedata += updatedata[2]+3;

    // we should be ready here
    // let's switch off dmp , flush fifo and clear interrupt

    // disable DMP
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_USER_CTRL, &user_ctrl);
    user_ctrl &= (~BIT_USER_CTRL_DMP_EN);
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);
    // reset fifo
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_USER_CTRL, &user_ctrl);
    user_ctrl |= BIT_USER_CTRL_FIFO_RESET;
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_USER_CTRL, user_ctrl);
    // reset interrupt
    i2cmanager.ReadByteFromReg(i2caddr, MPU6050_RA_INT_STATUS, &intstatus);

    return i2cerror;
}

bool MPU_6050::writeMemory(const UINT8* dataptr, UINT16 datalen, bool verify, UINT8 frombank, UINT8 fromaddr) {

    UINT8 burstlen;
    bool verified = true;

    while(datalen>0) {
        // let's burst until we are aligned with a chunck bundary
        burstlen = MPU6050_BANK_BURST_LEN - (fromaddr & MPU6050_BANK_BURST_LEN_MASK);
        // in case of less data than chunck lenght the chunk is the data lenght
        if(datalen < burstlen) burstlen = datalen;
         // switch to the right bank
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_BANK_SEL, frombank ) ;
        // now we point to the right location in that bank
        i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_MEM_START_ADDR, fromaddr) ;

        i2cmanager.WriteToReg(i2caddr, MPU6050_RA_MEM_R_W, burstlen, dataptr);

        if(verify) {
            if(!IsMemoryEqual(dataptr, burstlen, frombank, fromaddr)) {
                verified = false;
                break;
            }
        }

        dataptr += burstlen;
        datalen -= burstlen;
        
        fromaddr+=burstlen; // fromaddr is an octet so it will rotate every 256 (bank size)
        if(fromaddr==0) frombank++;
    }

    return verified;
}

bool MPU_6050::IsMemoryEqual(const UINT8* dataptr, UINT16 datalen, UINT8 frombank, UINT8 fromaddr) {

    UINT8 burstlen;
    bool isequal = true;

    while(datalen>0) {
        // let's burst until we are aligned with a chunck bundary
        burstlen = MPU6050_BANK_BURST_LEN - (fromaddr & MPU6050_BANK_BURST_LEN_MASK);
        // in case of less data than chunck lenght the chunk is the data lenght
        if(datalen < burstlen) burstlen = datalen;

        readMemIntoWorkBuffer(burstlen, frombank, fromaddr);

        for(UINT16 c=0; c<burstlen; c++) {
            if(tempworkbuffer[c] != dataptr[c]) {
                isequal = false;
                break;
            }
        }

        if(isequal == false) break;

        datalen -= burstlen;
    }

    return isequal;
}

void MPU_6050::writeDmpConfigData(const UINT8* configdata, UINT16 cfgdatalen) {

    UINT8 bank,address,len;

    while(cfgdatalen>0) {
        bank = *configdata++;
        address = *configdata++;
        len = *configdata++;
        if(len == 0) { // special case used to enable DMP interrupts
            if(*configdata == 0x01) {
                i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_INT_ENABLE,
                                         BIT_DMP_INT_EN | BIT_FIFO_OFLOW_EN | BIT_ZMOT_EN) ;
            }
            configdata++; // because in the special case the len is 0 but there
            cfgdatalen--; // there is one data byte in fact
        }
        else {
            writeMemory(configdata, len, false, bank, address);
        }
        configdata += len;
        cfgdatalen -= (len + 3); // 3 = (bank, address, len) + len[data]
    }
}

void MPU_6050::setSampleRate(UINT8 rate) {
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_SMPLRT_DIV, rate);
}


UINT16 MPU_6050::getFifoCount() {
    UINT8 buffer[2];
    i2cmanager.ReadFromReg(i2caddr, MPU6050_RA_FIFO_COUNTH, 2, buffer);
    return (((UINT16)buffer[0]) << 8) | buffer[1];
}

void MPU_6050::readMemIntoWorkBuffer( UINT16 datalen, UINT8 frombank, UINT8 fromindex) {
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_BANK_SEL, frombank ) ;
    // now we point to the right location in that bank
    i2cmanager.WriteByteToReg(i2caddr, MPU6050_RA_MEM_START_ADDR, fromindex) ;
    i2cmanager.ReadFromReg(i2caddr, MPU6050_RA_MEM_R_W, datalen, tempworkbuffer);
}

bool MPU_6050::setFifoInterruptPin(UINT8 port, UINT8 pin) {

    // set pin to receive the interrupt from the MPU6050 and generate the
    // correspondent interrupt here in the driver to fetch the data
    // and/or inform the device client

}