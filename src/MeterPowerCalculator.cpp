#include "MeterPowerCalculator.h"

#define WAIT_TIME 1000
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

    if (delayCheck(_time, WAIT_TIME))
    {
        calculate();
    }
}

void MeterPowerCalculator::loop()
{
    // Wait time for to fast trigger
    if (_pulses > 0 && delayCheck(_time, WAIT_TIME))
    {
        calculate();
    }

    // Abort
    if (_power > 0 && _time > 0 && delayCheck(_time, ABORT_TIME))
    {
        _power = 0;
        _time = _lastPulseTime;
        _pulses = 0;
        _callback(_power, 0);
    }

    if (delayCheck(_test, 30))
    {
        // pulse();
        _test = millis();
    }
}

void MeterPowerCalculator::calculate()
{
    const uint32_t duration = _lastPulseTime - _time;
    _power = round(1.0 / ((double)duration / (3600000.0 / _reference * _pulses)));

    _callback(_power, duration);

    _time = millis();
    //_lastPulseTime = 0;
    _pulses = 0;
}
