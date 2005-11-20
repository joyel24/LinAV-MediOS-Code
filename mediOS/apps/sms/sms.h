
#ifndef _SMS_H_
#define _SMS_H_

#define TYPE_OVERSEAS   (0)
#define TYPE_DOMESTIC   (1)

/* SMS context */
typedef struct
{
    uint32 fcr[4];
    uint32 paused;
    uint32 save;
    uint32 country;
    uint32 port_3F;
    uint32 port_F2;
    uint32 use_fm;
    uint32 irq;
    uint32 psg_mask;
    uint32 cyclesperline;
}t_sms;


/* Global data */
uint8 sms_ram[0x2000];
uint8 sms_dummy[0x2000];
uint8 sms_sram[0x8000];
/*uint8 *sms_ram;
uint8 *sms_dummy;
uint8 *sms_sram;*/
extern t_sms sms;

/* Function prototypes */
void sms_frame(int skip_render);
void sms_init(void);
void sms_reset(void);
int  sms_irq_callback(int param);
void sms_mapper_w(int address, int data);
void cpu_reset(void);

#endif /* _SMS_H_ */
