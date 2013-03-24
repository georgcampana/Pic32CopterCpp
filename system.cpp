/*
 * File:   system.cpp
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

#include "include/system.h"


 void System::DelayMS(UINT16 ms2delay) {


     unsigned int int_status;
     while( ms2delay-- )
     {
         int_status = INTDisableInterrupts();
         OpenCoreTimer(GetSystemClock() / 2000);
         INTRestoreInterrupts(int_status);
         mCTClearIntFlag();
         while( !mCTGetIntFlag() );
     }
     mCTClearIntFlag();


}

int System::dbgcounter = 0;
