#ifndef __sensor_h_
#define __sensor_h_

#include <Arduino.h>
#include <stdlib.h>
#include <Wire.h>
#include <Beirdo-Utilities.h>
#include <canbus_ids.h>
#ifndef DISABLE_LOGGING
#include <ArduinoLog.h>
#endif

#define SENSOR_READING_COUNT  8
#define UNUSED_VALUE  (int32_t)(0x80000000)


class Sensor {
  public:
    Sensor(int id, int data_bytes, int32_t feedback_thresh, bool internal = false) :
      _id(id), _internal(internal), _data_bytes(data_bytes), _feedback_thresh(feedback_thresh), _value(UNUSED_VALUE),
      _prev_value(UNUSED_VALUE), _last_updated(0), _valid(false), _name(get_canbus_id_name(id)) {};

    void init(void) { _valid = true; };
    void update(void) {};

    void set_value(int32_t value) 
    { 
      _value = value; 
      _last_updated = millis();
      feedback();
    };

    int32_t get_value(void) { return _value; };
    int last_updated(void)  { return _last_updated; };

  protected:
    int _id;
    bool _internal;
    int _data_bytes;
    int32_t _feedback_thresh;
    int32_t _value;
    int32_t _prev_value;
    int _last_updated;
    bool _valid;
    const char *_name;

    bool check_threshold(void) 
    {
      if (_value == UNUSED_VALUE) {
        return false;
      }

      bool retval = (_prev_value == UNUSED_VALUE || abs((int)_prev_value - (int)_value) > _feedback_thresh);
      _prev_value = _value;
      return retval;
    }

    void feedback(void) {
      if (check_threshold()) {
        _do_feedback();
      }
    }

    virtual void _do_feedback(void) = 0;
};


class LocalSensor : public Sensor {
  public:
    LocalSensor(int id, int data_bytes, int32_t feedback_thresh, int bits, uint8_t i2c_address = 0x00, int mult = 0, int div_ = 0) :
      Sensor(id, data_bytes, feedback_thresh, true), 
      _bits(bits), _mult(mult), _div(div_), _tail(0), _i2c_address(i2c_address)
    {
      for (int i = 0; i < SENSOR_READING_COUNT; i++) {
        _readings[i] = UNUSED_VALUE;
      }

      if (_i2c_address == 0xFF) {
        _connected = false;
      } else if (_i2c_address == 0x00) {
        _connected = true;
      } else {
        _connected = i2c_is_connected();
      }

#ifndef DISABLE_LOGGING
      if (_connected) {
        Log.notice("Found sensor (%s) at I2C %X", _name, _i2c_address);
      } else {
        Log.error("No sensor (%s) at I2C %X", _name, _i2c_address);
      }
#endif
    };

    void add_value(int32_t value)
    {
      if (value == UNUSED_VALUE) {
        return;
      }

      _readings[_tail] = value;
      _tail = (_tail + 1) % SENSOR_READING_COUNT;
    };

    int32_t filter(void)
    {
      int32_t accumulator = 0;
      int count = 0;
      int32_t min_reading = (int32_t)0x7FFFFFFF;
      int min_index = -1;
      int32_t max_reading = (int32_t)0x80000000;
      int max_index = -1;
      int32_t value;

      for (int i = 0; i < SENSOR_READING_COUNT; i++) {
        value = _readings[i];
        if (value != UNUSED_VALUE && value < min_reading) {
          min_reading = value;
          min_index = i;
        }

        if (value != UNUSED_VALUE && value > max_reading) {
          max_reading = value;
          max_index = i;
        }
      }

      for (int i = 0; i < SENSOR_READING_COUNT; i++) {
        value = _readings[i];
        if (value == UNUSED_VALUE || i == min_index || i == max_index) {
          continue;
        }

        accumulator += value;
        count++;
      }

      if (!count) {
        return UNUSED_VALUE;
      }

      return (int32_t)(accumulator / count);
    };

    void update(void)
    {
      if (!_connected) {
        return;
      }

      int32_t raw_value = get_raw_value();
      if (raw_value != UNUSED_VALUE) {
        add_value(convert(raw_value));
      }

      set_value(filter());
    };

  protected:
    int _bits;
    int _mult;
    int _div;
    int32_t _readings[SENSOR_READING_COUNT];
    int _tail;
    uint8_t _i2c_address;
    bool _connected;

    virtual int32_t get_raw_value(void) = 0;
    void _do_feedback(void);

    int32_t convert(int32_t reading)
    {
      int32_t value = 0;

      if (_div != 0) {
        value = reading;
        value *= _mult;
        value /= _div;
      }

      return value;
    };


    void i2c_write_register(uint8_t regnum, uint8_t value, bool skip_byte = false)
    {
      Wire.beginTransmission(_i2c_address);
      Wire.write(regnum);
      if (!skip_byte) {
        Wire.write(value);
      }
      Wire.endTransmission();
    };

    void i2c_write_register_word(uint8_t regnum, uint16_t value)
    {
      Wire.beginTransmission(_i2c_address);
      Wire.write(regnum);
      Wire.write((value >> 8) & 0xFF);
      Wire.write(value & 0xFF);
      Wire.endTransmission();
    };

    void i2c_read_data(uint8_t regnum, uint8_t *buf, uint8_t count, bool skip_regnum = false)
    {
      Wire.beginTransmission(_i2c_address);
      if (!skip_regnum) {
        Wire.write(regnum);
        Wire.endTransmission(false);
      }
      Wire.requestFrom(_i2c_address, count);

      for (int i = 0; i < count && Wire.available(); i++) {
        *(buf++) = Wire.read();
      }
    };

    bool i2c_is_connected(void)
    {
      Wire.beginTransmission(_i2c_address);
      return (Wire.endTransmission() == 0);
    };
};


class RemoteSensor : public Sensor {
  public:
    RemoteSensor(int id, int data_bytes, int32_t feedback_thresh) : 
      Sensor(id, data_bytes, feedback_thresh, false) {};

    void request(void);
    int32_t convert_from_packet(uint8_t *buf, int len)
    {
      int32_t value = 0;
 
      len = min(len, _data_bytes);
      if (len < _data_bytes || _data_bytes > 4) {
        return UNUSED_VALUE;
      }

      memcpy((uint8_t *)&value, buf, len);

      // network data is big-endian
      if (isLittleEndian()) {
        value = __bswap32(value);
      }

      return value;
    };

    int convert_to_packet(int32_t value, uint8_t *buf, int len)
    {
      len = min(len, _data_bytes);
      if (len < _data_bytes || _data_bytes > 4) {
        return 0;
      }

      // network data is big-endian
      if (isLittleEndian()) {
        value = __bswap32(value);
      }

      memcpy(buf, (uint8_t *)&value, len);
      return len;
    }

  protected:
    void _do_feedback(void);
};

#endif
