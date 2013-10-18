/*  interrupt prologue and epilogue
*
*   This handles the storing of task / nested ints when a new interrupt is served.
*   You must indicate the desired int level IDL in the form that is already shifted
*
*   level-0 = 0,        level-1 = 0x0400
*   level-2 = 0x0800,   level-3 = 0x0c00
*   level-4 = 0x1000,   level-5 = 0x1400
*   level-6 = 0x1800,   level-7 = 0x1c00 (shadow GPR)
*
*/


/* macro IntHandler */
.macro IntHandler intlevel intservice
   /* if the previous context was an interrupt then we simply execute the interrupt (reg safe)
      otherwise we save the context switch the sp to the interrupt reserved stack  */

    mfc0 $k0, $12          /* read STATUS register */
    lui  $k1, 7
    srl  $k1, $k1, 6       /* prepare mask to extract IDL from status */
    and  $k1, $k0, $k1
    bne  $k1, $zero, _hNested\intservice
    nop
    /* we come IDL = 0 which means "normal user level" we save the reg-file and switch */
    #rdpgpr $sp, $sp  /* use previous sp.  We have still STATUS in k0 */

    addiu $sp, $sp, -128  /* we create some space on the stack */

    /* TODO: rearrange the order to minimize disabled ints */
    sw $s7, 96($sp)     /* will host the current task stack pointer (used for level 0 only) */
    addiu $s7, $sp, 0   /* s7 contains the ORIGSP */

    sw $k0, 116($s7) /* original status after entering the INT (EXL = 1 and IDL=ontouched) */
    mfc0 $k1, $14    /* EPC */
    sw $k1, 120($s7) /* this is the future EPC */

   /* let's set the new Interrupt level */
    ins	$k0, $zero, 1, 15   /* this enables nested interrupts since EXL (and ERL are set t0 0) */
    ori	$k0, $k0, \intlevel /* 0x400 -> level = 1 0x1800 --> level = 6 */

    lw $sp,%gp_rel(interruptstack)($gp) /* MUST be before the reactivation of INTS */

    mtc0 $k0, $12 /* from here on nested INTS are enabled */

    sw $at, ($s7)
    sw $v0, 4($s7)
    sw $v1, 8($s7)
    sw $a0, 12($s7)
    sw $a1, 16($s7)
    sw $a2, 20($s7)
    sw $a3, 24($s7)
    sw $t0, 28($s7)
    sw $t1, 32($s7)
    sw $t2, 36($s7)
    sw $t3, 40($s7)
    sw $t4, 44($s7)
    sw $t5, 48($s7)
    sw $t6, 52($s7)
    sw $t7, 56($s7)
    sw $t8, 60($s7)
    sw $t9, 64($s7)
    sw $s0, 68($s7)
    sw $s1, 72($s7)
    sw $s2, 76($s7)
    sw $s3, 80($s7)
    sw $s4, 84($s7)
    sw $s5, 88($s7)
    sw $s6, 92($s7)
    sw $fp, 100($s7) /* fp=s8 this might be used by the interrupted task */
    sw $ra, 104($s7)
    mfhi $v0
    sw $v0, 108($s7)
    mflo $v0
    sw $v0, 112($s7)

    /*-BEGIN------- actual INT code to handle -------*/
    addiu $sp, $sp, -16 #
    jal \intservice # call into the c code
    nop
 /* addiu $sp, $sp, 16  # not needed because we cretate that space again below */
    /*-END------- actual INT code to handle -------*/

    di $zero    /* disable int--> disable nested interrupts */
    ehb

    /* now we try to get the  stackpointer of the next task to execute */
    addiu $a0, $s7, 0   /* s7 still contains the original stack pointer of the interrupted task */
/*  addiu $sp, $sp, -16 # balanced with addiu above (area for the called func to store a0-a3) */
    jal RescheduleIfNeeded
    nop
/*  addiu $sp, $sp, 16 #  avoided because we replace sp in the next instruction */

    /* we have in $v0 the stack pointer of the next task */
    addiu $sp, $v0, 0

    /* start to restore */
    lw $v0, 120($sp)
    mtc0 $v0, $14       /* new EPC */
    lw $v0, 116($sp)    /* new STATUS */
    ori $v0, $v0, 2     /* must ensure that we stay with EXL enabled exception condition */
    mtc0 $v0, $12       /* set CP0 Status */
    lw $v0, 112($sp)
    mtlo $v0
    lw $v0, 108($sp)
    mthi $v0
    lw $ra, 104($sp)
    lw $fp, 100($sp)    /*  fp=s8 */
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

    addiu $sp, $sp, 128  /* we restore the original sp */
    #wrpgpr $sp, $sp

    eret  /* back to the caller or orginal program counter */
    nop


_hNested\intservice:

    mfc0 $k1, $14        /* EPC */
    addiu $sp, $sp, -112  /* needed space to store the registers + 16 for the int call */
    sw $k1, 108($sp)      /* orig epc */
    sw $k0, 104($sp)      /* orig status */
   /* let's set the new Interrupt level */
    ins	$k0, $zero, 1, 15 /* this enables nested interrupts since EXL (and ERL are set t0 0) */
    ori	$k0, $k0, \intlevel   /* 0x400 -> level = 1 0x1800 --> level = 6 */
    mtc0 $k0, $12  /* set status, from here on nested INTS are enabled */

    sw $ra, 92($sp)
    sw $s8, 88($sp)
    sw $t9, 84($sp)
    sw $t8, 80($sp)
    sw $t7, 76($sp)
    sw $t6, 72($sp)
    sw $t5, 68($sp)
    sw $t4, 64($sp)
    sw $t3, 60($sp)
    sw $t2, 56($sp)
    sw $t1, 52($sp)
    sw $t0, 48($sp)
    sw $a3, 44($sp)
    sw $a2, 40($sp)
    sw $a1, 36($sp)
    sw $a0, 32($sp)
    sw $v1, 28($sp)
    sw $v0, 24($sp)
    sw $at, 20($sp)
    mflo $v0
    sw $v0, 100($sp)
    mfhi $v0
    sw $v0, 96($sp)

    /*-BEGIN------- actual INT code to handle -------*/
 /* addiu $sp, $sp, -16  # included in the addiu above */
    jal \intservice /* call into the c code */
    nop
/*  addiu $sp, $sp, 16   # included in the addiu below */
    /*-END------- actual INT code to handle -------*/

    lw $v0,100($sp)
    mtlo $v0
    lw $v1,96($sp)
    mthi $v0

    lw $ra, 92($sp)
    lw $s8, 88($sp)
    lw $t9, 84($sp)
    lw $t8, 80($sp)
    lw $t7, 76($sp)
    lw $t6, 72($sp)
    lw $t5, 68($sp)
    lw $t4, 64($sp)
    lw $t3, 60($sp)
    lw $t2, 56($sp)
    lw $t1, 52($sp)
    lw $t0, 48($sp)
    lw $a3, 44($sp)
    lw $a2, 40($sp)
    lw $a1, 36($sp)
    lw $a0, 32($sp)
    lw $v1, 28($sp)
    lw $v0, 24($sp)
    lw $at, 20($sp)
    di $zero
    ehb
    lw $k0,108($sp)
    lw $k1,104($sp)
    mtc0 $k0, $14       /* EPC restored */
    addiu $sp, $sp, 112 /* includes 16 for the int call */
    mtc0 $k1, $12       /* status restored */

    eret  /* back to the caller or orginal program counter */
    nop
.endm

