ENTRY(entry_code)
INPUT(boot.o)

MEMORY
{
   DRAM : ORIGIN = 0x03000000,    LENGTH = 0x01000000
   ARAM : ORIGIN = 0x00000004,    LENGTH = 0x00007C00-0x4
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
        _real_core_start = .;
    } > DRAM
    
    .cores 0x04 : AT ( _real_core_start )
    {    
        _core_code_start = .;
        *(.vectors)
        *(.core)
        *(.cored)
        _core_code_end = .;
    } > ARAM
    
    .bss :
    {
        *(.bss)
        _end_kernel = . ;
    } > DRAM
}