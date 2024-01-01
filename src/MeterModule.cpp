#include "MeterModule.h"

const std::string MeterModule::name()
{
    return "Meter";
}

const std::string MeterModule::version()
{
    return MODULE_Meter_Version;
}

void MeterModule::setup()
{
    for (uint8_t i = 0; i < MTR_ChannelCount; i++)
    {
        _channels[i] = new MeterChannel(i);
        _channels[i]->setup();
    }
}

void MeterModule::loop()
{
    uint8_t processed = 0;
    do
        _channels[_currentChannel]->loop();

    while (openknx.freeLoopIterate(MTR_ChannelCount, _currentChannel, processed));
}

MeterModule openknxMeterModule;