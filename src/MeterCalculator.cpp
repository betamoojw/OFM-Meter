#include "MeterCalculator.h"

void MeterCalculator::pulse()
{
    if (_startTime == 0)
    {
        _startTime = millis();
        return;
    }

    _pulses++;
    _lastPulseTime = millis();

    if (delayCheck(_startTime, _waitTime * 1000))
    {
        calculate();
    }
}
void MeterCalculator::loop()
{
    // Wait time
    if (_pulses > 0 && delayCheck(_startTime, _waitTime * 1000)) calculate();

    // Abort time
    if (_value > 0 && _startTime > 0 && delayCheck(_startTime, _abortTime * 60000)) calculate();
}

void MeterCalculator::calculate()
{
    const uint32_t duration = _lastPulseTime - _startTime;

    // Same result and consume same time
    _value = ceil(3600000.0 / duration * _pulses * 1000 / _reference);
    // _value = ceil(1000.0 / (duration / (3600000.0 * _pulses / _reference)));

    _callback(_value, duration, _pulses);
    _startTime = _lastPulseTime;
    _pulses = 0;
}
