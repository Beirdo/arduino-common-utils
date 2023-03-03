#ifndef __linbus_interface_h_
#define __linbus_interface_h_

#include <Arduino.h>
#include <LINBus_stack.h>
#include <linbus_registers.h>

extern LINBus_stack linbus;
extern uint8_t linbus_address;
extern uint8_t registerIndex;
extern LINBusRegister registers[];
extern int regLocation;
extern int maxRegisters;

void init_linbus(int sleepPin, int wakePin, int ledPin);
void update_linbus(void);
void process_linbus(void);
void dispatch_linbus(uint8_t index, uint8_t value);

#endif
