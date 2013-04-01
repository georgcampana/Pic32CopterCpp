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



// PIC32MX440F256H Configuration Bit Settings
// DEVCFG3
// USERID = No Setting

// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_15         // PLL Multiplier (15x Multiplier)
#pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider (2x Divider)
#pragma config UPLLEN = ON              // USB PLL Enable (Enabled)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1              // Watchdog Timer Postscaler (1:1)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)



#define I2C_CLOCK_FREQ      (20000) // tested to work up to 400KHz
#define UART_BAUD_RATE      (9600)

I2c i2c_mod_1(I2C1, GetPeripheralClock(), I2C_CLOCK_FREQ );
MPU_6050 motionsensor(i2c_mod_1);
UartManager debugserial(UART1, UART_BAUD_RATE);


int main(void) {


    // Led on the Pingulux micro
    mPORTDSetPinsDigitalOut(BIT_1);
    mPORTDClearBits(BIT_1);

    // configure for multi-vectored mode
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);

    // enable interrupts
    INTEnableInterrupts();

    debugserial.write("**** Pic32Copter: Hello World ****\n");
    debugserial.write("**** This is the second line ****\n");

    bool error = motionsensor.Init();
    
    if (error) {
        DBPRINTF("Buserror");
    }


    DBPRINTF("Finished");

    System::dbgcounter++;

    while(1)
    {
        int c=256*1024*10;
        while(c--);
        mPORTDToggleBits(BIT_1);
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
