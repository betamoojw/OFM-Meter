#pragma once
#include "OpenKNX.h"

typedef std::function<void(uint32_t value, uint32_t duration, uint32_t pulses)> MeterCalculatorCallback;

class MeterCalculator
{
  protected:
    float _modifier;
    uint16_t _waitTime;
    uint16_t _abortTime;
    uint16_t _reference;

    uint32_t _value = 0;
    uint16_t _pulses = 0;
    uint32_t _startTime = 0;
    uint32_t _lastPulseTime = 0;

    MeterCalculatorCallback _callback;

    void calculate();

  public:
    MeterCalculator(uint16_t reference, uint16_t waitTime, uint16_t abortTime)
        : _reference(reference), _waitTime(waitTime), _abortTime(abortTime){};

    void setCallback(MeterCalculatorCallback callback)
    {
        _callback = callback;
    }

    void pulse();
    void loop();
};