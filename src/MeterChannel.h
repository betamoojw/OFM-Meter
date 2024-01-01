#pragma once
#include "OpenKNX.h"

class MeterChannel : public OpenKNX::Channel
{

  protected:

  public:
    MeterChannel(uint8_t index);

    void setup() override;
    void loop() override;
    const std::string name() override;
};
