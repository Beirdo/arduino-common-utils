#ifndef __canbus_h_
#define __canbus_h_

#include <Arduino.h>
#include <SPI.h>
#include <canbus_ids.h>

typedef enum {
  CAN_REMOTE,
  CAN_DATA,
  CANFD_NO_BIT_RATE_SWITCH,
  CANFD_WITH_BIT_RATE_SWITCH
} canbus_packet_type_t;

class CANBus {
  public:
    CANBus() { 
      _initialized = false;
      _spi = 0;
    };

    bool begin(SPIClass *spi, int ss, int interrupt);
    int write(int id, const char *buf, int len, uint8_t type = CANFD_WITH_BIT_RATE_SWITCH);
    int read(int *id, const char *buf, int len, uint8_t *type);
    bool available(void);

  protected:
    bool _initialized;
    SPIClass *_spi;
    int _ss;
    int _interrupt;
};

#define CANBUS_BUF_SIZE   64


void init_canbus(SPIClass *spi, int ss, int interrupt);
void update_canbus_rx(void);
void update_canbus_tx(void);
void canbus_send(int id, uint8_t *buf, int len, uint8_t type = CANFD_WITH_BIT_RATE_SWITCH);

void canbus_output_value(int id, int32_t value, int data_bytes);
void canbus_request_value(int id);


#endif
