#ifndef __canbus_h_
#define __canbus_h_

#ifndef SKIP_CANBUS

#include <Beirdo-Utilities.h>
#include <Arduino.h>

#ifdef ARDUINO_ARCH_RP2040
#include <SPI.h>
#else
class SPIClass;
#endif

#include <canbus_ids.h>

typedef enum {
  CAN_REMOTE,
  CAN_DATA,
  CANFD_NO_BIT_RATE_SWITCH,
  CANFD_WITH_BIT_RATE_SWITCH
} canbus_packet_type_t;



class CANBusController {
  public:
    CANBusController(int enable = -1) : _enable(enable), _initialized(false) {};
    bool begin(void) { return false; };
    int write(int id, const char *buf, int len, uint8_t type) { return -1; };
    int read(int *id, const char *buf, int len, uint8_t *type) { return -1; };
    bool available(void) { return false; };

  protected:
    int _enable;
    bool _initialized;
};


class CANBus {
  public:
    CANBus() : _controller(0), _initialized(false) {};

    bool begin(SPIClass *spi, int ss, int interrupt, int enable = -1, void *filters = 0, int filter_count = 0);
    int write(int id, const char *buf, int len, uint8_t type = CANFD_WITH_BIT_RATE_SWITCH);
    int read(int *id, const char *buf, int len, uint8_t *type);
    bool available(void);

  protected:
    CANBusController *_controller = 0;
    bool _initialized;
};

#define CANBUS_BUF_SIZE   64


void update_canbus_rx(void);
void update_canbus_tx(void);
void canbus_send(int id, uint8_t *buf, int len, uint8_t type = CANFD_WITH_BIT_RATE_SWITCH);

void canbus_output_value(int id, int32_t value, int data_bytes);
void canbus_request_value(int id);


extern CANBus canbus;

#endif

#endif
