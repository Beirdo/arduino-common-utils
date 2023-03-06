#include <Arduino.h>

#ifdef USE_STM32_CAN

#ifndef DISABLE_LOGGING
#include <ArduinoLog.h>
#endif
#include <STM32-CAN.h>
#include <stdlib.h>

#include "canbus.h"
#include "canbus_stm32.h"


bool CANBusController_STM32::begin(void)
{
  _can.setFilters(_filters, _filter_count);
  HardwareSerial *serial = &Serial2;
#ifdef DISABLE_LOGGING
  serial = 0;
#endif
  _can.begin(serial, CAN_1000KBPS);

#ifndef DISABLE_LOGGING
  Log.notice("Initialized CANBus on STM32");
#endif
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
  if (!_initialized || len < 0 || len > 8) {
    return 0;
  }

  CAN_msg_t msg;
  msg.id = id;
  msg.len = len;
  msg.format = STANDARD_FORMAT;
  msg.type = (type == CAN_REMOTE ? REMOTE_FRAME  : DATA_FRAME);
  memset(msg.data, 0x00, 8);
  memcpy(msg.data, buf, len);
  
  if (_can.send(&msg)) {
    return len;
  } else {
    return -1;
  }
}

int CANBusController_STM32::read(int *id, const char *buf, int len, uint8_t *type)
{
  if (!_initialized) {
    return 0;
  }

  CAN_msg_t msg;
  if (!_can.receive(&msg)) {
    return -1;
  }

  if (msg.len > len) {
    return -1;
  }

  memcpy((char *)buf, msg.data, msg.len);
  *id = msg.id;
  *type = (msg.type == REMOTE_FRAME ? CAN_REMOTE : CAN_DATA);

  return msg.len;
}

bool CANBusController_STM32::available(void)
{
  if (!_initialized) {
    return false;
  }

  return !(!_can.available());
}


void init_canbus_stm32_internal(int enable, CAN_filter_t *filters, int filter_count)
{
#ifdef USE_MUTEX
  mutex_init(&canbus_mutex);
#endif
  canbus.begin(0, -1, -1, enable, filters, filter_count);
}

#endif
