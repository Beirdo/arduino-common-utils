#ifndef __sensor_h_
#define __sensor_h_

#include <Arduino.h>
#include <stdlib.h>
#include <Wire.h>

#define SENSOR_READING_COUNT  8


template <typename T>
class Sensor {
  public:
    Sensor(int id, T unused_val, T feedback_thresh, bool internal = false) :
      _id(id), _internal(internal), _unused(unused_val), _feedback_thresh(feedback_thresh), _value(unused_val),
      _prev_value(unused_val), _last_updated(0), _valid(false) {};

    void init(void) { _valid = true; };
    void update(void) {};

    void set_value(T value) 
    { 
      _value = value; 
      _last_updated = millis();
      feedback();
    };

    T get_value(void) { return _value; };
    int last_updated(void)  { return _last_updated; };

  protected:
    int _id;
    bool _internal;
    T _unused;
    T _feedback_thresh;
    T _value;
    T _prev_value;
    int _last_updated;
    bool _valid;

    bool check_threshold(void) 
    {
      if (_value == _unused) {
        return false;
      }

      bool retval = (_prev_value == _unused || abs((int)_prev_value - (int)_value) > _feedback_thresh);
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


template <typename T>
class LocalSensor : public Sensor<T> {
  public:
    LocalSensor(int id, T unused_val, T feedback_thresh, int bits, uint8_t i2c_address = 0x00, int mult = 0, int div_ = 0) :
      Sensor<T>(id, unused_val, feedback_thresh, true), 
      _bits(bits), _mult(mult), _div(div_), _tail(0), _i2c_address(i2c_address)
    {
      for (int i = 0; i < SENSOR_READING_COUNT; i++) {
        _readings[i] = unused_val;
      }

      if (_i2c_address == 0xFF) {
        _connected = false;
      } else if (_i2c_address == 0x00) {
        _connected = true;
      } else {
        _connected = i2c_is_connected();
      }

#ifdef ENABLE_LOGGING
      if (_connected) {
        Log.notice("Found sensor (%s) at I2C %X", capabilities_names[_index], _i2c_address);
      } else {
        Log.error("No sensor (%s) at I2C %X", capabilities_names[_index], _i2c_address);
      }
#endif
    };

    void add_value(T value)
    {
      if (value == this->_unused) {
        return;
      }

      _readings[_tail] = value;
      _tail = (_tail + 1) % SENSOR_READING_COUNT;
    };

    T filter(void)
    {
      int32_t accumulator = 0;
      int count = 0;
      int32_t min_reading = (int32_t)0x7FFFFFFF;
      int min_index = -1;
      int32_t max_reading = (int32_t)0x80000000;
      int max_index = -1;
      T value;
      T unused = this->_unused;

      for (int i = 0; i < SENSOR_READING_COUNT; i++) {
        value = _readings[i];
        if (value != unused && value < min_reading) {
          min_reading = value;
          min_index = i;
        }

        if (value != unused && value > max_reading) {
          max_reading = value;
          max_index = i;
        }
      }

      for (int i = 0; i < SENSOR_READING_COUNT; i++) {
        value = _readings[i];
        if (value == unused || i == min_index || i == max_index) {
          continue;
        }

        accumulator += value;
        count++;
      }

      if (!count) {
        return unused;
      }

      return (T)(accumulator / count);
    };

    void update(void)
    {
      if (!_connected) {
        return;
      }

      T raw_value = get_raw_value();
      if (raw_value != this->_unused) {
        T scaled_value = convert(raw_value);
        add_value(scaled_value);
      }

      set_value(filter());
    };

  protected:
    int _bits;
    int _mult;
    int _div;
    T _readings[SENSOR_READING_COUNT];
    int _tail;
    uint8_t _i2c_address;
    bool _connected;

    virtual T get_raw_value(void) = 0;
    void _do_feedback(void);

    T convert(T reading)
    {
      T value = 0;

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


template <typename T>
class RemoteSensor : public Sensor<T> {
  public:
    RemoteSensor(int id, T unused_val, T feedback_thresh) : 
      Sensor<T>(id, unused_val, feedback_thresh, false) {};

    void request(void);
    T convert_from_packet(uint8_t *buf, int len)
    {
      T value;
 
      len = min(len, sizeof(value));
      if (len < sizeof(value)) {
        return this->_unused;
      }

      memcpy((char *)&value, buf, len);
      return value;
    };

    int convert_to_packet(T value, uint8_t *buf, int len)
    {
      len = min(len, sizeof(value));
      if (len < sizeof(value)) {
        return 0;
      }

      memcpy(buf, (char *)&value, len);
      return len;
    }

  protected:
    void _do_feedback(void);
};

#endif
