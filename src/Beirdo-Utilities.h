#ifndef __Beirdo_Utilities_h_
#define __Beirdo_Utilities_h_


#define HI_BYTE(x)    ((uint8_t)(((int)(x) >> 8) & 0xFF))
#define LO_BYTE(x)    ((uint8_t)(((int)(x) & 0xFF)))

#define HI_NIBBLE(x)  ((uint8_t)(((int)(x) >> 4) & 0x0F))
#define LO_NIBBLE(x)  ((uint8_t)(((int)(x) & 0x0F)))

#define BIT(x) ((uint32_t)(1 << x))

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
inline T map(T x, T in_min, T in_max, T out_min, T out_max)
{
  // the perfect map fonction, with constraining and float handling
  x = clamp<T>(x, in_min, in_max);
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif
