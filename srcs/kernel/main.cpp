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
 * File:   main.c
 * Author: Georg
 *
 * Created on 14 ottobre 2012, 18.54
 */

#include "hal/hal.h"
#include "kernel.h"

#include "../maintask.h"


extern MainTask parenttask;

int main(void) {

    // this should only initialize fundamental kernel resources
    // like the hw timer used as the systimer
    HAL::Init();

    // now it's the turn of the Kernel Systimer
    SysTimer::Start();

    // This will never come back
    Kernel::InitAndStartMainTask(&parenttask);

    return 0;
}

