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
}
void MeterChannel::loop()
{
    if (!ParamMTR_ChannelMode)
        return;
}
