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

#ifdef	__cplusplus
extern "C" {
#endif



#ifdef	__cplusplus
}
#endif


class I2c;

enum MPU6050_I2C_ADDR  {
    MPU6050_ADDRESS_AD0_LOW = 0x68,
    MPU6050_ADDRESS_AD0_HIGH = 0x69,
    MPU6050_DEFAULT_ADDRESS = MPU6050_ADDRESS_AD0_LOW
};


class MPU_6050 {
    UINT8 i2caddr;
    I2c& i2cmanager;

    const static UINT8 dmpMemory[];
    const static UINT8 dmpConfig[];
    const static UINT8 dmpUpdates[];

public:

    MPU_6050(I2c& busmanager, UINT8 busaddress = MPU6050_DEFAULT_ADDRESS );
    bool Init();

    UINT8 getDmpPacketSize() ;

protected:
    void setSampleRate(UINT8 rate);
    void writeMemory(const UINT8* data2write, UINT16 datalen, UINT8 frombank = 0, UINT8 fromindex = 0);
    void writeDmpConfigData(const UINT8* config2write, UINT16 cfgdatalen);


};

inline UINT8 MPU_6050::getDmpPacketSize() { return 48; }


#endif	/* MPU_6050_H */

