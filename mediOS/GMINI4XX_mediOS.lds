ENTRY(entry_code)

MEMORY
{
   DRAM : ORIGIN = 0x00C10000,    LENGTH = 0x01000000
   IRAM : ORIGIN = 0x00000004,    LENGTH = 0x00007800-0x4
}

SECTIONS
{  
    .text :
    {   
        *(.text.init)             
        *(.text)        
        *(.gnu.linkonce.t.*)        
        *(.glue_7t)
        *(.glue_7)        
    } > DRAM
    
    .rodata :
    {
        *(.rodata)
        *(.rodata.str1.4)
    } > DRAM
  
    .data :    
    {
        *(.data)
				*(.got)
				*(.got.*)
        _real_iram_start = .;
    } > DRAM
    
    .iram 0x04 : AT ( _real_iram_start )
    {    
        _iram_start = .;
        *(.vectors)
        *(.core)
        *(.cored)
        _iram_end = .;
    } > IRAM
    
    .bss (NOLOAD) : AT ( _real_iram_start )
    {
        _bss_start = .;
        *(.bss)
        _bss_end = ALIGN(0x04);
    } > DRAM
    
    .common :
    {
        *(.common)
        _end_kernel = . ;
    } > DRAM
}