/*
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

