/*
 * File:   mpu-6050-regs.h
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
 * Created on 23 dicembre 2012, 11.53
 */

#ifndef MPU_6050_REGS_H
#define	MPU_6050_REGS_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif



enum MPU6050_REGS
{
    MPU6050_RA_XG_OFFS_TC       =0x00, //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
    MPU6050_RA_YG_OFFS_TC       =0x01, //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
    MPU6050_RA_ZG_OFFS_TC       =0x02, //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
    MPU6050_RA_X_FINE_GAIN      =0x03, //[7:0] X_FINE_GAIN
    MPU6050_RA_Y_FINE_GAIN      =0x04, //[7:0] Y_FINE_GAIN
    MPU6050_RA_Z_FINE_GAIN      =0x05, //[7:0] Z_FINE_GAIN
    MPU6050_RA_XA_OFFS_H        =0x06, //[15:0] XA_OFFS
    MPU6050_RA_XA_OFFS_L_TC     =0x07,
    MPU6050_RA_YA_OFFS_H        =0x08, //[15:0] YA_OFFS
    MPU6050_RA_YA_OFFS_L_TC     =0x09,
    MPU6050_RA_ZA_OFFS_H        =0x0A, //[15:0] ZA_OFFS
    MPU6050_RA_ZA_OFFS_L_TC     =0x0B,
    MPU6050_RA_XG_OFFS_USRH     =0x13, //[15:0] XG_OFFS_USR
    MPU6050_RA_XG_OFFS_USRL     =0x14,
    MPU6050_RA_YG_OFFS_USRH     =0x15, //[15:0] YG_OFFS_USR
    MPU6050_RA_YG_OFFS_USRL     =0x16,
    MPU6050_RA_ZG_OFFS_USRH     =0x17, //[15:0] ZG_OFFS_USR
    MPU6050_RA_ZG_OFFS_USRL     =0x18,
    MPU6050_RA_SMPLRT_DIV       =0x19,
#       define MPUREG_SMPLRT_1000HZ                             0x00
#       define MPUREG_SMPLRT_500HZ                              0x01
#       define MPUREG_SMPLRT_250HZ                              0x03
#       define MPUREG_SMPLRT_200HZ                              0x04
#       define MPUREG_SMPLRT_100HZ                              0x09
#       define MPUREG_SMPLRT_50HZ                               0x13
    MPU6050_RA_CONFIG           =0x1A,
#       define MPUREG_CONFIG_EXT_SYNC_MASK                      0x38
#       define MPUREG_CONFIG_EXT_SYNC_OFF                       0x00
#       define MPUREG_CONFIG_EXT_SYNC_TEMP                      0x08
#       define MPUREG_CONFIG_EXT_SYNC_GYROX                     0x10
#       define MPUREG_CONFIG_EXT_SYNC_GYROY                     0x18
#       define MPUREG_CONFIG_EXT_SYNC_GYROZ                     0x20
#       define MPUREG_CONFIG_EXT_SYNC_ACCX                      0x28
#       define MPUREG_CONFIG_EXT_SYNC_ACCY                      0x30
#       define MPUREG_CONFIG_EXT_SYNC_ACCZ                      0x38
#       define MPUREG_CONFIG_DLPF_MASK                          0x07
#       define MPUREG_CONFIG_DLPF_BW_256                        0x00
#       define MPUREG_CONFIG_DLPF_BW_188                        0x01
#       define MPUREG_CONFIG_DLPF_BW_98                         0x02
#       define MPUREG_CONFIG_DLPF_BW_42                         0x03
#       define MPUREG_CONFIG_DLPF_BW_20                         0x04
#       define MPUREG_CONFIG_DLPF_BW_10                         0x05
#       define MPUREG_CONFIG_DLPF_BW_5                          0x06
    MPU6050_RA_GYRO_CONFIG      =0x1B,
#       define BITS_GYRO_FS_250DPS                              0x00
#       define BITS_GYRO_FS_500DPS                              0x08
#       define BITS_GYRO_FS_1000DPS                             0x10
#       define BITS_GYRO_FS_2000DPS                             0x18
#       define BITS_GYRO_FS_MASK                                0x18    // only bits 3 and 4 are used for gyro full scale so use this to mask off other bits
#       define BITS_GYRO_ZGYRO_SELFTEST                         0x20
#       define BITS_GYRO_YGYRO_SELFTEST                         0x40
#       define BITS_GYRO_XGYRO_SELFTEST                         0x80
    MPU6050_RA_ACCEL_CONFIG     =0x1C,
    MPU6050_RA_FF_THR           =0x1D,
    MPU6050_RA_FF_DUR           =0x1E,
    MPU6050_RA_MOT_THR          =0x1F,
    MPU6050_RA_MOT_DUR          =0x20,
    MPU6050_RA_ZRMOT_THR        =0x21,
    MPU6050_RA_ZRMOT_DUR        =0x22,
    MPU6050_RA_FIFO_EN          =0x23,
    MPU6050_RA_I2C_MST_CTRL     =0x24,
    MPU6050_RA_I2C_SLV0_ADDR    =0x25,
    MPU6050_RA_I2C_SLV0_REG     =0x26,
    MPU6050_RA_I2C_SLV0_CTRL    =0x27,
    MPU6050_RA_I2C_SLV1_ADDR    =0x28,
    MPU6050_RA_I2C_SLV1_REG     =0x29,
    MPU6050_RA_I2C_SLV1_CTRL    =0x2A,
    MPU6050_RA_I2C_SLV2_ADDR    =0x2B,
    MPU6050_RA_I2C_SLV2_REG     =0x2C,
    MPU6050_RA_I2C_SLV2_CTRL    =0x2D,
    MPU6050_RA_I2C_SLV3_ADDR    =0x2E,
    MPU6050_RA_I2C_SLV3_REG     =0x2F,
    MPU6050_RA_I2C_SLV3_CTRL    =0x30,
    MPU6050_RA_I2C_SLV4_ADDR    =0x31,
    MPU6050_RA_I2C_SLV4_REG     =0x32,
    MPU6050_RA_I2C_SLV4_DO      =0x33,
    MPU6050_RA_I2C_SLV4_CTRL    =0x34,
    MPU6050_RA_I2C_SLV4_DI      =0x35,
    MPU6050_RA_I2C_MST_STATUS   =0x36,
    MPU6050_RA_INT_PIN_CFG      =0x37,
#       define BIT_INT_RD_CLEAR                                 0x10    // clear the interrupt when any read occurs
    MPU6050_RA_INT_ENABLE       =0x38,
#       define BIT_RAW_RDY_EN                                   0x01
#       define BIT_DMP_INT_EN                                   0x02    // enabling this bit (DMP_INT_EN) also enables RAW_RDY_EN it seems
#       define BIT_UNKNOWN_INT_EN                               0x04
#       define BIT_I2C_MST_INT_EN                               0x08
#       define BIT_FIFO_OFLOW_EN                                0x10
#       define BIT_ZMOT_EN                                      0x20
#       define BIT_MOT_EN                                       0x40
#       define BIT_FF_EN                                        0x80
    MPU6050_RA_DMP_INT_STATUS   =0x39,
    MPU6050_RA_INT_STATUS       =0x3A,
    //(same bit pattern as above because this register shows what interrupt actually fired)
#       define BIT_RAW_RDY_INT                                  0x01
#       define BIT_DMP_INT                                      0x02
#       define BIT_UNKNOWN_INT                                  0x04
#       define BIT_I2C_MST_INT                                  0x08
#       define BIT_FIFO_OFLOW_INT                               0x10
#       define BIT_ZMOT_INT                                     0x20
#       define BIT_MOT_INT                                      0x40
#       define BIT_FF_INT                                       0x80
    MPU6050_RA_ACCEL_XOUT_H     =0x3B,
    MPU6050_RA_ACCEL_XOUT_L     =0x3C,
    MPU6050_RA_ACCEL_YOUT_H     =0x3D,
    MPU6050_RA_ACCEL_YOUT_L     =0x3E,
    MPU6050_RA_ACCEL_ZOUT_H     =0x3F,
    MPU6050_RA_ACCEL_ZOUT_L     =0x40,
    MPU6050_RA_TEMP_OUT_H       =0x41,
    MPU6050_RA_TEMP_OUT_L       =0x42,
    MPU6050_RA_GYRO_XOUT_H      =0x43,
    MPU6050_RA_GYRO_XOUT_L      =0x44,
    MPU6050_RA_GYRO_YOUT_H      =0x45,
    MPU6050_RA_GYRO_YOUT_L      =0x46,
    MPU6050_RA_GYRO_ZOUT_H      =0x47,
    MPU6050_RA_GYRO_ZOUT_L      =0x48,
    MPU6050_RA_EXT_SENS_DATA_00 =0x49,
    MPU6050_RA_EXT_SENS_DATA_01 =0x4A,
    MPU6050_RA_EXT_SENS_DATA_02 =0x4B,
    MPU6050_RA_EXT_SENS_DATA_03 =0x4C,
    MPU6050_RA_EXT_SENS_DATA_04 =0x4D,
    MPU6050_RA_EXT_SENS_DATA_05 =0x4E,
    MPU6050_RA_EXT_SENS_DATA_06 =0x4F,
    MPU6050_RA_EXT_SENS_DATA_07 =0x50,
    MPU6050_RA_EXT_SENS_DATA_08 =0x51,
    MPU6050_RA_EXT_SENS_DATA_09 =0x52,
    MPU6050_RA_EXT_SENS_DATA_10 =0x53,
    MPU6050_RA_EXT_SENS_DATA_11 =0x54,
    MPU6050_RA_EXT_SENS_DATA_12 =0x55,
    MPU6050_RA_EXT_SENS_DATA_13 =0x56,
    MPU6050_RA_EXT_SENS_DATA_14 =0x57,
    MPU6050_RA_EXT_SENS_DATA_15 =0x58,
    MPU6050_RA_EXT_SENS_DATA_16 =0x59,
    MPU6050_RA_EXT_SENS_DATA_17 =0x5A,
    MPU6050_RA_EXT_SENS_DATA_18 =0x5B,
    MPU6050_RA_EXT_SENS_DATA_19 =0x5C,
    MPU6050_RA_EXT_SENS_DATA_20 =0x5D,
    MPU6050_RA_EXT_SENS_DATA_21 =0x5E,
    MPU6050_RA_EXT_SENS_DATA_22 =0x5F,
    MPU6050_RA_EXT_SENS_DATA_23 =0x60,
    MPU6050_RA_MOT_DETECT_STATUS =0x61,
    MPU6050_RA_I2C_SLV0_DO      =0x63,
    MPU6050_RA_I2C_SLV1_DO      =0x64,
    MPU6050_RA_I2C_SLV2_DO      =0x65,
    MPU6050_RA_I2C_SLV3_DO      =0x66,
    MPU6050_RA_I2C_MST_DELAY_CTRL   =0x67,
    MPU6050_RA_SIGNAL_PATH_RESET    =0x68,
    MPU6050_RA_MOT_DETECT_CTRL      =0x69,
    MPU6050_RA_USER_CTRL        =0x6A,
#       define BIT_USER_CTRL_SIG_COND_RESET             0x01            // resets signal paths and results registers for all sensors (gyros, accel, temp)
#       define BIT_USER_CTRL_I2C_MST_RESET              0x02            // reset I2C Master (only applicable if I2C_MST_EN bit is set)
#       define BIT_USER_CTRL_FIFO_RESET                 0x04            // Reset (i.e. clear) FIFO buffer
#       define BIT_USER_CTRL_DMP_RESET                  0x08            // Reset DMP
#       define BIT_USER_CTRL_I2C_IF_DIS                 0x10            // Disable primary I2C interface and enable hal.spi->interface
#       define BIT_USER_CTRL_I2C_MST_EN                 0x20            // Enable MPU to act as the I2C Master to external slave sensors
#       define BIT_USER_CTRL_FIFO_EN                    0x40            // Enable FIFO operations
#       define BIT_USER_CTRL_DMP_EN                     0x80            // Enable DMP operations
    MPU6050_RA_PWR_MGMT_1       =0x6B,
#       define BIT_PWR_MGMT_1_CLK_INTERNAL              0x00            // clock set to internal 8Mhz oscillator
#       define BIT_PWR_MGMT_1_CLK_XGYRO                 0x01            // PLL with X axis gyroscope reference
#       define BIT_PWR_MGMT_1_CLK_YGYRO                 0x02            // PLL with Y axis gyroscope reference
#       define BIT_PWR_MGMT_1_CLK_ZGYRO                 0x03            // PLL with Z axis gyroscope reference
#       define BIT_PWR_MGMT_1_CLK_EXT32KHZ              0x04            // PLL with external 32.768kHz reference
#       define BIT_PWR_MGMT_1_CLK_EXT19MHZ              0x05            // PLL with external 19.2MHz reference
#       define BIT_PWR_MGMT_1_CLK_STOP                  0x07            // Stops the clock and keeps the timing generator in reset
#       define BIT_PWR_MGMT_1_TEMP_DIS                  0x08            // disable temperature sensor
#       define BIT_PWR_MGMT_1_CYCLE                     0x20            // put sensor into cycle mode.  cycles between sleep mode and waking up to take a single sample of data from active sensors at a rate determined by LP_WAKE_CTRL
#       define BIT_PWR_MGMT_1_SLEEP                     0x40            // put sensor into low power sleep mode
#       define BIT_PWR_MGMT_1_DEVICE_RESET              0x80            // reset entire device
    MPU6050_RA_PWR_MGMT_2       =0x6C,
    MPU6050_RA_BANK_SEL         =0x6D,
    MPU6050_RA_MEM_START_ADDR   =0x6E,
    MPU6050_RA_MEM_R_W          =0x6F,
    MPU6050_RA_DMP_CFG_1        =0x70,
    MPU6050_RA_DMP_CFG_2        =0x71,
    MPU6050_RA_FIFO_COUNTH      =0x72,
    MPU6050_RA_FIFO_COUNTL      =0x73,
    MPU6050_RA_FIFO_R_W         =0x74,
    MPU6050_RA_WHO_AM_I         =0x75

};



#endif	/* MPU_6050_REGS_H */

