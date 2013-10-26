/* 
 * File:   mpu-9150-regs.h
 * Author: georg
 *
 * Created on 24 ottobre 2013, 11.51
 */

#ifndef MPU_9150_REGS_H
#define	MPU_9150_REGS_H


enum MPU9150_REGS {
    REG_YG_OFFS_TC  = 0x01,
    REG_ACCELL_OFFSETS = 0x06

};


#if defined MPU6050
const struct gyro_reg_s reg = {
    .who_am_i       = 0x75,
    .rate_div       = 0x19,
    .lpf            = 0x1A,
    .prod_id        = 0x0C,
    .user_ctrl      = 0x6A,
    .fifo_en        = 0x23,
    .gyro_cfg       = 0x1B,
    .accel_cfg      = 0x1C,
    .motion_thr     = 0x1F,
    .motion_dur     = 0x20,
    .fifo_count_h   = 0x72,
    .fifo_r_w       = 0x74,
    .raw_gyro       = 0x43,
    .raw_accel      = 0x3B,
    .temp           = 0x41,
    .int_enable     = 0x38,
    .dmp_int_status = 0x39,
    .int_status     = 0x3A,
    .pwr_mgmt_1     = 0x6B,
    .pwr_mgmt_2     = 0x6C,
    .int_pin_cfg    = 0x37,
    .mem_r_w        = 0x6F,
    .accel_offs     = 0x06,
    .i2c_mst        = 0x24,
    .bank_sel       = 0x6D,
    .mem_start_addr = 0x6E,
    .prgm_start_h   = 0x70
#ifdef AK89xx_SECONDARY
    ,.raw_compass   = 0x49,

    .s0_addr        = 0x25,
    .s0_reg         = 0x26,
    .s0_ctrl        = 0x27,
    .s1_addr        = 0x28,
    .s1_reg         = 0x29,
    .s1_ctrl        = 0x2A,
    .s4_ctrl        = 0x34,
    .s0_do          = 0x63,
    .s1_do          = 0x64,
    .i2c_delay_ctrl = 0x67
#endif
};


#endif	/* MPU_9150_REGS_H */

