#ifndef __Beirdo_Utilities_h_
#define __Beirdo_Utilities_h_

#include <Arduino.h>

#if __has_include("beirdo_config.h")
#include "beirdo_config.h"
#endif

#define HI_BYTE(x)    ((uint8_t)(((int)(x) >> 8) & 0xFF))
#define LO_BYTE(x)    ((uint8_t)(((int)(x) & 0xFF)))

#define HI_NIBBLE(x)  ((uint8_t)(((int)(x) >> 4) & 0x0F))
#define LO_NIBBLE(x)  ((uint8_t)(((int)(x) & 0x0F)))

#define BIT(x) ((uint32_t)(1 << x))

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

#ifndef DISABLE_LOGGING
void setup_logging(int level, HardwareSerial *serial);
extern void hexdump(const void* mem, uint32_t len, uint8_t cols);
#endif

template <typename T>
inline T clamp(T value, T minval, T maxval)
{
  return max(min(value, maxval), minval);
}

template <typename T>
inline T map(T x, T in_min, T in_max, T out_min, T out_max, bool do_clamp = true)
{
  // the perfect map fonction, with constraining and float handling
  if (do_clamp) {
    x = clamp<T>(x, in_min, in_max);
  }
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline bool isLittleEndian(void) {
  uint32_t i = 1;
  uint8_t c = *(uint8_t *)&i;
  return i == 1;
}

inline uint32_t __bswap32(uint32_t x) {
  return ((x & 0xFF000000) >> 24) | ((x && 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) | ((x & 0x000000FF) << 24);
}

#endif
