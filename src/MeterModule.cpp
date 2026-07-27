#include "MeterModule.h"
#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    #include <NetworkModule.h>
#endif

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
    uint8_t active = 0;
    for (uint8_t i = 0; i < MTR_ChannelCount; i++)
    {
        MeterChannel* ch = new MeterChannel(i);
        if (ch->isActive())
        {
            _channels[i] = ch;
            _channels[i]->setup();
            active++;
        }
        else
        {
            delete ch;
        }
    }
    logInfoP("Setup completed with %u/%u channels active", active, MTR_ChannelCount);

#if defined(OPENKNX_WEBSERVER) && (defined(KNX_IP_LAN) || defined(KNX_IP_WIFI))
    if (knx.configured())
    {
        openknxNetwork.webserver.addMenuItem("Z\u00e4hlermodul", "/meter");
        openknxNetwork.webserver.addRoute(OpenKNX::Network::WEB_GET, "/meter", [](OpenKNX::Network::WebRequest& /*req*/, OpenKNX::Network::WebResponse& res) {
            if (!knx.configured())
            {
                res.setStatus(404);
                res.send("");
                return;
            }

            std::string html = "<div class='container'><h1>Z&auml;hlermodul</h1><h2>Kan&auml;le</h2>";
            html += "<table><thead><tr>"
                    "<th>Kanal</th><th>Modus</th><th>Interner Z&auml;hler</th><th>Referenzz&auml;hler</th>"
                    "</tr></thead><tbody>";

            for (uint8_t i = 0; i < MTR_ChannelCount; i++)
            {
                MeterChannel* ch = openknxMeterModule.getChannel(i);

                html += "<tr><td>Kanal ";
                html += std::to_string(i + 1);
                html += "</td><td>";

                if (!ch || !ch->mode())
                {
                    html += "Unkonfiguriert</td><td>&mdash;</td><td>&mdash;</td></tr>";
                    continue;
                }

                switch (ch->mode())
                {
                    case 1: html += "Standardz&auml;hler"; break;
                    case 2: html += "Impulsz&auml;hler";   break;
                    case 3: html += "Zeitz&auml;hler";      break;
                    default: html += "&mdash;";              break;
                }

                html += "</td><td>";
                uint32_t counter = ch->counter();
                if (ch->counterTypeSigned())
                    html += std::to_string((int32_t)counter);
                else
                    html += std::to_string(counter);

                html += "</td><td>";
                if (ch->mode() == 1)
                {
                    uint32_t reference = ch->reference();
                    if (ch->referenceTypeSigned())
                        html += std::to_string((int32_t)reference);
                    else
                        html += std::to_string(reference);
                }
                else
                {
                    html += "&mdash;";
                }

                html += "</td></tr>";
            }

            html += "</tbody></table></div>";
            res.setLayout(true);
            res.send(html.c_str());
        });
    }
#endif
}

MeterChannel *MeterModule::getChannel(uint8_t index)
{
    if (index >= MTR_ChannelCount)
        return nullptr;

    return _channels[index];
}

void MeterModule::loop()
{
    uint8_t processed = 0;
    do
    {
        if (_channels[_currentChannel] != nullptr)
            _channels[_currentChannel]->loop();
    }
    while (openknx.freeLoopIterate(MTR_ChannelCount, _currentChannel, processed));
}

void MeterModule::processInputKo(GroupObject &ko)
{
    for (uint8_t i = 0; i < MTR_ChannelCount; i++)
        if (_channels[i] != nullptr) _channels[i]->processInputKo(ko);
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
        if (_channels[i] != nullptr)
            _channels[i]->restore();
        else
            for (uint8_t j = 0; j < 8; j++) openknx.flash.readByte();
    }
}

void MeterModule::writeFlash()
{
    openknx.flash.writeByte(1); // Version
    for (uint8_t i = 0; i < MTR_ChannelCount; i++)
    {
        if (_channels[i] != nullptr)
            _channels[i]->save();
        else
            for (uint8_t j = 0; j < 8; j++) openknx.flash.writeByte(0);
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
        if (_channels[i] != nullptr) _channels[i]->printConsoleCounter();

    logIndentDown();
    return true;
}

bool MeterModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    if (!knx.configured()) return false;
    if (objectIndex != 160) return false;
    if (propertyId != 2) return false;
    if (length < 2) return false; // action and channel

    switch (data[0])
    {
        case 1:
        {
            uint8_t channel = data[1];
            if (channel >= MTR_ChannelCount || _channels[channel] == nullptr) return false;
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
            if (length < 3) return false; // plus the full flag

            uint8_t channel = data[1];
            if (channel >= MTR_ChannelCount || _channels[channel] == nullptr) return false;

            _channels[channel]->reset(data[2]);
            openknx.flash.save();
            resultData[0] = 0;
            resultLength = 1;
            return true;
        }
        case 3:
        {
            if (length < 7) return false; // plus the sign flag and 4 counter bytes

            uint8_t channel = data[1];
            if (channel >= MTR_ChannelCount || _channels[channel] == nullptr) return false;

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