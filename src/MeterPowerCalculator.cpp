#include "MeterPowerCalculator.h"

#define WAIT_TIME 5000
#define ABORT_TIME 60000

void MeterPowerCalculator::pulse()
{
    if (_time == 0)
    {
        _time = millis();
        return;
    }

    _pulses++;
    _lastPulseTime = millis();

    if (delayCheck(_time, _waitTime * 1000))
    {
        calculate();
    }
}

void MeterPowerCalculator::loop()
{
    // Wait time for to fast trigger
    if (_pulses > 0 && delayCheck(_time, _waitTime * 1000)) calculate();

    // Abort
    if (_power > 0 && _time > 0 && delayCheck(_time, _abortTime * 60000)) calculate();
}

void MeterPowerCalculator::calculate()
{
    const uint32_t duration = _lastPulseTime - _time;
    _power = ceil(1.0 / ((double)duration / (3600000.0 / _reference * _pulses)) * _modifier);

    _callback(_power, duration, _pulses);

    _time = _lastPulseTime;
    _pulses = 0;
}
