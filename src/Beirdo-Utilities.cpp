#include <Arduino.h>
#include <ArduinoLog.h>
#include <Beirdo-Utilities.h>

#ifndef DISABLE_LOGGING

#ifdef USE_MUTEX
mutex_t log_mutex;
#endif

HardwareSerial *logSerial;

void sendCoreNum(Print *output, int level);
void sendCRLF(Print *output, int level);

void setup_logging(int level, HardwareSerial *serial)
{
#ifdef USE_MUTEX
  mutex_init(&log_mutex);
#endif

  logSerial = serial;
  if (!logSerial) {
    return;
  }
  
  Log.begin(level, logSerial);
  Log.setPrefix(sendCoreNum);
  Log.setSuffix(sendCRLF);

}

void sendCoreNum(Print *output, int level)
{
  if (!logSerial) {
    return;
  }

  if (level > LOG_LEVEL_VERBOSE) {
    return;
  }

#ifdef USE_MUTEX
  mutex_enter_blocking(&log_mutex);
#endif

#ifdef ARDUINO_ARCH_RP2040
  int coreNum = get_core_num();
#else
  int coreNum = 0;
#endif

  output->printf("C%d: %10d: ", coreNum, millis());
}

void sendCRLF(Print *output, int level)
{
  if (!logSerial) {
    return;
  }

  if (level > LOG_LEVEL_VERBOSE) {
    return;
  }

  output->printf("\n\r");
  output->flush();

#ifdef USE_MUTEX
  mutex_exit(&log_mutex);
#endif
}

void hexdump(const void* mem, uint32_t len, uint8_t cols) 
{
  const char* src = (const char*)mem;
  static char line[128];
  char *ch = line;
  int written;  

  memset(ch, 0x00, 128);
      
  written = sprintf(ch, "[HEXDUMP] Address: %08X len: %04X (%d)", src, len, len);
  Log.notice("%s", line);

  while (len > 0) {
    ch = line;
    memset(ch, 0x00, 128);

    uint32_t linesize = cols > len ? len : cols;
    written = sprintf(ch, "[%08X] 0x%04X: ", src, (int)(src - (const char*)mem));
    ch += written;
    for (uint32_t i = 0; i < linesize; i++) {
        written = sprintf(ch, "%02X ", *(src + i));
        ch += written;
    }
    written = sprintf(ch, "  ");
    ch += written;

    for (uint32_t i = linesize; i < cols; i++) {
        written = sprintf(ch, "   ");
        ch += written;
    }

    for (uint32_t i = 0; i < linesize; i++) {
        unsigned char c = *(src + i);
        *(ch++) = isprint(c) ? c : '.';
    }

    src += linesize;
    len -= linesize;
    Log.info("%s", line);
  }
}

#endif


