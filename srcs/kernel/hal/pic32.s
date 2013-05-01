


.globl swapContext

.text

.ent swapContext
swapContext:
    .set noreorder
    .set nomips16


    eret


.end swapContext

.section .vector_0,code
   j      swapContext
   nop
