#ifndef __linbus_registers_h_
#define __linbus_registers_h_

#include <Arduino.h>

class LINBusRegister {
  public:
    LINBusRegister(uint8_t write_mask, uint8_t default_value = 0x00) :
      _write_mask(write_mask), _default_value(default_value), _value(default_value) {}
    void write(uint8_t value, bool raw = false);
    uint8_t read(void);
    void changeBit(uint8_t bitNum, bool newValue, bool raw = false);
  private:
    uint8_t _write_mask;
    uint8_t _default_value;
    uint8_t _value;
};

#endif
