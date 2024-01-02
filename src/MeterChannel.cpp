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
    {
        _powerCalculator = new MeterPowerCalculator(ParamMTR_ChannelInPulses);
        _powerCalculator->setCallback([this](uint32_t power, uint32_t duration) { this->processPower(power, duration); });
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

void MeterChannel::processPower(uint32_t power, uint32_t duration)
{
    logDebugP("Power %i (%ims)", power, duration);
    KoMTR_ChannelOptional.value(power, DPT_Value_Power);
}