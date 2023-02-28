#ifndef __canbus_mcp2517fd_h_
#define __canbus_mcp2517fd_h_

#ifdef RASPBERRY_PI_PICO

#include <Arduino.h>
#include <ACAN2517FD.h>
#include "canbus.h"

class CANBusController_MCP2517FD : public CANBusController {
  public:
    CANBusController_MCP2517FD(SPIClass *spi, int ss, int interrupt) :
      CANBusController(-1), _spican(0), _spi(spi), _ss(ss), _interrupt(interrupt) {};

    bool begin(void);
    int write(int id, const char *buf, int len, uint8_t type = CANFD_WITH_BIT_RATE_SWITCH);
    int read(int *id, const char *buf, int len, uint8_t *type);
    bool available(void);

  protected:
    ACAN2517FD *_spican;
    SPIClass *_spi;
    int _ss;
    int _interrupt;
};

void init_canbus_mcp2517fd(SPIClass *spi, int ss, int interrupt);

#endif

#endif
