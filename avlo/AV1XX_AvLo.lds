ENTRY(entry_code)

MEMORY
{
   DRAM : ORIGIN = 0x01300000,    LENGTH = 0x00600000
}

SECTIONS
{  
    .text :
    {   
	_start_kernel = .;        
        *(.text)        
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
    } > DRAM    
       
    .bss :
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

