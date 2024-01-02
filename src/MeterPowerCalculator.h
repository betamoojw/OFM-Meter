#pragma once
#include "OpenKNX.h"

class MeterPowerCalculator
{
  private:
    uint16_t _reference = 0;
    uint16_t _pulses = 0;
    uint32_t _time = 0;

  public:
    MeterPowerCalculator(uint16_t reference) : _reference(reference){};
    bool calculate(uint32_t power);
};