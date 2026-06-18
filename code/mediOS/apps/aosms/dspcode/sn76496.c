
#include "sn76496.h"

#define MAX_OUTPUT  0x7FFF
#define STEP        0x10000L

#define FB_WNOISE   0x14002L
#if 0
#define FB_PNOISE   0x8000
#define NG_PRESET   0x0F35
#else
#define FB_PNOISE   0x8000
#define NG_PRESET   0x0001
#endif

t_SN76496 sn;

void SN76496Write(unsigned char data)
{
	int n;

	if (data & 0x80)
	{
		int r = (data & 0x70) >> 4;
		int c = r/2;

		sn.LastRegister = r;
		sn.Register[r] = (sn.Register[r] & 0x3f0) | (data & 0x0f);
		switch (r)
		{
			case 0:	/* tone 0 : frequency */
			case 2:	/* tone 1 : frequency */
			case 4:	/* tone 2 : frequency */
				sn.Period[c] = sn.UpdateStep * sn.Register[r];
				if (sn.Period[c] == 0) sn.Period[c] = sn.UpdateStep;
				if (r == 4)
				{
					/* update noise shift frequency */
					if ((sn.Register[6] & 0x03) == 0x03)
						sn.Period[3] = 2 * sn.Period[2];
				}
				break;
			case 1:	/* tone 0 : volume */
			case 3:	/* tone 1 : volume */
			case 5:	/* tone 2 : volume */
			case 7:	/* noise  : volume */
				sn.Volume[c] = VolTable[data & 0x0f];
				break;
			case 6:	/* noise  : frequency, mode */
				{
					int n = sn.Register[6];
					sn.NoiseFB = (n & 4) ? FB_WNOISE : FB_PNOISE;
					n &= 3;
					/* N/512,N/1024,N/2048,Tone #3 output */
					sn.Period[3] = ((n&3) == 3) ? 2 * sn.Period[2] : (sn.UpdateStep << (5+(n&3)));
					sn.Period[3]=sn.Period[3]<<4;

					/* reset noise shifter */
					sn.RNG = NG_PRESET;
					sn.Output[3] = sn.RNG & 1;
				}
				break;
		}
	}
	else
	{
		int r = sn.LastRegister;
		int c = r/2;

		switch (r)
		{
			case 0:	/* tone 0 : frequency */
			case 2:	/* tone 1 : frequency */
			case 4:	/* tone 2 : frequency */
				sn.Register[r] = (sn.Register[r] & 0x0f) | ((data & 0x3f) << 4);
				sn.Period[c] = sn.UpdateStep * sn.Register[r];
				if (sn.Period[c] == 0) sn.Period[c] = sn.UpdateStep;
				if (r == 4)
				{
					/* update noise shift frequency */
					if ((sn.Register[6] & 0x03) == 0x03)
						sn.Period[3] = 2 * sn.Period[2];
				}
				break;
			case 1:	/* tone 0 : volume */
			case 3:	/* tone 1 : volume */
			case 5:	/* tone 2 : volume */
			case 7:	/* noise  : volume */
				sn.Volume[c] = VolTable[data & 0x0f];
				sn.Register[r] = (sn.Register[r] & 0x3f0) | (data & 0x0f);
				break;
			case 6:	/* noise  : frequency, mode */
				{
					sn.Register[r] = (sn.Register[r] & 0x3f0) | (data & 0x0f);
					n = sn.Register[6];
					sn.NoiseFB = (n & 4) ? FB_WNOISE : FB_PNOISE;
					n &= 3;
					/* N/512,N/1024,N/2048,Tone #3 output */
					sn.Period[3] = ((n&3) == 3) ? 2 * sn.Period[2] : (sn.UpdateStep << (5+(n&3)));
					sn.Period[3]=sn.Period[3]<<4;

					/* reset noise shifter */
					sn.RNG = NG_PRESET;
					sn.Output[3] = sn.RNG & 1;
				}
				break;
		}
	}
}



void SN76496Update(short *buffer[2],int length, unsigned char mask)
{
    int i, j;
    int buffer_index = 0;

	/* If the volume is 0, increase the counter */
	for (i = 0;i < 4;i++)
	{
		if (sn.Volume[i] == 0)
		{
			/* note that I do count += length, NOT count = length + 1. You might think */
			/* it's the same since the volume is 0, but doing the latter could cause */
			/* interferencies when the program is rapidly modulating the volume. */
			if (sn.Count[i] <= length*STEP) sn.Count[i] += length*STEP;
		}
	}

	while (length > 0)
	{
		long vol[4];
		unsigned long out[2];
		long left;


		/* vol[] keeps track of how long each square wave stays */
		/* in the 1 position during the sample period. */
		vol[0] = vol[1] = vol[2] = vol[3] = 0;

		for (i = 0;i < 3;i++)
		{
			if (sn.Output[i]) vol[i] += sn.Count[i];
			sn.Count[i] -= STEP;
			/* Period[i] is the half period of the square wave. Here, in each */
			/* loop I add Period[i] twice, so that at the end of the loop the */
			/* square wave is in the same status (0 or 1) it was at the start. */
			/* vol[i] is also incremented by Period[i], since the wave has been 1 */
			/* exactly half of the time, regardless of the initial position. */
			/* If we exit the loop in the middle, Output[i] has to be inverted */
			/* and vol[i] incremented only if the exit status of the square */
			/* wave is 1. */
			while (sn.Count[i] <= 0)
			{
				sn.Count[i] += sn.Period[i];
				if (sn.Count[i] > 0)
				{
					sn.Output[i] ^= 1;
					if (sn.Output[i]) vol[i] += sn.Period[i];
					break;
				}
				sn.Count[i] += sn.Period[i];
				vol[i] += sn.Period[i];
			}
			if (sn.Output[i]) vol[i] -= sn.Count[i];
		}

		left = STEP;
		do
		{
			long nextevent;


			if (sn.Count[3] < left) nextevent = sn.Count[3];
			else nextevent = left;

			if (sn.Output[3]) vol[3] += sn.Count[3];
			sn.Count[3] -= nextevent;
			if (sn.Count[3] <= 0)
			{
				if (sn.RNG & 1) sn.RNG ^= sn.NoiseFB;
				sn.RNG >>= 1;
				sn.Output[3] = sn.RNG & 1;
				sn.Count[3] += sn.Period[3];
				if (sn.Output[3]) vol[3] += sn.Period[3];
			}
			if (sn.Output[3]) vol[3] -= sn.Count[3];

			left -= nextevent;
		} while (left > 0);

		out[0] = out[1] = 0;

        for(j = 0; j < 4; j += 1)
        {
            long k = vol[j] * sn.Volume[j];

            if(mask & (1 << (4+j))) out[0] += k;
            if(mask & (1 << (0+j))) out[1] += k;
        }

        if(out[0] > MAX_OUTPUT * STEP) out[0] = MAX_OUTPUT * STEP;
        if(out[1] > MAX_OUTPUT * STEP) out[1] = MAX_OUTPUT * STEP;

        buffer[0][buffer_index] = out[0] / STEP;
        buffer[1][buffer_index] = out[1] / STEP;

        /* Next sample set */
        buffer_index += 1;

		length--;
	}
}

void SN76496_set_clock(long clock)
{
	sn.UpdateStep = ((double)STEP * sn.SampleRate * 16) / clock;
}



int SN76496_init(long clock,long sample_rate)
{
	int i;

	sn.SampleRate = sample_rate;
	SN76496_set_clock(clock);

	for (i = 0;i < 4;i++) sn.Volume[i] = 0;

	sn.LastRegister = 0;
	for (i = 0;i < 8;i+=2)
	{
		sn.Register[i] = 0;
		sn.Register[i + 1] = 0x0f;	/* volume = 0 */
	}

	for (i = 0;i < 4;i++)
	{
		sn.Output[i] = 0;
		sn.Period[i] = sn.Count[i] = sn.UpdateStep;
	}
	sn.RNG = NG_PRESET;
	sn.Output[3] = sn.RNG & 1;

	return 0;
}
