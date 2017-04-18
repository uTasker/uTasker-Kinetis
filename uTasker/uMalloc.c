/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      uMalloc.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    12.10.2007 uMallocAlign() quantity extended to 64k                   {1}
    05.05.2009 Add conditional compile on UNUSED_STACK_PATTERN           {2}
    02.02.2010 Add optional uFree()                                      {3}
    28.05.2010 Add stack monitoring support in different memory to heap  {4}
    12.12.2011 Add optional UNUSED_HEAP_PATTERN to aid debugging         {5}
    29.05.2013 Add optional second heap in fixed sized memory area       {6}
    06.11.2015 Modify fnStackFree() to allow worst-case used stack to be returned {7}

*/


#include "config.h"


#if defined _WINDOWS
    static unsigned char ucHeap[OUR_HEAP_SIZE] = {0};                    // space for heap simulation
    extern void *fnGetHeapStart(void) { return ucHeap; }
#else
    #if defined (_GNU)
        extern unsigned char _end;                                       // variable at last location in RAM (GNU linker)
    #elif defined (COMPILE_KEIL)
        extern unsigned char __libspace_start;
    #endif
#endif

#if defined SECONDARY_UMALLOC                                            // {6}
    static void fnInitSecondaryArea(void);
#endif

// Minimalist heap management without free support
//
static HEAP_REQUIREMENTS present_HeapSize = 0;
static unsigned char *pucBottomOfHeap = 0;
static unsigned char *pucTopOfHeap = 0;

#if defined SECONDARY_UMALLOC                                            // {6}
static unsigned char *pucBottomOfHeap2 = 0;
static unsigned char *pucTopOfHeap2 = 0;
static unsigned long present_HeapSize2 = 0;
#endif

extern void fnInitialiseHeap(const HEAP_NEEDS *ctOurHeap, void *HeapStart)
{
#if defined UNUSED_STACK_PATTERN                                         // {2}
    unsigned char *ucPattern;
    #if !defined _WINDOWS 
    unsigned char ucTopOfStack;                                          // put a temporary variable on stack
    #endif
#endif
#if defined UNUSED_HEAP_PATTERN
    unsigned char *ptrHeap;
#endif

    present_HeapSize = 0;                                                // initially no heap has been allocated
    pucTopOfHeap = pucBottomOfHeap = (unsigned char *)HeapStart;         // mark bottom of heap

    while (ctOurHeap->ConfigNr != OurConfigNr) {
        if (ctOurHeap->ConfigNr == 0) {
            return;                                                      // end of list - node not found
        }
        ctOurHeap++;
    }
    pucTopOfHeap += ctOurHeap->need_this_amount;                         // we need the following amount of memory for heap use

#if defined UNUSED_HEAP_PATTERN
    ptrHeap = pucBottomOfHeap;
    while (ptrHeap < pucTopOfHeap) {
        *ptrHeap++ = UNUSED_HEAP_PATTERN;                                // {5} initialise heap with pattern
    }
#endif
#if defined UNUSED_STACK_PATTERN                                         // {2}
    #if defined STACK_START_ADDRESS                                      // {4}
    ucPattern = (unsigned char *)STACK_START_ADDRESS;                    // fixed stack start address
    #else
    ucPattern = pucTopOfHeap;                                            // stack is located after heap
    #endif
    #if !defined _WINDOWS                                                // we now fill a pattern from the top of stack until the top of heap
    while (ucPattern <= &ucTopOfStack) {                                 // this pattern is used later for Stack use monitoring
        *ucPattern++ = UNUSED_STACK_PATTERN;
    }
    #endif
#endif
#if defined SECONDARY_UMALLOC                                            // {6}
    fnInitSecondaryArea();                                               // perform initialisation of the secondary area with fixed locatiosn and sizes
#endif
}


extern void *uMalloc(MAX_MALLOC __size)
{
#if defined _ALIGN_HEAP_4
    unsigned char *ptr = pucBottomOfHeap + present_HeapSize;
    if ((unsigned long)ptr & 0x3) {                                      // ensure all memory is aligned on 4 byte boundary
        ptr = (unsigned char *)((unsigned long)ptr & ~0x3);
        ptr += 4;
    }
#else
    unsigned char *ptr = pucBottomOfHeap + present_HeapSize;             // packs with maximum density for processors with no restrictions
#endif

    if ((ptr + __size) >= pucTopOfHeap) {
#if defined _WINDOWS
        _EXCEPTION("uMalloc failed!!");                                  // generate exception so that the problem can be immediately seen and cured
#endif
        return 0;                                                        // no more room - it will be necessary to increase heap size
    }

    uMemset(ptr, 0, __size);                                             // ensure new memory chunk is always zeroed
#if defined _ALIGN_HEAP_4
    present_HeapSize = (ptr - pucBottomOfHeap) + __size;
#else
    present_HeapSize += __size;                                          // new heap size
#endif
    return ((void *)ptr);
}

#if defined _MALLOC_ALIGN
extern void *uMallocAlign(MAX_MALLOC __size, unsigned short usAlign)     // {1}
{
    unsigned char *ptr = pucBottomOfHeap + present_HeapSize;
    if ((unsigned long)ptr & (usAlign - 1)) {                            // ensure all memory is aligned on specified byte boundary
        ptr = (unsigned char *)((unsigned long)ptr & ~(usAlign - 1));
        ptr += usAlign;
    }

    if ((ptr + __size) >= pucTopOfHeap) {
    #if defined _WINDOWS
         _EXCEPTION("uMallocAlign failed!!");                            // generate exception so that the problem can be immediately seen and cured
    #endif
        return 0;                                                        // no more room - it will be necessary to increase heap size
    }

    #if defined RUN_LOOPS_IN_RAM
        #if !defined DMA_MEMCPY_SET || defined DEVICE_WITHOUT_DMA
    if (uMemset) {                                                       // don't do this before the routine has been installed
        uMemset(ptr, 0, __size);                                         // ensure new memory chunk is always zeroed
    }
        #else
    uMemset(ptr, 0, __size);                                             // ensure new memory chunk is always zeroed
        #endif
    #else
    uMemset(ptr, 0, __size);                                             // ensure new memory chunk is always zeroed
    #endif

    present_HeapSize = (ptr - pucBottomOfHeap) + __size;
    return ((void *)ptr);
}
#endif

#if defined SUPPORT_UFREE                                                // {3}
// This function allows the present malloc() state to be saved and later restored, in order to free allocated memory regions for reuse
// This only supports a single region which must be on the top of the uMalloc() area - this means that it should usually only be used once all fixed
// resources have already been fully allocated
//
extern MAX_MALLOC uFree(int iFreeRegion)
{
    static MAX_MALLOC free_start = 0;                                    // invalid
    if (iFreeRegion == 0) {                                              // mark the start of a single region
        free_start = present_HeapSize;
    }
    else {                                                               // free the region
        if (free_start != 0) {
            MAX_MALLOC freed_heap = (present_HeapSize - free_start);
            present_HeapSize = free_start;                               // set the uMalloc() size back to when the free region was originally marked
            free_start = 0;                                              // invalidate
            return freed_heap;                                           // the size of heap that was freed
        }
    }
    return 0;
}
#endif

extern STACK_REQUIREMENTS fnStackFree(STACK_REQUIREMENTS *stackUsed)     // {7}
{
#if defined UNUSED_STACK_PATTERN && !defined _WINDOWS                    // {2}
    #if defined STACK_START_ADDRESS                                      // {4}
    unsigned char *ptrStack = (unsigned char *)STACK_START_ADDRESS;      // fixed stack start address
    #else
    unsigned char *ptrStack = pucTopOfHeap;                              // stack begins after heap
    #endif
    STACK_REQUIREMENTS FreeStack = 0;

    while (*(++ptrStack) == UNUSED_STACK_PATTERN) {
        FreeStack++;                                                     // count free locations in RAM never used until now
    }
    if (FreeStack != 0) {
        ptrStack = pucTopOfHeap;
    }
    *stackUsed = ((unsigned char *)((RAM_START_ADDRESS + (SIZE_OF_RAM - NON_INITIALISED_RAM_SIZE))) - ptrStack); // {7}
    return (FreeStack);
#else
    *stackUsed = 0;
    return 0;
#endif
}

extern HEAP_REQUIREMENTS fnHeapAvailable(void)
{
    return (pucTopOfHeap - pucBottomOfHeap);
}

extern HEAP_REQUIREMENTS fnHeapFree(void)
{
    return ((pucTopOfHeap - pucBottomOfHeap) - present_HeapSize);
}

#if defined SECONDARY_UMALLOC                                            // {6}
    #if defined _WINDOWS
static unsigned char ucHeap2[HEAP2_SIZE] = {0};                          // heap memory used by the simulator
    #endif

// The secondary uMalloc area is usually a free area of fixed sized memory - such as large external SRAM or SDRAM - that is not used for variable storage.
// An equivalent uMalloc interface is used but the user can define which dynamic variables are in which area
//
static void fnInitSecondaryArea(void)
{
    present_HeapSize2 = 0;                                               // initially no heap has been allocated
    pucBottomOfHeap2 = (unsigned char *)HEAP2_ADDRESS;                   // mark bottom of heap
    pucTopOfHeap2 = (pucBottomOfHeap2 + HEAP2_SIZE);                     // mark top of heap
}

extern void *uMalloc2(unsigned long __size)
{
    #if defined _ALIGN_HEAP_4
        #if defined _WINDOWS
    unsigned char *_ptr;
        #endif
    unsigned char *ptr = (pucBottomOfHeap2 + present_HeapSize2);
    if ((unsigned long)ptr & 0x3) {                                      // ensure all memory is aligned on 4 byte boundary
        ptr = (unsigned char *)((unsigned long)ptr & ~0x3);
        ptr += 4;
    }
    #else
    unsigned char *ptr = (pucBottomOfHeap2 + present_HeapSize2);         // packs with maximum density for processors with no restrictions
    #endif

    if ((ptr + __size) >= pucTopOfHeap2) {
    #if defined _WINDOWS
        _EXCEPTION("uMalloc2 failed!!");                                 // generate exception so that the problem can be immediately seen and cured
    #endif
        return 0;                                                        // no more room - it will be necessary to increase heap size
    }
    #if defined _WINDOWS
        #if defined _ALIGN_HEAP_4
    _ptr = ptr;                                                          // backup original pointer
        #endif
    ptr = (ucHeap2 + (ptr - (unsigned char *)HEAP2_ADDRESS));            // move to simulated heap space
    #endif

    uMemset(ptr, 0, __size);                                             // ensure new memory chunk is always zeroed
    #if defined _ALIGN_HEAP_4
        #if defined _WINDOWS
    present_HeapSize2 = (_ptr - pucBottomOfHeap2) + __size;
        #else
    present_HeapSize2 = (ptr - pucBottomOfHeap2) + __size;
        #endif
    #else
    present_HeapSize2 += __size;                                         // new heap size
    #endif
    return ((void *)ptr);
}

    #if defined _MALLOC_ALIGN
extern void *uMallocAlign2(unsigned long __size, unsigned short usAlign)
{
        #if defined _WINDOWS
    unsigned char *_ptr;
        #endif
    unsigned char *ptr = (pucBottomOfHeap2 + present_HeapSize2);
    if ((unsigned long)ptr & (usAlign - 1)) {                            // ensure all memory is aligned on specified byte boundary
        ptr = (unsigned char *)((unsigned long)ptr & ~(usAlign - 1));
        ptr += usAlign;
    }

    if ((ptr + __size) >= pucTopOfHeap2) {
        #if defined _WINDOWS
         _EXCEPTION("uMallocAlign2 failed!!");                           // generate exception so that the problem can be immediately seen and cured
        #endif
        return 0;                                                        // no more room - it will be necessary to increase heap size
    }

        #if defined _WINDOWS
    _ptr = ptr;                                                          // backup original pointer
    ptr = (ucHeap2 + (ptr - (unsigned char *)HEAP2_ADDRESS));            // move to simulated heap space
        #endif

        #if defined RUN_LOOPS_IN_RAM
            #if !defined DMA_MEMCPY_SET || defined DEVICE_WITHOUT_DMA
    if (uMemset != 0) {                                                  // don't do this before the routine has been installed
        uMemset(ptr, 0, __size);                                         // ensure new memory chunk is always zeroed
    }
            #else
    uMemset(ptr, 0, __size);                                             // ensure new memory chunk is always zeroed
            #endif
        #else
    uMemset(ptr, 0, __size);                                             // ensure new memory chunk is always zeroed
        #endif

        #if defined _WINDOWS
    present_HeapSize2 = (_ptr - pucBottomOfHeap2) + __size;
        #else
    present_HeapSize2 = (ptr - pucBottomOfHeap2) + __size;
        #endif
    return ((void *)ptr);
}
    #endif

    #if defined SUPPORT_UFREE
extern unsigned long uFree2(int iFreeRegion)
{
    static unsigned long free_start2 = 0;                                // invalid
    if (iFreeRegion == 0) {                                              // mark the start of a single region
        free_start2 = present_HeapSize2;
    }
    else {                                                               // free the region
        if (free_start2 != 0) {
            unsigned long freed_heap = (present_HeapSize2 - free_start2);
            present_HeapSize2 = free_start2;                             // set the uMalloc2() size back to when the free region was originally marked
            free_start2 = 0;                                             // invalidate
            return freed_heap;                                           // the size of heap that was freed
        }
    }
    return 0;
}
    #endif
#endif

