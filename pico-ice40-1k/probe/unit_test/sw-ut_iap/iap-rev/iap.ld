
MEMORY
{
  FLASH (rx) : ORIGIN = 0x1FFF1000, LENGTH = 0xA00
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 0x3FC0
}

ENTRY(IAP_Handler)

SECTIONS
  {
    .text :
    {
      *(.text) _etext = . ; 
    } > FLASH

  .mdata 0x2000 : 
    AT ( ADDR (.text) + SIZEOF (.text) )
    { _data = . ; *(.data); _edata = . ;  }
  .bss 0x3000 :
    { _bstart = . ;  *(.bss) *(COMMON) ; _bend = . ;}
}
