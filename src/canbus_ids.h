#ifndef __canbus_ids_h_
#define __canbus_ids_h_

#include <Beirdo-Utilities.h>

// CANBus regular IDs are 11 bits, extended are 29 bits
// we will use 11 bits and use the MSB as a "write" modifier to send
// config or commands to the remote sensor while leaving the normal 
// IDs for the sensors to broadcast reading updates.

typedef enum {
  CANBUS_ID_MAINBOARD = 1,
  CANBUS_ID_WBUS,
  CANBUS_ID_LINBUS_BRIDGE,

  // Onboard Sensors
  CANBUS_ID_INTERNAL_TEMP = 128,
  CANBUS_ID_FLAME_DETECTOR,
  CANBUS_ID_VSYS_VOLTAGE,

  // Sensors formerly on I2C
  CANBUS_ID_EXTERNAL_TEMP = 256,
  CANBUS_ID_BATTERY_VOLTAGE,
  CANBUS_ID_COOLANT_TEMP_WEBASTO,
  CANBUS_ID_EXHAUST_TEMP,
  CANBUS_ID_IGNITION_SENSE,
  CANBUS_ID_EMERGENCY_STOP,
  CANBUS_ID_START_RUN,

  // Sensors on LINBus
  CANBUS_ID_VEHICLE_FAN_PERCENT = 512,
  CANBUS_ID_VEHICLE_FAN_SPEED,
  CANBUS_ID_VEHICLE_FAN_INT_TEMP,
  CANBUS_ID_VEHICLE_FAN_EXT_TEMP,
  CANBUS_ID_RADIATOR_FAN_PERCENT,
  CANBUS_ID_RADIATOR_FAN_SPEED,
  CANBUS_ID_RADIATOR_FAN_INT_TEMP,
  CANBUS_ID_RADIATOR_FAN_EXT_TEMP,
  CANBUS_ID_PELTIER_FAN_PERCENT,
  CANBUS_ID_PELTIER_FAN_SPEED,
  CANBUS_ID_PELTIER_FAN_INT_TEMP,
  CANBUS_ID_PELTIER_FAN_EXT_TEMP,
  CANBUS_ID_VALVE_BYPASS_ENGINE_CONTROL,
  CANBUS_ID_VALVE_BYPASS_ENGINE_CURRENT,
  CANBUS_ID_VALVE_HEAT_EXCHANGE_CONTROL,
  CANBUS_ID_VALVE_HEAT_EXCHANGE_CURRENT,
  CANBUS_ID_VALVE_BYPASS_HEATER_CONTROL,
  CANBUS_ID_VALVE_BYPASS_HEATER_CURRENT,
  CANBUS_ID_VALVE_BYPASS_RADIATOR_CONTROL,
  CANBUS_ID_VALVE_BYPASS_RADIATOR_CURRENT,
} canbus_id_t;

#define CANBUS_ID_WRITE_MODIFIER  BIT(10)
#define CANBUS_ID_LINBUS_BASE     512
#define CANBUS_ID_LINBUS_MASK     0x01FF

#define NAME_HELPER(x)    #x
#define CANBUS_ID_NAME(x) NAME_HELPER(x)

const char *get_canbus_id_name(int id);

#endif
