#ifndef __canbus_stm32_h_
#define __canbus_stm32_h_

#include <Arduino.h>

#ifdef USE_STM32_CAN

#include <STM32-CAN.h>
#include "canbus.h"

class CANBusController_STM32 : public CANBusController {
  public:
    CANBusController_STM32(int enable, void *filters, int filter_count) :
      CANBusController(enable), _filters(static_cast<CAN_filter_t *>(filters)), _filter_count(filter_count) {};

    bool begin(void);
    int write(int id, const char *buf, int len, uint8_t type = CAN_DATA);
    int read(int *id, const char *buf, int len, uint8_t *type);
    bool available(void);

  protected:
    STM32_CAN _can;
    CAN_filter_t *_filters;
    int _filter_count;
};

void init_canbus_stm32_internal(int enable, CAN_filter_t *filters, int filter_count);

#endif

#endif

