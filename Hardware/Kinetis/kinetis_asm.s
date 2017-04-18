;//**********************************************************************
;// Mark Butcher    Bsc (Hons) MPhil MIET
;//
;// M.J.Butcher Consulting
;// Birchstrasse 20f,    CH-5406, Rütihof
;// Switzerland
;//
;// www.uTasker.com    Skype: M_J_Butcher
;//
;// ---------------------------------------------------------------------
;// File:      kinetis_asm.s [Freescale]
;// Project:   Single Chip Embedded Internet
;// ---------------------------------------------------------------------
;// Copyright (C) M.J.Butcher Consulting 2004..2015
;// *********************************************************************
;//
;// This file contains assembler code needed by Keil since it doesn't support in-line assmembler Thumb mode
;//
;
;  Normally there is no assembler startup code required when using the
;  Kinetis. However Keil requires stack pointer and heap settings to be
;  assigned in a routine called	__user_initial_stackheap. This needs
;  to put some values into specific registers. The inline assembler
;  doesn't work in Thumb mode (which the Cortex M4 uses exclusively) and
;  so this has been left here in case someone wishes to use it.
;
;  Note that the heap is not used in the uTasker project (size 0)
;  but can be configured for the Keil library use here.
;  The stack values are not initialised since the uTasker
;  startup code sets it to the top of RAM and it can be 
;  left there (see const RESET_VECTOR reset_vect in 
;  kinetis.c)
;  12.09.2014 Add KINETIS_M0 start_application code (add define KINETIS_M0 to the assembler define setting to use)
;  26.02.2015 Add uMask_Interrupt() for K devices

Stack   EQU     0x00000100
Heap    EQU     0x00000000

; space for heap can be allocated here
;
        AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
HeapMem
        SPACE   Heap
__heap_limit

        PRESERVE8

        AREA    |.text|, CODE, READONLY

        EXPORT  __user_initial_stackheap
__user_initial_stackheap
        LDR     R0, = HeapMem
;       LDR     R1, = (StackMem + Stack)      - not used
        LDR     R2, = (HeapMem + Heap)
;       LDR     R3, = StackMem                - not used
        BX      LR

        ALIGN

    AREA    |.text|, CODE, READONLY

	ALIGN

    EXPORT start_application
start_application
        IF :DEF:CORTEX_M0
    ldr r1, [r0,#0]                                                      ; get the stack pointer value from the program's reset vector	
    mov sp, r1                                                           ; copy the value to the stack pointer
    ldr r0, [r0,#4]                                                      ; get the program counter value from the program's reset vector
    blx r0                                                               ; jump to the start address
        ELSE
    ldr sp, [r0,#0]                                                      ; load the stack pointer value from the program's reset vector
    ldr pc, [r0,#4]                                                      ; load the program counter value from the program's reset vector to cause operation to continue from there
        ENDIF	
    ALIGN


    IF :DEF:CORTEX_M0
    ELSE
    EXPORT uMask_Interrupt
uMask_Interrupt
    msr basepri, r0                                                      ; modify the base priority to block interrupts with a lower priority than this level
    bx lr                                                                ; return

    ALIGN
    ENDIF

    END
