# Georg Campana  Hardware Abstraction Layer
#

.globl swapTaskContext

.text
# must be called with Interrupts already switched off
.ent swapTaskContext
swapTaskContext:
    .set noreorder
    .set nomips16
    .set noat

    # no need to get the right sp since this swapContext can be called from a Task only
    # we start to put the whole gpr on the stack
    # the allocated space/sequence must be the same as in case of an interrupt
    # because an int can reschedule the current task
    # steps: create stack space; save status, save gpr, save orig sp;
    #        take new task's sp; restore gpr; restore status; remove sp space
    #  116,  112,108,104,100,96,92,88,84,80,76,72,68,64,60,56,52,48,44,40,36,32,28,24,20,16,12, 8, 4, 0
    # status,lo ,hi, ra, s8, s7,s6,s5,s4,s3,s2,s1,s0,t9,t8,t7,t6,t5,t4,t3,t2,t1,t0,a3,a2,a1,a0,v1,v0,at

    addiu $sp, $sp, -120  # we create some space on the stack
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
    #mfc0 $v0, 12 #Status
    #sw $v0, 116($sp)
    # ---- OLD -----

    # ---- NEW ------
    # From here we start to take the gpr we still have the new task sp in a1
    addiu $sp, $a1, 0
    #lw $v0, 116($sp)
    #mtc0 $v0, $12 #Status
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

    lw $sp, ($a0) # we restore the original sp

    addiu $sp, $sp, 120  # we restore the original sp
    jr $ra   # we jump back


/*
    # this is the gpr on the stack after the INT prologue
    # 108 104   100 96  94   92 88 84 80 76 72 68 64 60 56 52 48 44 40 36 32 28 24 20  16    12    8    4     0
    # epc,status,lo,hi,<pad>,ra,s8,t9,t8,t7,t6,t5,t4,t3,t2,t1,t0,a3,a2,a1,a0,v1,v0,at,<??$0>(arga3,arga2,arga1,arga0)
    # it must be completed with the missing registers because it could be restored from an interrupt
9D003744  415DE800   RDPGPR SP, SP
9D003748  401A7000   MFC0 K0, EPC
9D00374C  401B6000   MFC0 K1, Status
9D003750  27BDFF90   ADDIU SP, SP, -112
9D003754  AFBA006C   SW K0, 108(SP)
9D003758  AFBB0068   SW K1, 104(SP)
9D00375C  7C1B7844   INS K1, ZERO, 1, 15
9D003760  377B1400   ORI K1, K1, 5120
9D003764  409B6000   MTC0 K1, Status
9D003768  AFBF005C   SW RA, 92(SP)
9D00376C  AFBE0058   SW S8, 88(SP)
9D003770  AFB90054   SW T9, 84(SP)
9D003774  AFB80050   SW T8, 80(SP)
9D003778  AFAF004C   SW T7, 76(SP)
9D00377C  AFAE0048   SW T6, 72(SP)
9D003780  AFAD0044   SW T5, 68(SP)
9D003784  AFAC0040   SW T4, 64(SP)
9D003788  AFAB003C   SW T3, 60(SP)
9D00378C  AFAA0038   SW T2, 56(SP)
9D003790  AFA90034   SW T1, 52(SP)
9D003794  AFA80030   SW T0, 48(SP)
9D003798  AFA7002C   SW A3, 44(SP)
9D00379C  AFA60028   SW A2, 40(SP)
9D0037A0  AFA50024   SW A1, 36(SP)
9D0037A4  AFA40020   SW A0, 32(SP)
9D0037A8  AFA3001C   SW V1, 28(SP)
9D0037AC  AFA20018   SW V0, 24(SP)
9D0037B0  AFA10014   SW AT, 20(SP)
9D0037B4  00001012   MFLO V0
9D0037B8  AFA20064   SW V0, 100(SP)
9D0037BC  00001810   MFHI V1
9D0037C0  AFA30060   SW V1, 96(SP)
9D0037C4  03A0F021   ADDU S8, SP, ZERO
*/


    jr $ra #return from subroutine


.end swapTaskContext

.section .vector_0,code
   j      swapTaskContext
   nop
