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
    void showHelp() override;
    bool processCommand(const std::string command, bool diagnose) override;
    void processInputKo(GroupObject &ko) override;

    const std::string name() override;
    const std::string version() override;

    void readFlash(const uint8_t *buffer, const uint16_t size) override;
    void writeFlash() override;
    uint16_t flashSize() override;

    bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;
};

extern MeterModule openknxMeterModule;