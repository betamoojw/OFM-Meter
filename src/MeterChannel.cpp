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
    if (!ParamMTR_ChannelMode) return;

    // KoBI_ChannelOutput.valueNoSend(false, DPT_Switch);

    // Debug
    logTraceP("ChannelMode: %i", ParamMTR_ChannelMode);
    logTraceP("ChannelLock: %i", ParamMTR_ChannelLock);
    logTraceP("ChannelInModifier: %f", ParamMTR_ChannelInModifier);
    logTraceP("ChannelOutModifier: %f", ParamMTR_ChannelOutModifier);
    logTraceP("ChannelPowerModifier: %f", ParamMTR_ChannelPowerModifier);
    logTraceP("ChannelInType: %i", ParamMTR_ChannelInType);
    logTraceP("ChannelOutType: %i", ParamMTR_ChannelOutType);
    logTraceP("ChannelDurationType: %i", ParamMTR_ChannelDurationType);
    logTraceP("ChannelInFallback: %i", ParamMTR_ChannelInFallback);
    logTraceP("ChannelInPulses: %i", ParamMTR_ChannelInPulses);
    logTraceP("ChannelInDistance: %i", ParamMTR_ChannelInDistance);
    logTraceP("ChannelIgnoreZero: %i", ParamMTR_ChannelIgnoreZero);
    logTraceP("ChannelPowerCalc: %i", ParamMTR_ChannelPowerCalc);
    logTraceP("ChannelBackstop: %i", ParamMTR_ChannelBackstop);
    logTraceP("ChannelPowerWaitTime: %i", ParamMTR_ChannelPowerWaitTime);
    logTraceP("ChannelPowerAbortTime: %i", ParamMTR_ChannelPowerAbortTime);

    if (ParamMTR_ChannelPowerCalc)
    {
        _powerCalculator = new MeterPowerCalculator(ParamMTR_ChannelInPulses, ParamMTR_ChannelPowerModifier, ParamMTR_ChannelPowerWaitTime, ParamMTR_ChannelPowerAbortTime);
        _powerCalculator->setCallback([this](uint32_t power, uint32_t duration, uint32_t pulses) { this->processPowerCalculation(power, duration, pulses); });
    }
}

void MeterChannel::loop()
{
    if (!ParamMTR_ChannelMode) return;

    if (ParamMTR_ChannelPowerCalc)
    {
        _powerCalculator->loop();
    }
}

void MeterChannel::processInputKo(GroupObject &ko)
{
    if (!ParamMTR_ChannelMode) return;

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
        // logTraceP("Impuls pulses %i counter %i", reference, counter);
        if (ParamMTR_ChannelPowerCalc)
        {

            _powerCalculator->pulse();
        }
    }
}

void MeterChannel::processPowerCalculation(uint32_t power, uint32_t duration, uint32_t pulses)
{
    logDebugP("Power %i (%ims with %i pulses)", power, duration, pulses);
    KoMTR_ChannelOptional.value(power, DPT_Value_Power);
}