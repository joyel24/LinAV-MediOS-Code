#ifndef __datatypes__
#define __datatypes__

#define __ASSEMBLY__

#define  HOST_LITTLE_ENDIAN

#ifdef __GNUC__
#define  INLINE      static inline
#else
#undef	 INLINE
//#define  INLINE      inline
#endif

#define  ZERO_LENGTH 0

/* quell stupid compiler warnings */
#ifndef UNUSED
#define  UNUSED(x)   ((x) = (x))
#endif

typedef  signed char    int8;
typedef  signed short   int16;
typedef  signed long     int32;
typedef  unsigned char  uint8;
typedef  unsigned short uint16;
typedef  unsigned int   uint32;

typedef  unsigned long   ulong;

#define boolean bool

#define  ASSERT(expr)
#define  ASSERT_MSG(msg)

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef signed char Sint8;
typedef signed short Sint16;
typedef signed int Sint32;

//typedef Uint8 u8;

#undef byte
#undef word
typedef uint16 word;
typedef uint8 byte;
typedef signed char offset;

#endif
