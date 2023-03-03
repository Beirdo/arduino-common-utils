#include <Arduino.h>
#include <Beirdo-Utilities.h>

#include "linbus_registers.h"


void LINBusRegister::write(uint8_t value, bool raw)
{
  if (raw) {
    _value = value;
  } else {
    _value = (_value & ~(_write_mask)) | (value & _write_mask);
  }
}

uint8_t LINBusRegister::read(void)
{
  uint8_t value;
  value = _value;

  return value;
}

void LINBusRegister::changeBit(uint8_t bitNum, bool newValue, bool raw)
{
  uint8_t bit_mask = BIT(bitNum);
  if (raw || (_write_mask & bit_mask)) {
    _value &= ~bit_mask;
    if (newValue) {
      _value |= bit_mask;
    }
  }
}
