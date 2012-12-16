/*
 * File:   main.c
 * Author: georg
 *
 * Created on 22 ottobre 2012, 0.29
 */

#include "include/MPU-6050.h"
#include "include/i2cmanager.h"

MPU_6050::MPU_6050(I2c& busmanager, UINT8 busaddress ): i2cmanager(busmanager), i2caddr(busaddress) {

}

void MPU_6050::init() {

}


