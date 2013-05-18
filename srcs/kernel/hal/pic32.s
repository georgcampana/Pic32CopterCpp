


.globl swapTaskContext

.text
# must be called with Interrupts already switched off
.ent swapTaskContext
swapTaskContext:
    .set noreorder
    .set nomips16

    # no need to get the right sp since this swapContext can be called from a Task only
    # we start to put the whole gpr on the stack
    # the allocated space/sequence must be the same as i case of an interrupt
    # 108 104   100 96  94   92 88 84 80 76 72 68 64 60 56 52 48 44 40 36 32 28 24 20  16    12    8    4     0
    # epc,status,v0,v1,<pad>,ra,s8,t9,t8,t7,t6,t5,t4,t3,t2,t1,t0,a3,a2,a1,a0,v1,v0,at,<??>(arga3,arga2,arga1,arga0)
    # it must be complete because could be restored from an interrupt
    addiu $sp, $sp, -112  # we create some space on the stack

    mfc0 $k0, $14 #EPC
    mfc0 $k1, $12 #status
/*
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
