/* KL_32_2.ld                                                            */
/* GCC memory file for the Kinetis KL - for use with the uTasker project */
/* Use when running in FLASH (32k FLASH / 2k CPU SRAM)                   */


MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00008000
    SRAM (wx)  : ORIGIN = 0x1ffffec0, LENGTH = 0x00000800-0xc0           /* vector size 0xc0 (192 bytes) */
}

SECTIONS
{
  __SRAM_segment_start__   = 0x1ffffec0;
  __SRAM_segment_end__     = 0x200005ff;
  __Vector_segment_start__ = 0x00000000;
  __Vector_segment_end__   = 0x00000008;
  __Fconfig_segment_start__ = 0x00000400;
  __Fconfig_segment_end__  = 0x00000410;
  __FLASH_segment_start__  = 0x00000410;
  __FLASH_segment_end__    = 0x00007fff;

  __STACKSIZE__ = 0;
  __HEAPSIZE__ = 0;                                                      /* set to non-zero when using library malloc() type functions */


  .vectors ALIGN(__Vector_segment_start__ , 4) :
  {
    __vector_table = .;
    KEEP(*(.vectors .vectors.*))
  }
  __vectors_end__ = __vector_table + SIZEOF(.vectors);

  .f_config ALIGN(__Fconfig_segment_start__ , 4) :
  {
    __flash_config = .;
    KEEP(*(.f_config .f_config.*))
  }

  __init_load_start__ = __FLASH_segment_start__;
  .init ALIGN(__FLASH_segment_start__ , 4) :
  {
    __init_start__ = .;
    *(.init .init.*)
  }
  __init_end__ = __init_start__ + SIZEOF(.init);


  __text_load_start__ = ALIGN(__init_end__ , 4);
  .text ALIGN(__init_end__ , 4) :
  {
    __text_start__ = .;
    *(.text .text.* .glue_7t .glue_7 .gnu.linkonce.t.*)
  }
  __text_end__ = __text_start__ + SIZEOF(.text);

 
  __dtors_load_start__ = ALIGN(__text_end__ , 4);
  .dtors ALIGN(__text_end__ , 4) :
  {
    __dtors_start__ = .;
    KEEP (*(SORT(.dtors.*))) KEEP (*(.dtors))
  }
  __dtors_end__ = __dtors_start__ + SIZEOF(.dtors);

 
  __ctors_load_start__ = ALIGN(__dtors_end__ , 4);
  .ctors ALIGN(__dtors_end__ , 4) :
  {
    __ctors_start__ = .;
    KEEP (*(SORT(.ctors.*))) KEEP (*(.ctors))
  }
  __ctors_end__ = __ctors_start__ + SIZEOF(.ctors);

 
   __rodata_load_start__ = ALIGN(__ctors_end__ , 4);
  .rodata ALIGN(__ctors_end__ , 4) :
  {
    __rodata_start__ = .;
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  }
  __rodata_end__ = __rodata_start__ + SIZEOF(.rodata);


  __fast_load_start__ = ALIGN(__rodata_end__ , 4);
  .fast __SRAM_segment_start__ : AT(ALIGN(__rodata_end__ , 4))
  {
    __fast_start__ = .;
    *(.fast .fast.*)
  }
  __fast_end__ = __fast_start__ + SIZEOF(.fast);

  __fast_load_end__ = __fast_load_start__ + SIZEOF(.fast);

 
  .fast_run ALIGN(__fast_end__ , 4) (NOLOAD) :
  {
    __fast_run_start__ = .;
    . = MAX(__fast_run_start__ + SIZEOF(.fast), .);
  }
  __fast_run_end__ = __fast_run_start__ + SIZEOF(.fast_run);

 
  __data_load_start__ = ALIGN(__fast_load_start__ + SIZEOF(.fast) , 4);
  .data __fast_run_end__ : AT(ALIGN(__fast_load_start__ + SIZEOF(.fast) , 4))
  {
    __data_start__ = .;
    *(.data .data.* .gnu.linkonce.d.*)
  }
  __data_end__ = __data_start__ + SIZEOF(.data);

  __data_load_end__ = __data_load_start__ + SIZEOF(.data);

  __FLASH_segment_used_end__ = ALIGN(__fast_load_start__ + SIZEOF(.fast) , 4) + SIZEOF(.data);

  . = ASSERT((__data_load_start__ + SIZEOF(.data)) >= __FLASH_segment_start__ && (__data_load_start__ + SIZEOF(.data)) <= (__FLASH_segment_end__) , "error: .data is too large to fit in FLASH memory segment");

  .data_run ALIGN(__fast_run_end__ , 4) (NOLOAD) :
  {
    __data_run_start__ = .;
    . = MAX(__data_run_start__ + SIZEOF(.data), .);
  }
  __data_run_end__ = __data_run_start__ + SIZEOF(.data_run);

 
  __bss_load_start__ = ALIGN(__data_run_end__ , 4);
  .bss ALIGN(__data_run_end__ , 4) (NOLOAD) :
  {
    __bss_start__ = .;
    *(.bss .bss.* .gnu.linkonce.b.*) *(COMMON)
  }
  __bss_end__ = __bss_start__ + SIZEOF(.bss);

  
  __non_init_load_start__ = ALIGN(__bss_end__ , 4);
  .non_init ALIGN(__bss_end__ , 4) (NOLOAD) :
  {
    __non_init_start__ = .;
    *(.non_init .non_init.*)
  }
  __non_init_end__ = __non_init_start__ + SIZEOF(.non_init);

 
  __heap_load_start__ = ALIGN(__non_init_end__ , 4);
  .heap ALIGN(__non_init_end__ , 4) (NOLOAD) :
  {
    __heap_start__ = .;
    *(.heap)
    . = MAX(__heap_start__ + __HEAPSIZE__ , .);
  }
  __heap_end__ = __heap_start__ + SIZEOF(.heap);

  . = ASSERT(__heap_end__ >= __SRAM_segment_start__ && __heap_end__ <= (__SRAM_segment_end__) , "error: .heap is too large to fit in SRAM memory segment");

  __stack_load_start__ = ALIGN(__heap_end__ , 4);
  .stack ALIGN(__heap_end__ , 4) (NOLOAD) :
  {
    __stack_start__ = .;
    *(.stack)
    . = MAX(__stack_start__ + __STACKSIZE__ , .);
  }
 
  __stack_end__ = __SRAM_segment_end__;
}

