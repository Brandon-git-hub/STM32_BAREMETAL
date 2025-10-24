    .syntax unified
    .cpu cortex-m0
    .thumb

    .extern _estack
    .extern _sidata   /* Flash load addr of .data */
    .extern _sdata    /* RAM start of .data */
    .extern _edata    /* RAM end   of .data */
    .extern _sbss     /* RAM start of .bss */
    .extern _ebss     /* RAM end   of .bss */
    .extern main

    .global Reset_Handler

    .section .isr_vector, "a", %progbits
    .word _estack
    .word Reset_Handler
    .word 0      /* NMI */
    .word 0      /* HardFault */
    .rept 12
     .word 0
    .endr

    .text
    .thumb

Reset_Handler:
    /* -------- copy .data: for (dst=_sdata; dst<_edata; dst+=4, src+=4) *dst = *src; */
    ldr   r0, =_sdata
    ldr   r1, =_edata
    ldr   r2, =_sidata
1:
    cmp   r0, r1
    bcc   2f              /* if (r0 < r1) go copy */
    b     3f              /* else done with .data */
2:
    ldr   r3, [r2]        /* r3 = *src */
    adds  r2, #4          /* src += 4 */
    str   r3, [r0]        /* *dst = r3 */
    adds  r0, #4          /* dst += 4 */
    b     1b

3:  /* -------- zero .bss: for (p=_sbss; p<_ebss; p+=4) *p = 0; */
    ldr   r0, =_sbss
    ldr   r1, =_ebss
    movs  r2, #0
4:
    cmp   r0, r1
    bcc   5f
    b     6f
5:
    str   r2, [r0]        /* *p = 0 */
    adds  r0, #4          /* p += 4 */
    b     4b

6:  /* -------- call main -------- */
    bl    main

7:  /* if main returns, loop forever */
    b     7b

    /* <-- 保留這個換行，避免 EOF 無換行警告 */
