#ifndef __COLORDEFS_H__
#define __COLORDEFS_H__
#include <inttypes.h>

//#define CRGB(r,g,b) ( ( (r) & 0x7 ) + ( ((g)<<3) & (0x7<<3) ) + ( ((b)<<6) & (0x3<<6) ) )
static inline uint32_t CRGB(uint8_t r,uint8_t g,uint8_t b)
{
    return ( ( ((uint32_t)r)<<16 ) + ( ((uint32_t)g)<<8 ) + ((uint32_t)b)  );

}
#endif
