#ifndef _SILICON_18F26Q10
#define _SILICON_18F26Q10

#include <device_config.h>

#define FUSE_ADDRESS 0x300000
#define FUSE_COUNT 6	// words

#define REVISION_ID_ADDRESS 0x3FFFFFC
#define DEVICE_ID_ADDRESS 0x3FFFFFE

#ifdef  _XTAL_FREQ

# define __OPTIMIZE__
#	define delay_us(_v)	__delay_us(_v)
#	define delay_ms(_v)	__delay_ms(_v)

#	define XTAL_FREQ_MHZ (_XTAL_FREQ / 1000000)
#	define XTAL_FREQ_KHZ (_XTAL_FREQ / 1000)

# define CPU_uS_COUNT(_n) (XTAL_FREQ_MHZ * (_n))
// Round up 5
# define CPU_100nS_COUNT(_n) ((XTAL_FREQ_MHZ * (_n) + 5) / 10)

#else	/* _XTAL_FREQ */

#	define delay_us(_v)
#	define delay_ms(_v)
#endif

#define LFINTOSC_FREQ_KHZ (31)

#include "q10io.h"

#endif	/* _XTAL_FREQ */