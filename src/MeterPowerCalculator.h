#pragma once
#include "OpenKNX.h"

typedef std::function<void(uint32_t power, uint32_t duration)> MeterPowerCalculatorCallback;

class MeterPowerCalculator
{
  private:
    uint16_t _reference = 0;
    uint16_t _pulses = 0;
    uint32_t _time = 0;
    uint32_t _power = 0;
    uint32_t _lastPulseTime = 0;
    uint32_t _test = 0;
    bool wait = true;

    MeterPowerCalculatorCallback _callback;

    void calculate();

  public:
    MeterPowerCalculator(uint16_t reference) : _reference(reference){};
    void pulse();
    void loop();
    void setCallback(MeterPowerCalculatorCallback callback)
    {
        _callback = callback;
    }
};