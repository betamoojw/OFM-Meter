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
    logTraceP("ChannelInType: %i", ParamMTR_ChannelInType);
    logTraceP("ChannelOutType: %i", ParamMTR_ChannelOutType);
    logTraceP("ChannelDurationType: %i", ParamMTR_ChannelDurationType);
    logTraceP("ChannelInFallback: %i", ParamMTR_ChannelInFallback);
    logTraceP("ChannelInPulses: %i", ParamMTR_ChannelInPulses);
    logTraceP("ChannelInDistance: %i", ParamMTR_ChannelInDistance);
    logTraceP("ChannelIgnoreZero: %i", ParamMTR_ChannelIgnoreZero);
    logTraceP("ChannelBackstop: %i", ParamMTR_ChannelBackstop);
    logTraceP("ChannelPulseType: %i", ParamMTR_ChannelPulseType);
    logTraceP("ChannelPulseCalculation: %i", ParamMTR_ChannelPulseCalculation);
    logTraceP("ChannelCalcWaitTime: %i", ParamMTR_ChannelCalcWaitTime);
    logTraceP("ChannelCalcAbortTime: %i", ParamMTR_ChannelCalcAbortTime);
    logTraceP("ChannelCalcModifier: %f", ParamMTR_ChannelCalcModifier);

    if (ParamMTR_ChannelPulseCalculation)
    {
        _calculator = new MeterCalculator(ParamMTR_ChannelInPulses, ParamMTR_ChannelCalcWaitTime, ParamMTR_ChannelCalcAbortTime);
        _calculator->setCallback([this](uint32_t value, uint32_t duration, uint32_t pulses) { this->processCalculation(value, duration, pulses); });
    }
}

void MeterChannel::loop()
{
    if (!ParamMTR_ChannelMode) return;

    if (ParamMTR_ChannelPulseCalculation) _calculator->loop();
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
        logDebugP("Impuls counter %i (%f)", reference, ((float)reference / ParamMTR_ChannelInPulses * ParamMTR_ChannelInModifier));
        if (ParamMTR_ChannelPulseCalculation) _calculator->pulse();
    }
}

void MeterChannel::processCalculation(uint32_t value, uint32_t duration, uint32_t pulses)
{
    if (ParamMTR_ChannelPulseType == 1)
    {
        logDebugP("value %i (%ims with %i pulses)", value, duration, pulses);
        KoMTR_ChannelOptional.value(value, DPT_Value_Power);
    }
    else if (ParamMTR_ChannelPulseType == 2)
    {
        logDebugP("Flow %i (%ims with %i pulses)", value, duration, pulses);
        KoMTR_ChannelOptional.value(value, DPT_Value_4_Ucount);
    }
}
