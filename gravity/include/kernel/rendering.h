/* 
*   include/rendering.h
*
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __RENDERING_H
#define __RENDERING_H

#include <api.h>

static int RenderTriangle (
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
	long* pZBuffer);

static int AlphaBilinearInterpolatedStretch (
	GFX_DATA* pDst,
	GFX_DATA* pSrc,
	unsigned long* pBuffer1,
	unsigned long* pBuffer2);

#endif
