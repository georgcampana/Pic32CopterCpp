# Georg Campana  Hardware Abstraction Layer
#

.globl swapTaskContext, forkTask, transferMainStack, getInterruptLevel, atomicInc, atomicDec


.text



# must be called with Interrupts already switched off & never from an exception
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
    *  124, 120, 116,  112,108,104,100,96,92,88,84,80,76,72,68,64,60,56,52,48,44,40,36,32,28,24,20,16,12, 8, 4, 0
    *  fp,  epc,status,lo ,hi, ra, s8, s7,s6,s5,s4,s3,s2,s1,s0,t9,t8,t7,t6,t5,t4,t3,t2,t1,t0,a3,a2,a1,a0,v1,v0,at
    */

    #syscall /* causes a soft exception that ends  */
    # we simulate a systemcall here
    mfc0 $k0, $12  # CP0 STATUS register
    ori $k0, $k0, 2 # Set EXL bit = 1 (we simulate an exception)
    mtc0 $k0, $12

    mtc0 $ra, $14  # we set the ra into E-PC so that it used to return with ERET

    /* from here we pretend to be system code: EXL is 1 and ints are disabled*/

    addiu $sp, $sp, -128  # we create some space on the stack

    sw $sp, ($a0) # we store the current sp to the current task sp pointer
    sw $at, ($sp)
    sw $v0, 4($sp)
    sw $v1, 8($sp)
    sw $a0, 12($sp) # our first arg currtask->savedsppointer
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
    sw $s8, 100($sp)
    sw $ra, 104($sp)
    mfhi $v0
    sw $v0, 108($sp)
    mflo $v0
    sw $v0, 112($sp)
    mfc0 $v0, $12 #Status
    sw $v0, 116($sp)
    sw $ra, 120($sp)  # this is the future EPC
    # ---- OLD -----

    # ---- NEW ------
    # From here we start to take the gpr we still have the new task sp in a1
    addiu $sp, $a1, 0

    lw $v0, 120($sp)
    mtc0 $v0, $14 # new EPC
    lw $v0, 116($sp) # new STATUS
    ori $v0, $v0, 2  # must ensure that we stay with EXL enabled exception condition
    mtc0 $v0, $12 # set CP0 Status
    lw $v0, 112($sp)
    mtlo $v0
    lw $v0, 108($sp)
    mthi $v0
    lw $ra, 104($sp)
    lw $s8, 100($sp)
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

    addiu $sp, $sp, 128  # we restore the original sp

    eret  # back to the caller or orginal program counter
    nop
.end swapTaskContext

/*
* we get called from the creator task (forking)
* we will prepare the new task stack
* this must contain everyting needed to jump back (return) to the fork caller
* but returning 1 in $v0
* in addition we add the saved gpr so that the reschedule function restore it
* arg0=$a0 = void** points to the taskpointer,
* arg1=$a1 = void* the new taskpointer,
* arg2=$a2 = char** stackpointer
* arg3=$a3 = stacksize
*/
.ent forkTask
forkTask:
    .set noreorder
    .set nomips16
    .set noat

# arg0 taskpointer
# arg1 pointer to the new stackpointer
# arg2 stacksize
# arg3 pointer to execfunction

    lw $t0, ($a1)   # $t0 is our newtask stack pointer
    add $t0, $t0, $a2  #in mips the stack grows downwards so we start from the top

    addiu $t0, $t0, -16 # arg0-arg3 list

    #time to init the stack for the reschedule
    # 124, 120, 116,  112,108,104,100,96,92,88,84,80,76,72,68,64,60,56,52,48,44,40,36,32,28,24,20,16,12, 8, 4, 0
    # fp,  epc, status,lo ,hi, ra, s8,s7,s6,s5,s4,s3,s2,s1,s0,t9,t8,t7,t6,t5,t4,t3,t2,t1,t0,a3,a2,a1,a0,v1,v0,at
    addiu $t0, $t0, -128  # we create some space on the stack 
    sw $0, ($t0)        # at
    addiu $v0, $a0, 0   # v0 = return value = pointer to forkedtask
    sw $v0, 4($t0)      # v0
    sw $0,  8($t0)      # v1
    sw $a0,12($t0)      # a0
    sw $a1,16($t0)      # a1
    sw $a2,20($t0)      # a2
    sw $a3,24($t0)      # a3
    sw $0, 28($t0)      # t0
    sw $0, 32($t0)      # t1
    sw $t2,36($t0)      # t2
    sw $t3,40($t0)      # t3
    sw $t4,44($t0)      # t4
    sw $t5,48($t0)      # t5
    sw $t6,52($t0)      # t6
    sw $t7,56($t0)      # t7
    sw $t8,60($t0)      # t8
    sw $t9,64($t0)      # t9
    sw $s0,68($t0)      # s0
    sw $s1,72($t0)      # s1
    sw $s2,76($t0)      # s2
    sw $s3,80($t0)      # s3
    sw $s4,84($t0)      # s4
    sw $s5,88($t0)      # s5
    sw $s6,92($t0)      # s6
    sw $s7,96($t0)      # s7
    sw $s8,100($t0)     # s8
    sw $ra,104($t0)     # return address (should never be necesary since we just started a new task)
    sw $0, 108($t0)     # hi
    sw $0, 112($t0)     # lo
    mfc0 $t1, $12 #Status
    sw $t1, 116($t0)         # CP0 status
    sw $a3, 120($t0)         #
    sw $fp, 124($t0)         # this might be used by the interrupted task

    sw $t0, ($a1)       # we store the stackpointer in the var location

    add $v0, $0, $0 # result is false for the caller (original task)
    jr $ra #return from subroutine
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

    lw $t0, ($a0)   # $t0 is our newtask stack pointer
    add $t0, $t0, $a1  #in mips the stack grows downwards so we start from the top

    addiu $t0, $t0, -32 # space for the arg0-arg3 list and to copy 4 more words
    lw $t1, 16($sp)
    sw $t1, 16($t0)
    lw $t1, 20($sp)
    sw $t1, 20($t0)
    lw $t1, 24($sp)
    sw $t1, 24($t0)
    lw $t1, 28($sp)
    sw $t1, 28($t0)     # we copy the local part of the callers stack

    sw $t1, ($a0)       # we store the stackpointer in the var location

    add $sp, $t0, $0    # we move the sp to the new prepared location

    jr $ra #return from subroutine
    nop
.end transferMainStack


#macro IntPrologue
.macro IntPrologue intlevel=0x400
   # if the previous context was an interrupt then we simply execute the interrupt (reg safe)
   # otherwise we save the context switch the sp to the interrupt reserved stack

    mfc0 $k0, $12          /* read STATUS register */
    lui  $k1, 7
    srl  $k1, $k1, 6       /* prepare mask to extract IDL from status */
    and  $k1, $k0, $k1
    bne  $k1, $0, _hCTnestedInt
    nop
    /* we come IDL = 0 which means "normal user level" we save the reg-file and switch */
    #rdpgpr $sp, $sp /* use previous sp.  We have still STATUS in k0 */

    addiu $sp, $sp, -128  # we create some space on the stack
    sw $sp, %gp_rel(ORIGSP)($gp) # we persist the sp of the currently loosing-cpu task

    /* TODO: rearrange the order to minimize disabled ints */
    sw $at, ($sp)
    sw $v0, 4($sp)
    sw $v1, 8($sp)
    sw $a0, 12($sp)
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
    sw $s8, 100($sp)
    sw $ra, 104($sp)
    mfhi $v0
    sw $v0, 108($sp)
    mflo $v0
    sw $v0, 112($sp)
    #mfc0 $v0, $12 #Status
    #sw $v0, 116($sp)
    sw $k0, 116($sp) # original status after entering the INT (EXL = 1 and IDL=ontouched)
    mfc0 $v0, $14 # EPC
    sw $v0, 120($sp)  # this is the future EPC
    sw $fp, 124($sp)  # this might be used by the interrupted task

   /* let's set the new Interrupt level */
    ins	$k0, $zero, 1, 15 # this enables nested interrupts since EXL (and ERL are set t0 0)
    ori	$k0, $k0, \intlevel   # 0x400 -> level = 1 0x1800 --> level = 6

    mtc0 $k0, $12 # from here on nested INTS are enabled

    lw $sp,%gp_rel(interruptstack)($gp)
.endm

#macro IntEpilogue
.macro IntEpilogue
    di $zero # disable int--> disable nested interrupts
    ehb

    /* now we try to get the  stackpointer of the next task to execute */
    lw $a0, %gp_rel(ORIGSP)($gp)
    addiu $sp, $sp, -16 # (area for the called func to store a0-a3)
    jal RescheduleIfNeeded
    nop
    addiu $sp, $sp, 16 #  could be avoided because we replace sp in the next instruction

    /* we have in $v0 the stack pointer of the next task */
    addiu $sp, $v0, 0

    /* start to restore */
    lw $fp, 124($sp)
    lw $v0, 120($sp)
    mtc0 $v0, $14 # new EPC
    lw $v0, 116($sp) # new STATUS
    ori $v0, $v0, 2  # must ensure that we stay with EXL enabled exception condition
    mtc0 $v0, $12 # set CP0 Status
    lw $v0, 112($sp)
    mtlo $v0
    lw $v0, 108($sp)
    mthi $v0
    lw $ra, 104($sp)
    lw $s8, 100($sp)
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

    addiu $sp, $sp, 128  # we restore the original sp
    #wrpgpr $sp, $sp

    eret  # back to the caller or orginal program counter
    nop

.endm



/* PIC32 CoreTimer interrupt (vector0)
* This is the prologue + epilogue for the pre-empting interrupt routine which is
* present in the Hal module
* We have a pointer to the stack reserved for intrrupts
* (see "interruptstack" in the sbss section).
*/

.section .vector_0,code
   j      handleCoreTimer
   nop

.section .text,code
.ent handleCoreTimer
handleCoreTimer:
    .set noreorder
    .set nomips16
    .set noat
    
    /* 0x400 = level 1 */
    IntPrologue intlevel=0x400

    /*-BEGIN------- actual INT code to handle -------*/
    addiu $sp, $sp, -16 # probably not needed (area for the called func to store a0-a3)
    jal handleSysTimerINT # call into t he c code
    nop
    addiu $sp, $sp, 16  # see above
    /*-END------- actual INT code to handle -------*/

    IntEpilogue

 _hCTnestedInt:
    sdbbp 0

# TODO: nested timeslice (should be simpler)


.end handleCoreTimer

.ent getInterruptLevel
getInterruptLevel:
    .set noreorder
    .set nomips16
    .set noat

    mfc0 $t0, $12          /* read STATUS register */
    lui  $v0, 7
    srl  $v0, $v0, 6       /* prepare mask to extract IDL from status */
    and  $v0, $t0, $v0     /* v0 contains the IPL now */
    srl  $v0, 10           /* bits 0 aligned */

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
    .set nomips16
    .set noat

    ll $v0, 0($a0)
    addiu    $v0, $v0, -1
    sc $v0, 0($a0)
    beq $v0, $zero, atomicDec
    nop

    jr $ra
    nop
.end atomicDec


.section	.sbss,bss
.globl	interruptstack
	.align	2
	.type	interruptstack, @object
	.size	interruptstack, 4
interruptstack:
	.space	4

/* this is to hold the Original StackPointer when entering the SYS CALL */
ORIGSP:
	.space	4
