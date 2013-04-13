/*
 * File:   newmain.c
 * Author: Georg
 *
 * Created on 14 ottobre 2012, 18.54
 */


#include "include/system.h"

#include "include/i2cmanager.h"
#include "include/mpu-6050.h"
#include "include/uartmanager.h"
#include "include/digitaliomanager.h"
#include "include/dbgout.h"


#define I2C_CLOCK_FREQ      (100000) // tested to work up to 400KHz
#define UART_BAUD_RATE      (115200)

// I2C driver
I2c i2c_mod_1(I2C1, GetPeripheralClock(), I2C_CLOCK_FREQ );
// Invensense sensor, with the interrupt line connected to CN15/RD6
MPU_6050 motionsensor(i2c_mod_1);
InputPin mpu6050intpin(IOPORT_D, BIT_6);
PinChangeHandler mpu6050datardy(mpu6050intpin, CN15_ENABLE);

DigitalIO digitalports;

// Uart Manager, used for the debug console
UartManager debugserial(UART1, GetPeripheralClock(), UART_BAUD_RATE);
DebugConsole dbgout(debugserial);

// led present on the Pinguino micro
OutputPin led(IOPORT_D, BIT_1);

int main(void) {

    //float testf = 5/3;

    // Led on the Pingulux micro
    led << false;

    digitalports.addPinChangeHandler(&mpu6050datardy);
    //digitalports.enableChangeNotification(true);

    // configure for multi-vectored mode
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    // enable interrupts
    INTEnableInterrupts();

    dbgout << "**** Pic32Copter: Hello World ****\r\n";
    dbgout << "**** This is the second line ****\r\n";
    dbgout << "**** This is the third line ****\r\n";

    bool error = motionsensor.Init();

    dbgout << "**** This is the fourth line ****\r\n";
    if (error) {
        dbgout << "i2c buserror";
    }

    System::dbgcounter++;
    // testf++;

    dbgout << "dbgcounter=" << System::dbgcounter; // << testf;

    while(1)
    {
        int c=16*1024*10;
        while(c--);
        led.toggle();
        dbgout << "dbgcounter=" << System::dbgcounter;
    }

    return 0;
}


//OLD man.c
//void main(void) {
//    // Configure the device for maximum performance but do not change the PBDIV
//    // Given the options, this function will change the flash wait states, RAM
//    // wait state and enable prefetch cache but will not change the PBDIV.
//    // The PBDIV value is already set via the pragma FPBDIV option above..
//    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
//
//    // switch to Multivectored interrupts
//    INTEnableSystemMultiVectoredInt();
//
//
//    // Generate PWM at
//    /* Open Timer2 with Period register value */
//    OpenTimer2(T2_ON, 0x550);
//
//    /* Enable OC | 32 bit Mode  | Timer2 is selected | Continuous O/P   | OC Pin High , S Compare value, Compare value*/
//    OpenOC1( OC_ON | OC_TIMER_MODE32 | OC_TIMER2_SRC | OC_CONTINUE_PULSE | OC_LOW_HIGH , 0x550, 0x500 );
//
//
//}
