#ifndef __sensor_h_
#define __sensor_h_

#include <Arduino.h>
#include <stdlib.h>

#define SENSOR_READING_COUNT  8


template <typename T>
class Sensor {
  public:
    Sensor(int id, T unused_val, T feedback_thresh, bool internal = false) :
      _id(id), _internal(internal), _unused(unused_val), _feedback_thresh(feedback_thresh), _value(unused_val),
      _prev_value(unused_val), _last_updated(0) {};

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

    bool check_threshold(void) 
    {
      if (_value == _unused) {
        return false;
      }

      bool retval = (_prev_value == _unused || abs((int)_prev_value - (int)_value) > _feedback_threshold);
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
class LocalSensor : public Sensor {
  public:
    LocalSensor(int id, T unused_val, T feedback_thresh, int bits, int mult, int div_) :
      Sensor(id, unused_val, feedback_thresh, true), _bits(bits), _mult(mult), _div(div_), _tail(0) {};

    void add_value(T value)
    {
      if (value == _unused) {
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

      for (int i = 0; i < SENSOR_READING_COUNT; i++) {
        value = _readings[i];
        if (value != _unused && value < min_reading) {
          min_reading = value;
          min_index = i;
        }

        if (value != _unused && value > max_reading) {
          max_reading = value;
          max_index = i;
        }
      }

      for (int i = 0; i < SENSOR_READING_COUNT; i++) {
        value = _readings[i];
        if (value == _unused || i == min_index || i == max_index) {
          continue;
        }

        accumulator += value;
        count++;
      }

      if (!count) {
        return _unused;
      }

      return (T)(accumulator / count);
    };

    void update(void)
    {
      if (!_connected) {
        return;
      }

      T raw_value = get_raw_value();
      if (raw_value != _unused) {
        T scaled_value = convert(raw_value);
        add_value(scaled_value);
      }

      set_value(filter());
    };

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

    virtual T get_raw_value(void) = 0;

  protected:
    int _bits;
    int _mult;
    int _div;
    T _readings[SENSOR_READING_COUNT];
    int _tail;0
    bool _connected;

    void _do_feedback(void);
};


template <typename T>
class RemoteSensor : public Sensor {
  public:
    RemoteSensor(int id, T unused_val, T feedback_thresh) : Sensor(id, unused_val, feedback_thresh, false) {};

    void request(void);
    T convert_from_packet(uint8_t *buf, int len)
    {
      T value;
 
      len = min(len, sizeof(value));
      if (len < sizeof(value)) {
        return _unused;
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
