#include <Arduino.h>

#ifdef STM32F0xx

#include <ArduinoLog.h>

#include "canbus.h"
#include "canbus_stm32.h"


bool CANBusController_STM32::begin(void)
{
  Log.notice("Initialized CANBus on STM32");
  _initialized = true;

  if (_enable != -1) {
    pinMode(_enable, OUTPUT);

    // active low enable for transceiver
    digitalWrite(_enable, LOW);
  }

  return _initialized;
}

int CANBusController_STM32::write(int id, const char *buf, int len, uint8_t type)
{
  if (!_initialized) {
    return 0;    
  }

  // STM32 internal
}

int CANBusController_STM32::read(int *id, const char *buf, int len, uint8_t *type)
{
  if (!_initialized) {
    return 0;
  }

  // STM32 internal
}

bool CANBusController_STM32::available(void)
{
  if (!_initialized) {
    return false;
  }

  // STM32 internal
}


void init_canbus_stm32_internal(int enable)
{
#ifdef USE_MUTEX
  mutex_init(&canbus_mutex);
#endif
  canbus.begin(0, -1, -1, enable);  
}

#endif
