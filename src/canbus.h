#ifndef __canbus_h_
#define __canbus_h_

#include <Arduino.h>
#include <SPI.h>
#include <canbus_ids.h>

class CANBus {
  public:
    CANBus() { 
      _initialized = false;
      _spi = 0;
    };

    bool begin(SPIClass *spi, int ss, int interrupt);
    int write(int id, const char *buf, int len);
    int read(int *id, const char *buf, int len);
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
void canbus_send(int id, uint8_t *buf, int len);

void canbus_output_value(int id, int32_t value, int data_bytes);
void canbus_request_value(int id);


#endif
