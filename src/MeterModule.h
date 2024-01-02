#pragma once
#include "MeterChannel.h"
#include "OpenKNX.h"

class MeterModule : public OpenKNX::Module
{

  protected:
    MeterChannel *_channels[MTR_ChannelCount];
    uint8_t _currentChannel = 0;

  public:
    void setup() override;
    void loop() override;
    const std::string name() override;
    const std::string version() override;
    void processInputKo(GroupObject &ko) override;
};

extern MeterModule openknxMeterModule;