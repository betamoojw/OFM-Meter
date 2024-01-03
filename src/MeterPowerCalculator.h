#pragma once
#include "OpenKNX.h"

typedef std::function<void(uint32_t power, uint32_t duration, uint32_t pulses)> MeterPowerCalculatorCallback;

class MeterPowerCalculator
{
  private:
    float _modifier;
    uint16_t _waitTime;
    uint16_t _abortTime;
    uint16_t _reference;
    uint16_t _pulses = 0;
    uint32_t _time = 0;
    uint32_t _power = 0;
    uint32_t _lastPulseTime = 0;
    // uint32_t _test = 0;
    bool wait = true;

    MeterPowerCalculatorCallback _callback;

    void calculate();

  public:
    MeterPowerCalculator(uint16_t reference, float modifier, uint16_t waitTime, uint16_t abortTime)
        : _reference(reference), _modifier(modifier), _waitTime(waitTime), _abortTime(abortTime){};
    void pulse();
    void loop();
    void setCallback(MeterPowerCalculatorCallback callback)
    {
        _callback = callback;
    }
};