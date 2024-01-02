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
void MeterModule::processInputKo(GroupObject &ko){
    // we have to check first, if external KO are used
    uint16_t asap = ko.asap();
    switch (asap)
    {
        // case MTR_KoDayNight:
        //     processInputKoDayNight(ko);
        //     break;

        default:
            // forward to trigger
            if (asap >= MTR_KoOffset && asap < MTR_KoOffset + MTR_ChannelCount * MTR_KoBlockSize)
            {
                uint8_t i = (asap - MTR_KoOffset) / MTR_KoBlockSize;
                _channels[i]->processInputKo(ko);
            }
            break;
    }
}

MeterModule openknxMeterModule;