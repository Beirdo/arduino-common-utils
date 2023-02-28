#include <Arduino.h>
#include <ArduinoLog.h>
#include <stdlib.h>
#include <cppQueue.h>
#include <Beirdo-Utilities.h>
#include <canbus_dispatch.h>

#include "canbus.h"
#include "canbus_mcp2517fd.h"
#include "canbus_stm32.h"

CANBus canbus;

#define CANBUS_BUF_COUNT  16

typedef struct {
  int id;
  uint8_t buf[CANBUS_BUF_SIZE];
  int len;
  uint8_t type;
} canbus_buf_t;

canbus_buf_t canbus_tx_bufs[CANBUS_BUF_COUNT];
uint8_t canbus_rx_buf[CANBUS_BUF_SIZE];

#ifdef USE_MUTEX
mutex_t canbus_mutex;
#endif

int canbus_head = 0;
int canbus_tail = 0;

cppQueue canbus_tx_q(sizeof(int), CANBUS_BUF_COUNT, FIFO);


bool CANBus::begin(SPIClass *spi, int ss, int interrupt, int enable)
{
  _initialized = false;
  _controller = 0;

#ifdef RASPBERRY_PI_PICO
  if (spi) {
    _controller = new CANBusController_MCP2517FD(spi, ss, interrupt);
  }
#endif

#ifdef STM32F0xx
  _controller = new CANBusController_STM32(enable);
#endif

  if (_controller) {
    _initialized = _controller->begin();
  }

  return _initialized;
}


int CANBus::write(int id, const char *buf, int len, uint8_t type)
{
  if (!_initialized) {
    return 0;    
  }

  return _controller->write(id, buf, len, type);
}

int CANBus::read(int *id, const char *buf, int len, uint8_t *type)
{
  if (!_initialized) {
    return 0;
  }

  return _controller->read(id, buf, len, type);
}

bool CANBus::available(void)
{
  if (!_initialized) {
    return false;
  }

  return _controller->available();
}



void update_canbus_tx(void) 
{
  while (!canbus_tx_q.isEmpty()) {
#ifdef USE_MUTEX
    CoreMutex m(&canbus_mutex);
#endif

    int index;
    canbus_tx_q.pop(&index);

    canbus_head = (index + 1) % CANBUS_BUF_COUNT;

    canbus_buf_t *item = &canbus_tx_bufs[index];
    const uint8_t *buf = (const uint8_t *)item->buf;
    int len = item->len;
    int id = item->id;
    uint8_t type = item->type;

    int retlen = canbus.write(id, (const char *)buf, len, type);
    Log.notice("CANBus ID %X, type %d: Sent %d bytes of %d", id, type, retlen, len);

#ifdef HEXDUMP_TX
    hexdump(buf, retlen, 16);
#endif    
  }
}

void update_canbus_rx(void)
{
  while (canbus.available()) {
    int id;
    uint8_t type;
    int len = canbus.read(&id, (const char *)canbus_rx_buf, CANBUS_BUF_SIZE, &type);

    if (!len) {
      continue;
    }

    Log.notice("CANBus ID %X, type %d: Received %d bytes", id, type, len);

#ifdef HEXDUMP_TX
    hexdump(canbus_rx_buf, len, 16);
#endif

    canbus_dispatch(id, canbus_rx_buf, len, type);
  }
}

void canbus_send(int id, uint8_t *buf, int len, uint8_t type)
{
#ifdef USE_MUTEX
  CoreMutex m(&canbus_mutex);
#endif
  
  if ((canbus_tail + 1) % CANBUS_BUF_COUNT == canbus_head) {
    // We are out of space.  Discard the oldest
    canbus_head = (canbus_head + 1) % CANBUS_BUF_COUNT;
  }
  
  int index = canbus_tail;
  canbus_tail = (canbus_tail + 1) % CANBUS_BUF_COUNT;

  canbus_buf_t *item = &canbus_tx_bufs[index];
  item->id = id;
  item->len = len;
  item->type = type;
  memcpy(item->buf, buf, len);

  canbus_tx_q.push(&index);
}

void canbus_output_value(int id, int32_t value, int data_bytes)
{
  int len = clamp<int>(data_bytes, 0, 4);
  uint8_t buf[4];

  memcpy(buf, &value, len);
  canbus_send(id, buf, len);
}

void canbus_request_value(int id)
{
  canbus_send(id, 0, 0, CAN_REMOTE);
}
