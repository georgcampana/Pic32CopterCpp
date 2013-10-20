Pic32CopterCpp
==============

A C++ microOS to build a flight-controller with the PIC32
---------------------------------------------------------

Background:

I started to fall in love with the Quadcopter world and technology and saw that most of the controllers use an ATMEL microcontroller.
My first attempt and idea was to use the IOIO board and control it via Android but i gave up because of the high latency and the non Real time behavior.
I  found then the PIC32 based Pinguino from Olimex: https://www.olimex.com/Products/Duino/PIC32/
They are very low cost but have an impressive cpu-performance.

Then, by looking on how the software is written for various platforms, i saw that most of the sensor drivers are not using interrupts at all, they simply spin and poll the status.
I tought that in a lot of cases this is really wasting CPU cycles. 
For that reason i started to write my first drivers (UART and I2C) using interrupts. 
When you start to use interrupts you need to have a mechanism to wait for an event without wasting cycles i.e. by scheduling another thread or task.

That's why i started to write this kind of C++ based OS for the PIC32 MIPS cpu.

At the end i will have not only a flight controller firmware but a complete (RT)OS that can be reused for other 1000 applications.


Goals

- No periphals polling. Everything is based on interrupts to not waste cycles.
- Preemptive multitasking. With prioritised tasks running when ready or rescheduled after a time slice.
- Completely written in C++ with few exception for the low level part (atomics, interrupt prologue and epilogie)
- Can be developed using the standard Microchip MPLAB-X tool. I got a free test licence for the X32 c++ compiler.
- No heap allocation. Everything is built to be preallocated at compile time (a "must" to ensure assessed timings)
- low context switch latency. Not sure if i achieved this: if the cycles consumed to switch are more than those used to poll then this point is nto achieved.
