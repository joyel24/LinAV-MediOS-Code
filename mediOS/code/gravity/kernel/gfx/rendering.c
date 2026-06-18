/* 
*   kernel/gfx/rendering.c
*
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>
#include <kernel/kernel.h>

#include <types.h>

#define FIXED_SHIFT (8)
#define FIXED_DENOM (1<<FIXED_SHIFT)
#define FIXED_MASK  (FIXED_DENOM - 1)
#define FIXED_HALF  (1<<(FIXED_SHIFT-1))

#define minimum(v1, v2) ((v1 < v2)?v1:v2)
#define maximum(v1, v2) ((v1 > v2)?v1:v2)
#define bound(v,min,max) (( (v)>(min) )?( ( (v)<(max) )?(v):(max) ):(min))
#define swap(T, a, b) { T temp = a; a = b; b = temp; }

__IRAM_CODE int RenderTriangle (
	GFX_3DVERTEX* v0,
	GFX_3DVERTEX* v1,
	GFX_3DVERTEX* v2,
	unsigned char* pTexture,
	const long nTexWidth,
	const long nTexHeight,
	const long nTexStride,
	const long nClipX1,
	const long nClipY1,
	const long nClipX2,
	const long nClipY2,
	unsigned char* pOutput,
	const long nWidth,
	const long nHeight,
	const long nStride,
	long* pZBuffer)
{
	const long nXLow = minimum(maximum(nClipX1, 0), nWidth);
	const long nYLow = minimum(maximum(nClipY1, 0), nHeight);

	const long nXHigh = maximum(minimum(nClipX2, nWidth), 0);
	const long nYHigh = maximum(minimum(nClipY2, nHeight), 0);

	const long nW = nXHigh - nXLow;
	const long nH = nYHigh - nYLow;

	if (nW <= 0)
		return -1;

	if (nH <= 0)
		return -1;

	if (v0->ix < nXLow)
		if (v1->ix < nXLow)
			if (v2->ix < nXLow)
				return -2;

	if (v0->iy < nYLow)
		if (v1->iy < nYLow)
			if (v2->iy < nYLow)
				return -2;

	if (v0->ix >= nXHigh)
		if (v1->ix >= nXHigh)
			if (v2->ix >= nXHigh)
				return -2;

	if (v0->iy >= nYHigh)
		if (v1->iy >= nYHigh)
			if (v2->iy >= nYHigh)
				return -2;

///
/*
	{
	long x1 = v1->ix - v0->ix;
	long x2 = v2->ix - v0->ix;
	long y1 = v1->iy - v0->iy;
	long y2 = v2->iy - v0->iy;

	if (x1*y2 - x2*y1 <= 0)
		return -4;
	}
*/
///

	if (v0->iy > v1->iy)
		swap (GFX_3DVERTEX*,v0, v1);

	if (v0->iy > v2->iy)
		swap (GFX_3DVERTEX*,v0, v2);

	if (v1->iy > v2->iy)
		swap (GFX_3DVERTEX*,v1, v2);

	if (v0->iy == v2->iy)
		return -3;

	long current_x, current_y;

	long start_y = bound (v0->iy, nYLow, nYHigh);
	long stop_y  = bound (v1->iy, nYLow, nYHigh);

/*
  long nTexMask = 0;
  switch (nTexWidth)
  {
  case 64:  nTexMask = 0x0000003F; break;
  case 128: nTexMask = 0x0000007F; break;
  case 256: nTexMask = 0x000000FF; break;
  case 512: nTexMask = 0x000001FF; break;
  };
*/

	unsigned char* pWorkOutput = pOutput + start_y * nStride;
	long* pWorkZBuffer = 0;
	if (pZBuffer)
		pWorkZBuffer = pZBuffer + start_y * nWidth;
	for (current_y = start_y; current_y < stop_y; current_y++)
	{
		long unlimited_start_x = v0->ix + (v1->ix - v0->ix) * (current_y - v0->iy) / (v1->iy - v0->iy);
		long unlimited_stop_x  = v0->ix + (v2->ix - v0->ix) * (current_y - v0->iy) / (v2->iy - v0->iy);
		long unlimited_delta_x  = unlimited_stop_x - unlimited_start_x;

		if (unlimited_delta_x != 0)
		{
			long start_u = (FIXED_DENOM * v0->iu + FIXED_DENOM * (v1->iu - v0->iu) * (current_y - v0->iy) / (v1->iy - v0->iy));
			long stop_u  = (FIXED_DENOM * v0->iu + FIXED_DENOM * (v2->iu - v0->iu) * (current_y - v0->iy) / (v2->iy - v0->iy));
			long delta_u = stop_u - start_u;
			long step_u = delta_u / unlimited_delta_x;

			long start_v = (FIXED_DENOM * v0->iv + FIXED_DENOM * (v1->iv - v0->iv) * (current_y - v0->iy) / (v1->iy - v0->iy));
			long stop_v  = (FIXED_DENOM * v0->iv + FIXED_DENOM * (v2->iv - v0->iv) * (current_y - v0->iy) / (v2->iy - v0->iy));
			long delta_v = stop_v - start_v;
			long step_v = delta_v / unlimited_delta_x;

			long start_z = (FIXED_DENOM * v0->iz + FIXED_DENOM * (v1->iz - v0->iz) * (current_y - v0->iy) / (v1->iy - v0->iy));
			long stop_z  = (FIXED_DENOM * v0->iz + FIXED_DENOM * (v2->iz - v0->iz) * (current_y - v0->iy) / (v2->iy - v0->iy));
			long delta_z = stop_z - start_z;

			long start_x = bound (unlimited_start_x, nXLow, nXHigh);
			long stop_x  = bound (unlimited_stop_x , nXLow, nXHigh);

			if (start_x > stop_x)
				swap (long, start_x, stop_x);

			long current_u = start_u + delta_u*(start_x - unlimited_start_x) / unlimited_delta_x;
			long current_v = start_v + delta_v*(start_x - unlimited_start_x) / unlimited_delta_x;

			unsigned char* pWorkOutput2 = pWorkOutput + start_x*4;
			if (pWorkZBuffer)
			{
				long* ppWorkZBuffer2 = pWorkZBuffer;
				long lCurrentZ = start_z + delta_z*(start_x - unlimited_start_x) / unlimited_delta_x;
				long lStepZ = delta_z / unlimited_delta_x;
				for (current_x = start_x; current_x < stop_x; current_x++)
				{
					if (lCurrentZ < ppWorkZBuffer2[current_x])
					{
						ppWorkZBuffer2[current_x] = lCurrentZ;
						long tu = (current_u >> FIXED_SHIFT);
						long tv = (current_v >> FIXED_SHIFT);
						long nOffset = tu*4 + tv*nTexStride;
						*((unsigned long*)pWorkOutput2) = *((unsigned long*)(pTexture+nOffset));
					}
					lCurrentZ += lStepZ;
					current_u += step_u;
					current_v += step_v;
					pWorkOutput2 += 4;
				}
			}
			else
			{
				for (current_x = start_x; current_x < stop_x; current_x++)
				{
					long tu = (current_u >> FIXED_SHIFT);
					long tv = (current_v >> FIXED_SHIFT);
					long nOffset = tu*4 + tv*nTexStride;
					*((unsigned long*)pWorkOutput2) = *((unsigned long*)(pTexture+nOffset));
					current_u += step_u;
					current_v += step_v;
					pWorkOutput2 += 4;
				}
			}
		}

		pWorkOutput += nStride;

		if (pWorkZBuffer)
			pWorkZBuffer += nWidth;
	}

	start_y = bound (v1->iy, nYLow, nYHigh);
	stop_y  = bound (v2->iy, nYLow, nYHigh);
	pWorkOutput = pOutput + start_y * nStride;
	pWorkZBuffer = 0;
	if (pZBuffer)
		pWorkZBuffer = pZBuffer + start_y * nWidth;
	for (current_y = start_y; current_y < stop_y; current_y++)
	{
		long unlimited_start_x = v1->ix + (v2->ix - v1->ix) * (current_y - v1->iy) / (v2->iy - v1->iy);
		long unlimited_stop_x  = v0->ix + (v2->ix - v0->ix) * (current_y - v0->iy) / (v2->iy - v0->iy);
		long unlimited_delta_x  = unlimited_stop_x - unlimited_start_x;

		if (unlimited_delta_x != 0)
		{
			long start_u = (FIXED_DENOM * v1->iu + FIXED_DENOM * (v2->iu - v1->iu) * (current_y - v1->iy) / (v2->iy - v1->iy));
			long stop_u  = (FIXED_DENOM * v0->iu + FIXED_DENOM * (v2->iu - v0->iu) * (current_y - v0->iy) / (v2->iy - v0->iy));
			long delta_u = stop_u - start_u;
			long step_u = delta_u / unlimited_delta_x;

			long start_v = (FIXED_DENOM * v1->iv + FIXED_DENOM * (v2->iv - v1->iv) * (current_y - v1->iy) / (v2->iy - v1->iy));
			long stop_v  = (FIXED_DENOM * v0->iv + FIXED_DENOM * (v2->iv - v0->iv) * (current_y - v0->iy) / (v2->iy - v0->iy));
			long delta_v = stop_v - start_v;
			long step_v = delta_v / unlimited_delta_x;

			long start_z = (FIXED_DENOM * v1->iz + FIXED_DENOM * (v2->iz - v1->iz) * (current_y - v1->iy) / (v2->iy - v1->iy));
			long stop_z  = (FIXED_DENOM * v0->iz + FIXED_DENOM * (v2->iz - v0->iz) * (current_y - v0->iy) / (v2->iy - v0->iy));
			long delta_z = stop_z - start_z;

			long start_x = bound (unlimited_start_x, nXLow, nXHigh);
			long stop_x  = bound (unlimited_stop_x , nXLow, nXHigh);

			if (start_x > stop_x)
				swap (long, start_x, stop_x);

			long current_u = start_u + delta_u*(start_x - unlimited_start_x) / unlimited_delta_x;
			long current_v = start_v + delta_v*(start_x - unlimited_start_x) / unlimited_delta_x;

			unsigned char* pWorkOutput2 = pWorkOutput + start_x*4;
			if (pWorkZBuffer)
			{
				long* ppWorkZBuffer2 = pWorkZBuffer;
				long lCurrentZ = start_z + delta_z*(start_x - unlimited_start_x) / unlimited_delta_x;
				long lStepZ = delta_z / unlimited_delta_x;
				for (current_x = start_x; current_x < stop_x; current_x++)
				{
					if (lCurrentZ < ppWorkZBuffer2[current_x])
					{
						ppWorkZBuffer2[current_x] = lCurrentZ;
						long tu = (current_u >> FIXED_SHIFT);
						long tv = (current_v >> FIXED_SHIFT);
						long nOffset = tu*4 + tv*nTexStride;
						*((unsigned long*)pWorkOutput2) = *((unsigned long*)(pTexture+nOffset));
					}
					lCurrentZ += lStepZ;
					current_u += step_u;
					current_v += step_v;
					pWorkOutput2 += 4;
				}
			}
			else
			{
				for (current_x = start_x; current_x < stop_x; current_x++)
				{
					long tu = (current_u >> FIXED_SHIFT);
					long tv = (current_v >> FIXED_SHIFT);
					long nOffset = tu*4 + tv*nTexStride;
					*((unsigned long*)pWorkOutput2) = *((unsigned long*)(pTexture+nOffset));
					current_u += step_u;
					current_v += step_v;
					pWorkOutput2 += 4;
				}
			}
		}

		pWorkOutput += nStride;

		if (pWorkZBuffer)
			pWorkZBuffer += nWidth;
	}

	return 0;
}

__IRAM_CODE void AlphaHorFill (
	long dx,
	unsigned long* pRow,
	unsigned long* pSrc,
	long xstep)
{
	long xval = 0;
	long xxval = 0;
	unsigned long left = pSrc[xxval];
	unsigned long right = pSrc[xxval+1];
	do
	{
		unsigned long xmask = xval & FIXED_MASK;
		unsigned long invxmask = FIXED_MASK - xmask;

		if (xmask == 0)
			*pRow ++ = left;
		else
			*pRow ++ =
				(( invxmask * (left&0x000000FF) + xmask * (right&0x000000FF) +  FIXED_HALF)      >> FIXED_SHIFT) |
				(((invxmask * (left&0x0000FF00) + xmask * (right&0x0000FF00) + (FIXED_HALF<<8))  >> FIXED_SHIFT) & 0x0000FF00) |
				(((invxmask * (left&0x00FF0000) + xmask * (right&0x00FF0000) + (FIXED_HALF<<16)) >> FIXED_SHIFT) & 0x00FF0000);

		xval += xstep;
		if ((xval>>FIXED_SHIFT) > xxval)
		{
			xxval ++;
			left = right;
			right = pSrc[xxval+1];
		}
	}
	while (--dx);
}

__IRAM_CODE int AlphaBilinearInterpolatedStretch (
	GFX_CONTEXT* pDst,
	GFX_CONTEXT* pSrc,
	unsigned long* pBuffer1,
	unsigned long* pBuffer2)
{
	long src_width  = pSrc->w << FIXED_SHIFT;
	long src_height = pSrc->h << FIXED_SHIFT;

	long xstep = src_width / pDst->w;
	long ystep = src_height / pDst->h;

	unsigned long* sptr = (unsigned long*)pSrc->pixels;

	unsigned long* pPrevRow = pBuffer1;
	unsigned long* pNextRow = pBuffer2;

	long src_stride = pSrc->delta >> 2;
	long dst_padding = (pDst->delta >> 2) - pDst->w;

	AlphaHorFill (pDst->w, pPrevRow, sptr, xstep);

	sptr += src_stride;
	AlphaHorFill (pDst->w, pNextRow, sptr, xstep);

	unsigned long* dptr = (unsigned long*)pDst->pixels;
	long yval = 0;
	long yyval = 0;
	long dy = pDst->h;
	do
	{
		long dx = pDst->w;
		unsigned long* pPrev = pPrevRow;
		unsigned long ymask = yval & FIXED_MASK;
		if (ymask == 0)
			do
			{
				*dptr++ = *pPrev++;
			}
			while (--dx);
		else
		{
			unsigned long* pNext = pNextRow;
			unsigned long invymask = FIXED_MASK - ymask;
			do
			{
				unsigned long btm = *pPrev++;
				unsigned long top = *pNext++;
				*dptr++ =
					(( invymask * (btm&0x000000FF) + ymask * (top&0x000000FF)) >> FIXED_SHIFT) |
					(((invymask * (btm&0x0000FF00) + ymask * (top&0x0000FF00)) >> FIXED_SHIFT) & 0x0000FF00) |
					(((invymask * (btm&0x00FF0000) + ymask * (top&0x00FF0000)) >> FIXED_SHIFT) & 0x00FF0000);
			}
			while (--dx);
		}

		yval += ystep;
		if ((yval>>FIXED_SHIFT) > yyval)
		{
			yyval ++;

			unsigned long* pTmp = pNextRow;
			pNextRow = pPrevRow;
			pPrevRow = pTmp;

			sptr += src_stride;
			AlphaHorFill (pDst->w, pNextRow, sptr, xstep);
		}

		dptr += dst_padding;
	}
	while (--dy);

	return 0;
}
