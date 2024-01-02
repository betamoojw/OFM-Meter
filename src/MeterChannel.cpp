#include "MeterChannel.h"

MeterChannel::MeterChannel(uint8_t index)
{
    _channelIndex = index;
}

const std::string MeterChannel::name()
{
    return "Meter";
}

void MeterChannel::setup()
{
    if (!ParamMTR_ChannelMode)
        return;

    // KoBI_ChannelOutput.valueNoSend(false, DPT_Switch);

    // Debug
    logTraceP("ParamMTR_ChannelMode: %i", ParamMTR_ChannelMode);
    logTraceP("ParamMTR_ChannelLock: %i", ParamMTR_ChannelLock);
    logTraceP("ParamMTR_ChannelInModifier: %f", ParamMTR_ChannelInModifier);
    logTraceP("ParamMTR_ChannelOutModifier: %f", ParamMTR_ChannelOutModifier);
    logTraceP("ParamMTR_ChannelInType: %i", ParamMTR_ChannelInType);
    logTraceP("ParamMTR_ChannelOutType: %i", ParamMTR_ChannelOutType);
    logTraceP("ParamMTR_ChannelDurationType: %i", ParamMTR_ChannelDurationType);
    logTraceP("ParamMTR_ChannelInFallback: %i", ParamMTR_ChannelInFallback);
    logTraceP("ParamMTR_ChannelInPulses: %i", ParamMTR_ChannelInPulses);
    logTraceP("ParamMTR_ChannelInDistance: %i", ParamMTR_ChannelInDistance);
    logTraceP("ParamMTR_ChannelIgnoreZero: %i", ParamMTR_ChannelIgnoreZero);
    logTraceP("ParamMTR_ChannelPowerCalc: %i", ParamMTR_ChannelPowerCalc);
    logTraceP("ParamMTR_ChannelBackstop: %i", ParamMTR_ChannelBackstop);

    if (ParamMTR_ChannelPowerCalc)
        _powerCalculator = new MeterPowerCalculator(ParamMTR_ChannelInPulses);
}

void MeterChannel::loop()
{
    if (!ParamMTR_ChannelMode)
        return;
}

void MeterChannel::processInputKo(GroupObject &ko)
{
    if (!ParamMTR_ChannelMode)
        return;

    switch (MTR_KoCalcIndex(ko.asap()))
    {
        case MTR_KoChannelInput:
            processInputKoInput(ko);
            break;
    }
}

void MeterChannel::processInputKoInput(GroupObject &ko)
{
    if (ParamMTR_ChannelMode == 2 && ko.value(DPT_Switch))
    {
        reference++;
        referenceTime = millis();
        if (reference >= ParamMTR_ChannelInPulses)
        {
            reference -= ParamMTR_ChannelInPulses;
            counter++;
        }
        logDebugP("Impuls pulses %i counter %i", reference, counter);

        if (ParamMTR_ChannelPowerCalc)
        {
            uint32_t power = 0;
            if(_powerCalculator->calculate(power)) {
                logDebugP("Power %i", power);
            }


            // if (powerTime > 0)
            // {
            //     if (delayCheck(powerTime, 1000))
            //     {
            //         float power = 0;
            //         uint32_t diff = (millis() - powerTime);
            //         power = 1.0 / (diff / (3600000.0 / ParamMTR_ChannelInPulses)) * powerCounter;
            //         logDebugP("Power %f (%ims)", power, diff);
            //         powerTime = millis();
            //     }
            // }
            // else
            // {
            //     powerTime = millis();
            // }
        }
    }
}