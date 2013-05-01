/*
 * File:   newmain.c
 * Author: Georg
 *
 * Created on 14 ottobre 2012, 18.54
 */


#include "kernel/system.h"

#include "driver/i2cmanager.h"
#include "driver/mpu-6050.h"
#include "driver/uartmanager.h"
#include "driver/digitaliomanager.h"
#include "driver/dbgout.h"

#define I2C_CLOCK_FREQ      (100000) // tested to work up to 400KHz
#define UART_BAUD_RATE      (115200)

// I2C driver
I2c i2c_mod_1(I2C1, GetPeripheralClock(), I2C_CLOCK_FREQ );
// Invensense sensor, with the interrupt line connected to CN15/RD6
MPU_6050 motionsensor(i2c_mod_1);
InputPin mpu6050intpin(IOPORT_D, BIT_6);
PinChangeHandler mpu6050datardy(mpu6050intpin, CN15_ENABLE);

InputPin btinputpin(IOPORT_D, BIT_4);
PinChangeHandler btnchange(btinputpin, CN13_ENABLE);

DigitalIO digitalports;

// Uart Manager, used for the debug console
UartManager debugserial(UART1, GetPeripheralClock(), UART_BAUD_RATE);
DebugConsole dbgout(debugserial);

// led present on the Pinguino micro
OutputPin led(IOPORT_D, BIT_1);

extern void testexec();

int main(void) {

    //float testf = 5/3;

    // Led on the Pingulux micro
    led << false;

    digitalports.addPinChangeHandler(&mpu6050datardy);
    digitalports.addPinChangeHandler(&btnchange);
    digitalports.enableChangeNotification(true);

    // configure for multi-vectored mode
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    // enable interrupts
    INTEnableInterrupts();

    dbgout << "**** Pic32Copter: Hello World ****\r\n";
    dbgout << "**** Going to init the sensors****\r\n";
    bool error = motionsensor.Init();

    dbgout << "MPU6050 initialized\r\n";
    if (error) { dbgout << "i2c buserror during MPU6050 init \r\n"; }

    dbgout << "Going to enable the DMP\r\n";
    error = motionsensor.enableDMP();
    if (error) { dbgout << "i2c buserror during MPU6050 init \r\n"; }

    System::dbgcounter++;
    // testf++;

    dbgout << "dbgcounter=" << System::dbgcounter; // << testf;

    //testexec();

    unsigned int ssstatus = _CP0_GET_SRSCTL();
    unsigned int procid = _CP0_GET_PRID();
    unsigned int status = _CP0_GET_STATUS();

    dbgout << "SS Status= " << ssstatus << "\r\n";
    dbgout << "ProcID" << procid << "\r\n";
    dbgout << "CPU Status= " << status << "\r\n";

    while(1)
    {
        int c=16*1024*10;
        while(c--);
        led.toggle();
        dbgout << "dbgcounter=" << System::dbgcounter << " ";
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
