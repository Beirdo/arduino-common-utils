#include <Beirdo-Utilities.h>
#include <Arduino.h>

#ifdef USE_MCP2517FD

#ifndef DISABLE_LOGGING
#include <ArduinoLog.h>
#endif

#include <ACAN2517FD.h>
#include <SPI.h>

#include "canbus.h"
#include "canbus_mcp2517fd.h"

#ifdef USE_MUTEX
extern mutex_t canbus_mutex;
#endif

ACAN2517FD *controller = 0;

void mcp2517fd_isr(void)
{
  if (controller) {
    controller->isr();
  }
}

bool CANBusController_MCP2517FD::begin(void)
{
  _spican = new ACAN2517FD(_ss, *_spi, _interrupt);
  controller = _spican;

  ACAN2517FDSettings settings (ACAN2517FDSettings::OSC_20MHz,
                               1000 * 1000, DataBitRateFactor::x8);
  const uint32_t errorCode = _spican->begin(settings, mcp2517fd_isr) ;  
  if (errorCode) {
#ifndef DISABLE_LOGGING
    Log.error("CAN Initialization error code: %X", errorCode);
#endif
    _initialized = false;
  } else {
#ifndef DISABLE_LOGGING
    Log.notice("Initialized CANBus on MCP2517");
#endif
    _initialized = true;
  }

  return _initialized;
}

int CANBusController_MCP2517FD::write(int id, const char *buf, int len, uint8_t type)
{
  if (!_initialized) {
    return -1;    
  }

  CANFDMessage msg;
  if (len > 64) {
    return -1;
  }

  memcpy(msg.data, buf, len);
  msg.type = static_cast<CANFDMessage::Type>(type);
  msg.len = len;
  msg.pad();
  msg.id = id;
  msg.ext = false;

  if (!_spican->tryToSend(msg)) {
    return -1;
  }
  return len;
}

int CANBusController_MCP2517FD::read(int *id, const char *buf, int len, uint8_t *type)
{
  if (!_initialized) {
    return -1;
  }

  CANFDMessage msg;
  if (!_spican->receive(msg)) {
    return -1;
  }

  if (len < msg.len) {
    return -1;
  }

  memcpy((char *)buf, msg.data, msg.len);

  if (id) {
    *id = msg.id;
  }

  if (type) {
    *type = msg.type;
  }

  return msg.len;
}

bool CANBusController_MCP2517FD::available(void)
{
  if (!_initialized) {
    return false;
  }

  return _spican->available();
}



void init_canbus_mcp2517fd(SPIClass *spi, int ss, int interrupt)
{
#ifdef USE_MUTEX
  mutex_init(&canbus_mutex);
#endif
  canbus.begin(spi, ss, interrupt);  
}

#endif
