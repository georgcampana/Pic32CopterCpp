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
    bne  $k1, $zero, _hNestedP\intservice
    nop
    /* we come IDL = 0 which means "normal user level" we save the reg-file and switch */
    #rdpgpr $sp, $sp  /* use previous sp.  We have still STATUS in k0 */

    addiu $sp, $sp, -128  /* we create some space on the stack */

    /* TODO: rearrange the order to minimize disabled ints */
    sw $s6, 92($sp)     /* will host the current task stack pointer (used for level 0 only) */
    sw $s7, 96($sp)     /* will host the int level of the loosing task (must be 0) */
    addiu $s6, $sp, 0   /* s6 contains the ORIGSP */
    addiu $s7, $zero, 0 /* s7 contains the current level (0 since we are here) */

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
    sw $s8, 100($sp)
    sw $ra, 104($sp)
    mfhi $v0
    sw $v0, 108($sp)
    mflo $v0
    sw $v0, 112($sp)
    #mfc0 $v0, $12 /* Status */
    #sw $v0, 116($sp)
    sw $k0, 116($sp) /* original status after entering the INT (EXL = 1 and IDL=ontouched) */
    mfc0 $v0, $14    /* EPC */
    sw $v0, 120($sp) /* this is the future EPC */
    sw $fp, 124($sp) /* this might be used by the interrupted task */

   /* let's set the new Interrupt level */
    ins	$k0, $zero, 1, 15   /* this enables nested interrupts since EXL (and ERL are set t0 0) */
    ori	$k0, $k0, \intlevel /* 0x400 -> level = 1 0x1800 --> level = 6 */

    lw $sp,%gp_rel(interruptstack)($gp) /* MUST be before the reactivation of INTS */

    mtc0 $k0, $12 /* from here on nested INTS are enabled */
    j _hPEnd\intservice
    nop

_hNestedP\intservice:

    addiu $sp, $sp, -96  /* needed space to store the registers */
    sw $s7,   ($sp)      /* we store the s7 to use it for the leaving int level */
    addiu $s7, $k1, 0    /* the original level set before jumping here (shifted though) */
    mfc0 $k1, $14        /* EPC */
    sw $k1, 92($sp)      /* orig epc */
    sw $k0, 88($sp)      /* orig status */
   /* let's set the new Interrupt level */
    ins	$k0, $zero, 1, 15 /* this enables nested interrupts since EXL (and ERL are set t0 0) */
    ori	$k0, $k0, \intlevel   /* 0x400 -> level = 1 0x1800 --> level = 6 */
    mtc0 $k0, $12  /* set status, from here on nested INTS are enabled */

    sw $ra, 76($sp)
    sw $s8, 72($sp)
    sw $t9, 68($sp)
    sw $t8, 64($sp)
    sw $t7, 60($sp)
    sw $t6, 56($sp)
    sw $t5, 52($sp)
    sw $t4, 48($sp)
    sw $t3, 44($sp)
    sw $t2, 40($sp)
    sw $t1, 36($sp)
    sw $t0, 32($sp)
    sw $a3, 28($sp)
    sw $a2, 24($sp)
    sw $a1, 20($sp)
    sw $a0, 16($sp)
    sw $v1, 12($sp)
    sw $v0,  8($sp)
    sw $at,  4($sp)
    mflo $v0
    sw $v0, 84($sp)
    mfhi $v0
    sw $v0, 80($sp)


_hPEnd\intservice:

    /*-BEGIN------- actual INT code to handle -------*/
    addiu $sp, $sp, -16 #
    jal \intservice # call into the c code
    nop
    addiu $sp, $sp, 16  # see above
    /*-END------- actual INT code to handle -------*/


    di $zero    /* disable int--> disable nested interrupts */
    ehb

    bne $s7, $0, _hNestedE\intservice
    nop

    /* now we try to get the  stackpointer of the next task to execute */
    addiu $a0, $s6, 0   /* s6 still contains the original stack pointer of the interrupted task */
    addiu $sp, $sp, -16 /* (area for the called func to store a0-a3) */
    jal RescheduleIfNeeded
    nop
    #addiu $sp, $sp, 16 /*  avoided because we replace sp in the next instruction */

    /* we have in $v0 the stack pointer of the next task */
    addiu $sp, $v0, 0

    /* start to restore */
    lw $fp, 124($sp)
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

    addiu $sp, $sp, 128  /* we restore the original sp */
    #wrpgpr $sp, $sp

    eret  /* back to the caller or orginal program counter */
    nop

_hNestedE\intservice:

    lw $v0, 84($sp)
    mtlo $v0
    lw $v1, 80($sp)
    mthi $v0

    lw $ra, 76($sp)
    lw $s8, 72($sp)
    lw $t9, 68($sp)
    lw $t8, 64($sp)
    lw $t7, 60($sp)
    lw $t6, 56($sp)
    lw $t5, 52($sp)
    lw $t4, 48($sp)
    lw $t3, 44($sp)
    lw $t2, 40($sp)
    lw $t1, 36($sp)
    lw $t0, 32($sp)
    lw $a3, 28($sp)
    lw $a2, 24($sp)
    lw $a1, 20($sp)
    lw $a0, 16($sp)
    lw $v1, 12($sp)
    lw $v0,  8($sp)
    lw $at,  4($sp)
    lw $s7,   ($sp) /* used to hold the previous level */
    di $zero
    ehb
    lw $k0, 92($sp)
    lw $k1, 88($sp)
    mtc0 $k0, $14       /* EPC restored */
    addiu $sp, $sp, 96
    mtc0 $k1, $12       /* status restored */

    eret  /* back to the caller or orginal program counter */
    nop

.endm

