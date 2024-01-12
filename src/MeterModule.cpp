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
    // Every hour
    if (delayCheck(openknx.flash.lastWrite(), 6 * 3600 * 1000))
    {
        openknx.flash.save();
    }

    uint8_t processed = 0;
    do
        _channels[_currentChannel]->loop();

    while (openknx.freeLoopIterate(MTR_ChannelCount, _currentChannel, processed));
}

#define MTR_KoCalcChannel(number) ((number >= MTR_KoBlockOffset && number < MTR_KoBlockOffset + MTR_ChannelCount * MTR_KoBlockSize) ? (number - MTR_KoBlockOffset) / MTR_KoBlockSize : -1)

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
            // forward to channel

            int16_t channel = MTR_KoCalcChannel(asap);
            if (channel >= 0) _channels[channel]->processInputKo(ko);

            // logDebugP("Index %i -> %i", asap, channelIndex);

            // if (asap >= MTR_KoBlockOffset && asap < MTR_KoBlockOffset + MTR_ChannelCount * MTR_KoBlockSize)
            // {
            //     uint8_t i = (asap - MTR_KoBlockOffset) / MTR_KoBlockSize;
            //     _channels[i]->processInputKo(ko);
            // }
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
    if (!knx.configured()) return true;

    logInfoP("Show internal counter");
    logIndentUp();

    for (uint8_t i = 0; i < MTR_ChannelCount; i++)
        _channels[i]->printConsoleCounter();

    logIndentDown();
    return true;
}

bool MeterModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    if (!knx.configured()) return false;
    if (objectIndex != 160) return false;
    if (propertyId != 2) return false;

    switch (data[0])
    {
        case 1:
        {
            uint8_t channel = data[1];
            uint32_t counter = _channels[channel]->counter();
            bool counterSigned = _channels[channel]->counterTypeSigned();
            // counter = (int32_t)-2147483648;
            // counterSigned = true;
            // counter = 4294967295;
            // counterSigned = false;

            uint32_t reference = _channels[channel]->reference();
            bool referenceSigned = _channels[channel]->referenceTypeSigned();
            // reference = (int32_t)-2147483648;
            // referenceSigned = true;
            // reference = 4294967295;
            // referenceSigned = false;

            resultData[0] = 0; // Reserved for ErrorCode

            resultData[1] = counterSigned;
            resultData[2] = (uint8_t)(counter >> 24);
            resultData[3] = (uint8_t)(counter >> 16);
            resultData[4] = (uint8_t)(counter >> 8);
            resultData[5] = (uint8_t)counter;

            resultData[6] = referenceSigned;
            resultData[7] = (uint8_t)(reference >> 24);
            resultData[8] = (uint8_t)(reference >> 16);
            resultData[9] = (uint8_t)(reference >> 8);
            resultData[10] = (uint8_t)reference;
            resultLength = 11;
            return true;
        }
        case 2:
        {
            uint8_t channel = data[1];

            _channels[channel]->reset(data[2]);
            openknx.flash.save();
            resultData[0] = 0;
            resultLength = 1;
            return true;
        }
        case 3:
        {
            uint8_t channel = data[1];
            if ((bool)data[2] != _channels[channel]->counterTypeSigned())
            {
                logErrorP("Convertion error! %i", channel);
                resultData[0] = 1; // Typ error
                resultLength = 1;
                return true;
            }

            uint32_t counter = (data[3] << 24) | (data[4] << 16) | (data[5] << 8) | data[6];
            _channels[channel]->counter(counter);
            openknx.flash.save();

            resultData[0] = 0;
            resultLength = 1;
            return true;
        }
    }

    return false;
}

MeterModule openknxMeterModule;