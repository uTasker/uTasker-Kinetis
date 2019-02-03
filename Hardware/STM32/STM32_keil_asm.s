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
;// File:      STM32_keil_asm.s
;// Project:   Single Chip Embedded Internet
;// ---------------------------------------------------------------------
;// Copyright (C) M.J.Butcher Consulting 2004..2018
;// *********************************************************************
;//
;// This file contains assembler code needed by Keil since it doesn't support in-line assmebler Thumb mode
;//
;
;  Normally there is no assembler startup code required when using the
;  STM32. However Keil requires stack pointer and heap settings to be
;  assigned in a routine called	__user_initial_stackheap. This needs
;  to put some values into specific registers. The inline assembler
;  doesn't work in Thumb mode (which the Cortex M3/M4/M7 uses exclusively) and
;  so this has been left here in case someone wishes to use it.
;
;  Note that the heap is not used in the uTasker project (size 0)
;  but can be configured for Keil library use here.
;  The stack values are not initialised since the uTasker
;  startup code sets it to the top of RAM and it can be 
;  left there (see const RESET_VECTOR reset_vect in 
;  STM32.c)

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
    ldr sp, [r0,#0]
    ldr pc, [r0,#4]

    ALIGN

    END
