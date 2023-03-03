#include <Arduino.h>
#include <canbus_ids.h>


const char *get_canbus_id_name(int id)
{
#ifdef DISABLE_LOGGING
  (void)id;
#else
  canbus_id_t canbus_id = static_cast<canbus_id_t>(id);
  const char *name = CANBUS_ID_NAME(canbus_id);
  if (name) {
    return name + 10;
  }
#endif
  return 0;
}

