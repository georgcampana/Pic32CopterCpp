
#include "mpu-9150.h"
#include "mpu-9150-regs.h"
#include "../kernel/kernel.h"

MPU_9150::MPU_9150(I2c& busmanager, UINT8 busaddress ) :
                        i2cmanager(busmanager), i2caddr(busaddress),
                        half_sensitivity(false), dmp_enabled(false),
                        fifo_enabled(false), current_dlpf(CONFIG_DLPF_BW_256),
                        mpudest(NULL), dmpdest(NULL) {
    
}

bool MPU_9150::Init() {

    if(Reset()) return true;

    if(CheckChipVersion()) return true;

    if(SetGyroFullscale(GYRO_FS_1000DPS))return true ;
    
    if(SetAccelFullScale(ACCEL_FS_2G))return true ;

    if(SetLowPassFilter(CONFIG_DLPF_BW_98))return true ;

    if(SetSampleRate(25))return true;

    if(dmp_enabled == true) {
        if(ConfigFifoData(0)) return true;
    }
    else {
        if(ConfigFifoData( BITS_FIFO_EN_TEMP  | BITS_FIFO_EN_ACCEL | BITS_FIFO_EN_GYROZ |
                           BITS_FIFO_EN_GYROY | BITS_FIFO_EN_GYROX )) return true;
    }




    return false; // everything went ok
}


bool MPU_9150::Reset() {
    // reset 
    bool error = i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_PWR_MGMT_1, BIT_PWR_MGMT_1_DEVICE_RESET);
    Kernel::DelayCurrentTask(100);
    if(error) return error;

    // wake up and set clock source to be the gyro
    error = i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_PWR_MGMT_1, 0x00 | BIT_PWR_MGMT_1_CLK_XGYRO);

    return error;
}

bool MPU_9150::CheckChipVersion() {
    // get revision

    UInt8 tmp[6];
    bool error = i2cmanager.ReadFromReg(i2caddr, MPU9150_RA_XA_OFFS_H, 6, tmp);
    if(error) return error;

    UInt8 revision = ((tmp[5] & 0x01) << 2) | ((tmp[3] & 0x01) << 1) | (tmp[1] & 0x01);

    if(revision == 0) {
        error = i2cmanager.ReadByteFromReg(i2caddr, MPU9150_RA_XA_OFFS_H, tmp);
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

    return error;
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
    if(i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_CONFIG, regvalue)){
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

bool MPU_9150::ConfigFifoData(UInt8 bitmask) {

    // if dmp is on this should be 0 because fifo is filled by dmp not by mpu


    return i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_FIFO_EN, bitmask);


}

bool MPU_9150::EnableFifo() {

    bool error = false;

    if(dmp_enabled) {
        error = i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_USER_CTRL, BIT_USER_CTRL_DMP_EN);
    }
    else {
        error = i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_USER_CTRL, BIT_USER_CTRL_FIFO_EN);
    }

    if(error == false) {
        fifo_enabled = true;
    }

    return error;
}

bool MPU_9150::DisableFifo() {

    // disable dmp and fifo
    bool error = i2cmanager.WriteByteToReg(i2caddr, MPU9150_RA_USER_CTRL, 0x00);

    if(error == false) {
        // dmp flag controlled by the other methods
        fifo_enabled = false;
    }

    return error;
}

void MPU_9150::SetFifoDest(MpuFifoPacket* dest) {
    mpudest = dest;
}

void MPU_9150::SetFifoDest(DmpFifoPacket* dest) {
    dmpdest = dest;
}


bool MPU_9150::GetNextPacket() {
    UInt16 currentlen;
    
    if(ReadFifoLength(&currentlen)) return true; // error reading

    if(dmp_enabled && dmpdest != NULL) {
        if(currentlen < DmpFifoPacket::FifoPktLength) {
            return true; // no enough data should never happen if interrupt based
        }
        UInt8 buff[DmpFifoPacket::FifoPktLength];
        if( i2cmanager.ReadFromReg(i2caddr, MPU9150_RA_FIFO_R_W,
                                   DmpFifoPacket::FifoPktLength, buff)) {
            return true;
        }
        dmpdest->Quaternion1 = buff[0]<<24  | buff[1] << 16  | buff[2]<<8  | buff[3];
        dmpdest->Quaternion2 = buff[4]<<24  | buff[5] << 16  | buff[6]<<8  | buff[7];
        dmpdest->Quaternion3 = buff[8]<<24  | buff[9] << 16  | buff[10]<<8 | buff[11];
        dmpdest->Quaternion4 = buff[12]<<24 | buff[13] << 16 | buff[14]<<8 | buff[15];
        dmpdest->AccelX = buff[16]<<8 | buff[17];
        dmpdest->AccelY = buff[18]<<8 | buff[19];
        dmpdest->AccelZ = buff[20]<<8 | buff[21];
        dmpdest->GyroX = buff[22]<<8 | buff[23];
        dmpdest->GyroY = buff[24]<<8 | buff[25];
        dmpdest->GyroZ = buff[26]<<8 | buff[27];

        dmpdest->timestamp_ms = SysTimer::GetNowMillisecs();
    }
    else
    if(!dmp_enabled && mpudest != NULL) {
        if(currentlen < MpuFifoPacket::FifoPktLength) {
            return true; // no enough data should never happen if interrupt based
        }
        UInt8 buff[MpuFifoPacket::FifoPktLength];
        if( i2cmanager.ReadFromReg(i2caddr, MPU9150_RA_FIFO_R_W,
                                   MpuFifoPacket::FifoPktLength,  buff)) {
            return true;
        }

        mpudest->AccelX = buff[0]<<8  | buff[1];
        mpudest->AccelY = buff[2]<<8  | buff[3];
        mpudest->AccelZ = buff[4]<<8  | buff[5];
        mpudest->Temp   = buff[6]<<8  | buff[7];
        mpudest->GyroX  = buff[8]<<8  | buff[9];
        mpudest->GyroY  = buff[10]<<8 | buff[11];
        mpudest->GyroZ  = buff[12]<<8 | buff[13];

        mpudest->timestamp_ms = SysTimer::GetNowMillisecs();
    }

    
    return false;

}

bool MPU_9150::ReadFifoLength(UInt16* len) {
    *len = 0;
    UInt8 tmp[2];
    // we read FifoLen High and then Low
    bool error = i2cmanager.ReadFromReg(i2caddr, MPU9150_RA_FIFO_COUNTH, 2, tmp);
    if(error) return error;

    *len = ((tmp[0]<<8) | tmp[1]);

    return false;
}


bool MPU_9150::PushDmpFirmware()  {


}



