#include <Arduino.h>
#include <canbus_ids.h>


const char *get_canbus_id_name(int id)
{
  canbus_id_t canbus_id = static_cast<canbus_id_t>(id);
  const char *name = CANBUS_ID_NAME(canbus_id);
  if (name) {
    return name + 10;
  }
  return 0;
}


