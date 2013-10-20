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
 * Georg Campana  Hardware Abstraction Layer
 * functions needed for the hal to support the scheduler
 *
 */

.globl swapTaskContext, forkTask, transferMainStack, getInterruptLevel, atomicInc, atomicDec, lockTestAndSet
.text



/* must be called with Interrupts already switched off & never from an exception */
.ent swapTaskContext
swapTaskContext:
    .set noreorder
    .set nomips16
    .set noat

   /* no need to get the right sp since this swapContext can be called from a Task only
    * we start to call the SYS CALL isntruction to force a software exceptio and capture the EPC
    * so that we theh save the gpr, the allocated space/sequence must be the same as in case of an interrupt
    * because an int could reschedule the current task
    * steps: create stack space; save status, save gpr, save orig sp;
    *        take new task's sp; restore gpr; restore status; remove sp space
    *  124,   120, 116,  112,108,104,100,96,92,88,84,80,76,72,68,64,60,56,52,48,44,40,36,32,28,24,20,16,12, 8, 4, 0
    *  <free>,epc,status,lo ,hi, ra, fp, s7,s6,s5,s4,s3,s2,s1,s0,t9,t8,t7,t6,t5,t4,t3,t2,t1,t0,a3,a2,a1,a0,v1,v0,at
    */

    // we simulate a systemcall here
    mfc0 $k0, $12   // CP0 STATUS register
    ori $k0, $k0, 2 // Set EXL bit = 1 (we simulate an exception)
    mtc0 $k0, $12

    mtc0 $ra, $14   // we set the ra into E-PC so that it used to return with ERET

    // from here we pretend to be system code: EXL is 1 and ints are disabled

    addiu $sp, $sp, -128  // we create some space on the stack

    sw $sp, ($a0)   // we store the current sp to the current task sp pointer
    sw $at, ($sp)
    sw $v0, 4($sp)
    sw $v1, 8($sp)
    sw $a0, 12($sp) // our first arg currtask->savedsppointer
    sw $a1, 16($sp)
    sw $a2, 20($sp)
    sw $a3, 24($sp)
    sw $t0, 28($sp)
    sw $t1, 32($sp)
    sw $t2, 36($sp)
    sw $t3, 40($sp)
    sw $t4, 44($sp)
    sw $t5, 48($sp)
    sw $t6, 52($sp)
    sw $t7, 56($sp)
    sw $t8, 60($sp)
    sw $t9, 64($sp)
    sw $s0, 68($sp)
    sw $s1, 72($sp)
    sw $s2, 76($sp)
    sw $s3, 80($sp)
    sw $s4, 84($sp)
    sw $s5, 88($sp)
    sw $s6, 92($sp)
    sw $s7, 96($sp)
    sw $fp, 100($sp)
    sw $ra, 104($sp)
    mfhi $v0
    sw $v0, 108($sp)
    mflo $v0
    sw $v0, 112($sp)
    mfc0 $v0, $12 // Status
    sw $v0, 116($sp)
    sw $ra, 120($sp)  // this is the future EPC
    // ---- OLD -----

    // ---- NEW ------
    // From here we start to take the gpr we still have the new task sp in a1
    addiu $sp, $a1, 0

    lw $v0, 120($sp)
    mtc0 $v0, $14    // new EPC
    lw $v0, 116($sp) // new STATUS
    ori $v0, $v0, 2  // must ensure that we stay with EXL enabled exception condition
    mtc0 $v0, $12    // set CP0 Status
    lw $v0, 112($sp)
    mtlo $v0
    lw $v0, 108($sp)
    mthi $v0
    lw $ra, 104($sp)
    lw $fp, 100($sp)
    lw $s7, 96($sp)
    lw $s6, 92($sp)
    lw $s5, 88($sp)
    lw $s4, 84($sp)
    lw $s3, 80($sp)
    lw $s2, 76($sp)
    lw $s1, 72($sp)
    lw $s0, 68($sp)
    lw $t9, 64($sp)
    lw $t8, 60($sp)
    lw $t7, 56($sp)
    lw $t6, 52($sp)
    lw $t5, 48($sp)
    lw $t4, 44($sp)
    lw $t3, 40($sp)
    lw $t2, 36($sp)
    lw $t1, 32($sp)
    lw $t0, 28($sp)
    lw $a3, 24($sp)
    lw $a2, 20($sp)
    lw $a1, 16($sp)
    lw $a0, 12($sp)
    lw $v1,  8($sp)
    lw $v0,  4($sp)
    lw $at,   ($sp)

    addiu $sp, $sp, 128  // we restore the original sp

    eret  // back to the caller or orginal program counter
    nop
.end swapTaskContext

/*
* forkTask
* we get called from the creator task (forking)
* we will prepare the new task stack
*
* in addition we add the saved gpr so that the reschedule function restore it
* arg0 newtaskpointer
* arg1 pointer to the new stackpointer
* arg2 stacksize
* arg3 pointer to execfunction to start with
*
* returns 0 to the original caller or the pointer to the new task in the new task
*
*/
.ent forkTask
forkTask:
    .set noreorder
    .set nomips16
    .set noat


    lw $t0, ($a1)     // $t0 is our newtask stack pointer
    add $t0, $t0, $a2 // in mips the stack grows downwards so we start from the top

    addiu $t0, $t0, -16 // arg0-arg3 list

    /* time to init the stack for the reschedule
    * 124,   120, 116,  112,108,104,100,96,92,88,84,80,76,72,68,64,60,56,52,48,44,40,36,32,28,24,20,16,12, 8, 4, 0
    * <free>,epc, status,lo ,hi, ra, s8,s7,s6,s5,s4,s3,s2,s1,s0,t9,t8,t7,t6,t5,t4,t3,t2,t1,t0,a3,a2,a1,a0,v1,v0,at
    */
    addiu $t0, $t0, -128  // we create some space on the stack
    sw $0, ($t0)        // at
    addiu $v0, $a0, 0   // v0 = return value = pointer to forkedtask
    sw $v0, 4($t0)      // v0
    sw $0,  8($t0)      // v1
    sw $a0,12($t0)      // a0
    sw $a1,16($t0)      // a1
    sw $a2,20($t0)      // a2
    sw $a3,24($t0)      // a3
    sw $0, 28($t0)      // t0
    sw $0, 32($t0)      // t1
    sw $t2,36($t0)      // t2
    sw $t3,40($t0)      // t3
    sw $t4,44($t0)      // t4
    sw $t5,48($t0)      // t5
    sw $t6,52($t0)      // t6
    sw $t7,56($t0)      // t7
    sw $t8,60($t0)      // t8
    sw $t9,64($t0)      // t9
    sw $s0,68($t0)      // s0
    sw $s1,72($t0)      // s1
    sw $s2,76($t0)      // s2
    sw $s3,80($t0)      // s3
    sw $s4,84($t0)      // s4
    sw $s5,88($t0)      // s5
    sw $s6,92($t0)      // s6
    sw $s7,96($t0)      // s7
    sw $fp,100($t0)     // fp=s8 this might be used by the interrupted task
    sw $ra,104($t0)     // return address (should never be necesary since we just started a new task)
    sw $0, 108($t0)     // hi
    sw $0, 112($t0)     // lo
    mfc0 $t1, $12       // Status
    sw $t1, 116($t0)    // CP0 status
    sw $a3, 120($t0)    

    sw $t0, ($a1)       // we store the stackpointer in the var location

    add $v0, $0, $0     // result is false for the caller (original task)
    jr $ra              // return from subroutine
    nop
.end forkTask

/*
* This moves the stack of the main thread into the Task object stack space
* arg0=$a0 = char** stackpointer
* arg1=$a1 = stacksize
*
* Note: can be called from user-code only (not from an interrupt)
*/
.ent transferMainStack
transferMainStack:
    .set noreorder
    .set nomips16
    .set noat

    lw $t0, ($a0)      // $t0 is our newtask stack pointer
    add $t0, $t0, $a1  // in mips the stack grows downwards so we start from the top

    addiu $t0, $t0, -32 // space for the arg0-arg3 list and to copy 4 more words
    lw $t1, 16($sp)
    sw $t1, 16($t0)
    lw $t1, 20($sp)
    sw $t1, 20($t0)
    lw $t1, 24($sp)
    sw $t1, 24($t0)
    lw $t1, 28($sp)
    sw $t1, 28($t0)     // we copy the local part of the callers stack

    sw $t1, ($a0)       // we store the stackpointer in the var location

    add $sp, $t0, $0    // we move the sp to the new prepared location

    jr $ra              // return from subroutine
    nop
.end transferMainStack

.ent getInterruptLevel
getInterruptLevel:
    .set noreorder
    .set nomips16
    .set noat

    mfc0 $t0, $12          // read STATUS register
    lui  $v0, 7
    srl  $v0, $v0, 6       // prepare mask to extract IDL from status
    and  $v0, $t0, $v0     // v0 contains the IPL now
    srl  $v0, 10           // bits 0 aligned

    jr $ra
    nop

.end getInterruptLevel


.ent atomicInc
atomicInc:
    .set noreorder
    .set nomips16
    .set noat

    ll $v0, 0($a0)
    addiu    $v0, $v0, 1
    sc $v0, 0($a0)
    beq $v0, $zero, atomicInc
    nop

    jr $ra
    nop
.end atomicInc

.ent atomicDec
atomicDec:
    .set noreorder

    ll $v0, 0($a0)
    addiu    $v0, $v0, -1
    sc $v0, 0($a0)
    beq $v0, $zero, atomicDec
    nop

    jr $ra
    nop
.end atomicDec

/*
 *   returns old value if set already or zero in case of successful set
 *   $a0 = pointer to uint32 var  , $a1 = uint32 value to set
*/
.ent lockTestAndSet
lockTestAndSet:
    .set noreorder

    addiu $t0, $a1, 0           /* t0 = val to set (to preserve a1)   */
    ll $v0, 0($a0)              /* read current value and set monitor */
    bne $v0, $zero, _alreadyset /* if is not zero then we exit and return the current value */
    nop
    sc $t0, 0($a0)              /* if *a0 is unchanged then we set it to t0. Otherwise t0 is set to 0 */
    beq $t0, $zero, lockTestAndSet /* retry in case of changed value between test and set */
    nop
_alreadyset:
    jr $ra
    nop
.end lockTestAndSet