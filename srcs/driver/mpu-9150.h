/*
 * Pic32CopterCpp -- A C++ microOS for the PIC32
 *
 * Copyright (C) 2012 - 2014, Georg Campana
 *
 * Georg Campana <g.campana(AT)stetel.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE.txt file
 * at the top of the source tree.
 *
 * File:   mpu-9150.h
 * Author: Georg Campana
 *
 * Created on 24 ottobre 2013, 11.39
 */

#ifndef MPU_9150_H
#define	MPU_9150_H

#include "../haldriver/i2cmanager.h"



class MPU_9150 : public SingleAccessDevice, public IEventObserver {

public:
    
    class FifoPacket {
     public:
        UInt32 timestamp_ms;
        Int16 AccelX, AccelY, AccelZ;
        Int16 GyroX, GyroY, GyroZ;
    };

    // includes temperature
    class MpuFifoPacket : public FifoPacket {
      public:
          Int16 Temp;

          const static UInt16 FifoPktLength = (2+2+2 +2+2+2 +2); // Accelx,y,z Gyrox,y,z, Temp
          //Int32 GetCelsiusIntegerX10() { return (Temp/34 +350); };
    };

    class DmpFifoPacket : public FifoPacket  {
      public:
          Int32 Quaternion1, Quaternion2, Quaternion3, Quaternion4;

          const static UInt16 FifoPktLength = (2+2+2 +2+2+2 +4+4+4); // inc quaternion
    };

    enum MPU9150_I2C_ADDR  {
        MPU9150_ADDRESS_AD0_LOW = 0x68,
        MPU9150_ADDRESS_AD0_HIGH = 0x69,
        MPU9150_DEFAULT_ADDRESS = MPU9150_ADDRESS_AD0_LOW
    };

    // Low passer filter cin config reg
    enum LOW_PASS_FILTER {
        CONFIG_DLPF_BW_256 = 0x00,
        CONFIG_DLPF_BW_188,
        CONFIG_DLPF_BW_98,
        CONFIG_DLPF_BW_42,
        CONFIG_DLPF_BW_20,
        CONFIG_DLPF_BW_10,
        CONFIG_DLPF_BW_5,
        CONFIG_DLPF_2100HZ_NOLPF,

        CONFIG_DLPF_MASK = 0x07
    };

    // Gyro FSR
    enum GYRO_FSR {
        GYRO_FS_250DPS  =0x00,
        GYRO_FS_500DPS  =0x08,
        GYRO_FS_1000DPS =0x10,
        GYRO_FS_2000DPS =0x18,
        GYRO_FS_MASK    =0x18 // only bits 3 and 4 are used for gyro full scale so use this to mask off other bits
    };

    // Gyro FSR
    enum ACCEL_FSR {
        ACCEL_FS_2G  =0x00,
        ACCEL_FS_4G  =0x08,
        ACCEL_FS_8G  =0x10,
        ACCEL_FS_16G =0x18,
        ACCEL_FS_MASK=0x18 // only bits 3 and 4 are used for acel full scale so use this to mask off other bits
    };

private:


    UInt8 i2caddr;
    bool interruptenabled;

    I2c& i2cmanager;

    const static UInt8 dmpMemory[];
    const static UInt8 dmpConfig[];
    const static UInt8 dmpUpdates[];

    UInt8 tempworkbuffer[128];

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
    UInt8 tmpregvalue;

    bool dmp_enabled;
    bool fifo_enabled;
    bool half_sensitivity;
    LOW_PASS_FILTER current_dlpf;
    MpuFifoPacket* mpudest;
    DmpFifoPacket* dmpdest;

  public:

    MPU_9150(I2c& busmanager, UInt8 busaddress = MPU9150_DEFAULT_ADDRESS );
    bool Init();

    void SetFifoDest(MpuFifoPacket* dest);
    void SetFifoDest(DmpFifoPacket* dest);

    bool EnableFifo();
    bool DisableFifo();
    bool EnableDmp();
    bool DisableDmp();

    bool EnableInterrupt(TaskBase* task2signal=NULL);
    bool DisableInterrupt();
    bool WaitForNextPacket(int maxms = -1);

    bool GetNextPacket();

    bool ReadFifoLength(UInt16* len);

// IEventObserver to get interrupt
    bool onEventFired(Int32 eventid);

  protected:
    bool Reset();
    bool CheckChipVersion();
    bool SetGyroFullscale(GYRO_FSR fsvalue);
    bool SetAccelFullScale(ACCEL_FSR fsvalue);
    bool SetLowPassFilter(LOW_PASS_FILTER filtervalue);
    bool SetSampleRate(UInt16 desiredrate);
    bool ConfigFifoData(UInt8 bitmask);

    bool ReadFromFifo(UInt8* destbuffer, UInt8 len);

    bool ConfigInterrupt();

    bool PushDmpFirmware();



};




#endif	/* MPU_9150_H */

