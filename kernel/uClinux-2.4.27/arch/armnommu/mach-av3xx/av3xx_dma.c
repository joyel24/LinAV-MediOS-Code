#include <linux/kernel.h>

#include <asm/io.h>

#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_dma.h>

#define av3xx_dma_set_src(addr)      outw((addr>>16)&0xFFFF,AV3XX_DMA_SRC_HI);outw(addr&0xFFFF,AV3XX_DMA_SRC_LO);
#define av3xx_dma_set_dest(addr)     outw((addr>>16)&0xFFFF,AV3XX_DMA_DEST_HI);outw(addr&0xFFFF,AV3XX_DMA_DEST_LO);
#define av3xx_dma_set_size(size)     outw(size&0xFFFF,AV3XX_DMA_SIZE);
#define av3xx_dma_set_dev(src,dest)  outw(((src<<8)&0xFF00) | (dest&0x00FF),AV3XX_DMA_DEV_SEL);
#define av3xx_dma_start              outw(inw(AV3XX_DMA_STATE) | 0x1,AV3XX_DMA_START);
#define av3xx_dma_state              ((inw(AV3XX_DMA_STATE)&0x1)==0x1)

int av_ata_RW_DMA_Sector(unsigned int lba,int size,void * buffer,int direction)
{
    if(av3xx_dma_state)
        return 0;
    if(direction==AV_DMA_READ)
    {
        av3xx_dma_set_src(lba)
        av3xx_dma_set_dest((unsigned int)buffer)
        av3xx_dma_set_size(size)
        av3xx_dma_set_dev(AV3XX_DMA_ATA,AV3XX_DMA_SDRAM)
        av3xx_dma_start
    }
    else
    {
        av3xx_dma_set_dest(lba)
        av3xx_dma_set_src((unsigned int)buffer)
        av3xx_dma_set_size(size)
        av3xx_dma_set_dev(AV3XX_DMA_SDRAM,AV3XX_DMA_ATA)
        av3xx_dma_start
    }
    return 1;
}
