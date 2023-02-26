#ifndef __linbus_map_h_
#define __linbus_map_h_

typedef enum {
  BOARD_TYPE_VALVE_CONTROL = 0,
  BOARD_TYPE_COOLANT_TEMP,
  BOARD_TYPE_FLOW_SENSOR,
  BOARD_TYPE_FAN_CONTROL,
  BOARD_TYPE_PUMP_CONTROL,
  BOARD_TYPE_PELTIER_CONTROL,
} board_types_t;

typedef enum {
  LOC_HEATER_CORE = 0,
  LOC_VALVE_ENGINE_BYPASS,
  LOC_VALVE_HEAT_EXCHANGER,
  LOC_VALVE_HEATER_BYPASS,
  LOC_VALVE_RADIATOR_BYPASS,
  LOC_WEBASTO_INGRESS,
  LOC_WEBASTO_EGRESS,
  LOC_HEATER_EGRESS,
  LOC_EXTERIOR_LOOP_INGRESS,
  LOC_EXTERIOR_LOOP_EGRESS,
  LOC_HEAT_EXCH_INT_INGRESS,
  LOC_HEAT_EXCH_EXT_INGRESS,
  LOC_WATER_COOLER,
} physical_location_t;

#endif