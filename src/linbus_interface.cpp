#ifdef INCLUDE_LINBUS

#include <Arduino.h>
#include <LINBus_stack.h>
#include <EEPROM.h>
#include <linbus_map.h>
#include <linbus_registers.h>
#include <linbus_interface.h>
#include <Beirdo-Utilities.h>

uint8_t registerIndex = 0xFF;
uint8_t linbus_address;
uint8_t linbus_buf[2];
uint8_t linbus_buf_len = 2;

LINBus_stack linbus(Serial, 19200);

void init_linbus(int sleepPin, int wakePin, int ledPin)
{
  linbus_address = EEPROM[0];
  if (linbus_address == 0xFF) {
    bool led = false;
    while(1) {
      led = !led;
      digitalWrite(ledPin, led);
      delay(100);
    }
  }

  uint8_t location = EEPROM[1];
  registers[regLocation].write(location);

  linbus_address &= 0x1F;

  pinMode(sleepPin, OUTPUT);
  digitalWrite(sleepPin, LOW);

  pinMode(wakePin, OUTPUT);
  digitalWrite(wakePin, LOW);

  linbus.begin(wakePin, sleepPin, linbus_address);
}

void process_linbus(void)
{
  size_t read_;

  if (!linbus.waitBreak(50)) {
    return;    
  }

  if (linbus.read(linbus_buf, linbus_buf_len, &read_)) {
    if (read_) { 
      uint8_t addr = linbus_buf[0];
      uint8_t data = linbus_buf[1];

      if (addr & 0x80) {
        registerIndex = addr & 0x7F;
      } else if (addr < maxRegisters) {
        // this was a packet written to us
        registers[addr].write(data);

        if (addr == regLocation) {
          EEPROM.update(1, data);
        } else {
          dispatch_linbus(addr, data);
        }
      }
    } else {
      registerIndex = clamp<int>(registerIndex, 0, maxRegisters);
      linbus_buf[0] = registerIndex < maxRegisters ? registers[registerIndex++].read() : 0x00;

      registerIndex = clamp<int>(registerIndex, 0, maxRegisters);
      linbus_buf[1] = registerIndex < maxRegisters ? registers[registerIndex++].read() : 0x00;

      linbus.writeResponse(linbus_buf, 2);
    } 
  } else {
    linbus.sleep(STATE_SLEEP);
  }  
}

#endif
