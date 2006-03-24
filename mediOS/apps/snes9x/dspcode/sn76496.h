#ifndef SN76496_H
#define SN76496_H

typedef struct
{
	int Channel;
	long SampleRate;
	unsigned int UpdateStep;
    long Register[8];
    long LastRegister;
    unsigned long Volume[4];
    unsigned long RNG;
    long NoiseFB;
	long Period[4];
	long Count[4];
	long Output[4];
}t_SN76496;

static const unsigned short int VolTable[16] =
{
    5461, 5461, 5461, 4653, 3814, 3043, 2473, 1977,
    1573, 1212, 973,  753,   588,  459,  367,  0
};

extern t_SN76496 sn;

void SN76496Write(unsigned char data);
void SN76496Update(short *buffer[2],int length,unsigned char mask);
void SN76496_set_clock(long clock);
int SN76496_init(long clock,long sample_rate);

#endif
