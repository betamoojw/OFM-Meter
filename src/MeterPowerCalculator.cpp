#include "MeterPowerCalculator.h"

bool MeterPowerCalculator::calculate(uint32_t power)
{
    if (_time == 0)
    {
        _time = millis();
        return false;
    }

    _pulses++;

    if (delayCheck(_time, 1000))
    {
        const uint32_t diff = millis() - _time;

        power = round(1.0 / (diff / (3600000.0 / _reference / _pulses)));
        logDebug("MeterPowerCalculator", "Power %i (%ims - %i)", power, diff, _pulses);

        _pulses = 0;
        _time = millis();
        return true;
    }

    return false;
}
