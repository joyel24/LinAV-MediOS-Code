ENTRY(entry_code)

MEMORY
{
   DRAM : ORIGIN = 0x03000000,    LENGTH = 0x01000000
   IRAM : ORIGIN = 0x00000004,    LENGTH = 0x00007C00-0x4
}

SECTIONS
{  
    .text :
    {   
        *(.text.init)             
        *(.text)        
        *(.glue_7t)
        *(.glue_7)        
    } > DRAM
    
    .rodata :
    {
        *(.rodata)
    } > DRAM
  
    .data :    
    {
        *(.data)
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
    
    .bss :
    {
        *(.bss)
    } > DRAM
    
    .common :
    {
        *(.common)
        _end_kernel = . ;
    } > DRAM
}