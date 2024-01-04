#pragma once
#include "OpenKNX.h"
#include "MeterCalculator.h"

class MeterChannel : public OpenKNX::Channel
{

  protected:
    uint32_t reference = 0;
    uint32_t counter = 0;
    uint32_t referenceTime = 0;
    uint32_t powerTime = 0;
    uint16_t powerCounter = 0;
    MeterCalculator *_calculator = nullptr;
    void processInputKoInput(GroupObject &ko);

  public:
    MeterChannel(uint8_t index);

    void setup() override;
    void loop() override;
    void processInputKo(GroupObject &ko) override;
    void processCalculation(uint32_t value, uint32_t duration, uint32_t pulses);
    const std::string name() override;
};
