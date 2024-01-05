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

void MeterModule::processInputKo(GroupObject &ko)
{
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

uint16_t MeterModule::flashSize()
{
    // Version + Data (Channel * Inputs * (Dpt + Value))
    return 1 + (MTR_ChannelCount * 8);
}

void MeterModule::readFlash(const uint8_t *buffer, const uint16_t size)
{
    // first call - without data
    if (size == 0) return;

    uint8_t version = openknx.flash.readByte();
    if (version != 1) // version unknown
    {
        logErrorP("Wrong version of flash data (%i)", version);
        return;
    }

    uint8_t savedChannels = (size - 1) / 8;
    logDebugP("Reading channel data from flash (%i/%i)", savedChannels, MTR_ChannelCount);
    for (uint8_t i = 0; i < MIN(savedChannels, MTR_ChannelCount); i++)
    {
        _channels[i]->restore();
    }
}

void MeterModule::writeFlash()
{
    openknx.flash.writeByte(1); // Version
    for (uint8_t i = 0; i < MTR_ChannelCount; i++)
    {
        _channels[i]->save();
    }
}

void MeterModule::showHelp()
{
    openknx.console.printHelpLine("meter", "Show meter counter");
}

bool MeterModule::processCommand(const std::string command, bool diagnose)
{
    if (diagnose) return false;
    if (command != "meter") return false;

    for (uint8_t i = 0; i < MTR_ChannelCount; i++)
        _channels[i]->printConsoleCounter();

    return true;
}

MeterModule openknxMeterModule;