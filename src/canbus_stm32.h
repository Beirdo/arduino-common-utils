#ifndef __canbus_stm32_h_
#define __canbus_stm32_h_

#include <Arduino.h>

#ifdef STM32F0xx

#include <STM32-CAN.h>
#include "canbus.h"

class CANBusController_STM32 : public CANBusController {
  public:
    CANBusController_STM32(int enable) : CANBusController(enable) {};

    bool begin(void);
    int write(int id, const char *buf, int len, uint8_t type = CAN_DATA);
    int read(int *id, const char *buf, int len, uint8_t *type);
    bool available(void);

  protected:
    STM32_CAN _can;
};

void init_canbus_stm32_internal(int enable);

#endif

#endif

