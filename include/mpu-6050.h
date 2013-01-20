/* 
 * File:   mpu-6050.h
 * Author: Georg Campana
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
 *
 *
 * Created on 6 novembre 2012, 15.11
 */

#ifndef MPU_6050_H
#define	MPU_6050_H

#include <GenericTypeDefs.h>
#include "i2cmanager.h"


#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif

#define DMP_FIFO_PKT_LEN 48


enum MPU6050_I2C_ADDR  {
    MPU6050_ADDRESS_AD0_LOW = 0x68,
    MPU6050_ADDRESS_AD0_HIGH = 0x69,
    MPU6050_DEFAULT_ADDRESS = MPU6050_ADDRESS_AD0_LOW
};


class MPU_6050 : public I2c::EventListener {
    UINT8 i2caddr;
    I2c& i2cmanager;

    const static UINT8 dmpMemory[];
    const static UINT8 dmpConfig[];
    const static UINT8 dmpUpdates[];

    UINT8 tempworkbuffer[128];

public:

    MPU_6050(I2c& busmanager, UINT8 busaddress = MPU6050_DEFAULT_ADDRESS );
    bool Init();
    bool fillQuaternion();
    bool setFifoInterruptPin(UINT8 port, UINT8 pin);

protected:
    void setSampleRate(UINT8 rate);
    bool writeMemory(const UINT8* data2write, UINT16 datalen, bool verify=true, UINT8 frombank = 0, UINT8 fromindex = 0);
    void readMemIntoWorkBuffer( UINT16 datalen, UINT8 frombank, UINT8 fromindex);
    bool IsMemoryEqual(const UINT8* data2write, UINT16 datalen, UINT8 frombank, UINT8 fromindex);
    void writeDmpConfigData(const UINT8* config2write, UINT16 cfgdatalen);
    UINT16 getFifoCount();



public:
    // buffer[0]  - buffer[3] Quaternione w
    // buffer[4]  - buffer[7]  "      "   x
    // buffer[8]  - buffer[11]  "      "  y
    // buffer[12] - buffer[15]  "      "  z
    // buffer[16] - buffer[19]  Gyro  x
    // buffer[20] - buffer[23]  Gyro  y
    // buffer[24] - buffer[27]  Gyro  z
    // buffer[28] - buffer[31]  Accell  x
    // buffer[32] - buffer[35]  Accell  y
    // buffer[36] - buffer[39]  Accell  z
    // buffer[40] - buffer[41]  maybe Temperature ????????

    class DmpDataPacket {
        UINT8 buffer[DMP_FIFO_PKT_LEN];

        public:

            DmpDataPacket();

            UINT8 getPacketSize();
            UINT8* getBuffer2Fill();


    };

};

inline UINT8 MPU_6050::DmpDataPacket::getPacketSize() { return DMP_FIFO_PKT_LEN; }
inline UINT8* MPU_6050::DmpDataPacket::getBuffer2Fill() { return buffer; }

#endif	/* MPU_6050_H */

