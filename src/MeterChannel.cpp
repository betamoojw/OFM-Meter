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
    logDebugP("ChannelMode: %i", ParamMTR_ChannelMode);
    logDebugP("ChannelLock: %i", ParamMTR_ChannelLock);
    logDebugP("ChannelInModifier: %f", ParamMTR_ChannelInModifier);
    logDebugP("ChannelOutModifier: %f", ParamMTR_ChannelOutModifier);
    logDebugP("ChannelInType: %i", ParamMTR_ChannelInType);
    logDebugP("ChannelOutType: %i", ParamMTR_ChannelOutType);
    logDebugP("ChannelDurationType: %i", ParamMTR_ChannelDurationType);
    logDebugP("ChannelInFallback: %i", ParamMTR_ChannelInFallback);
    logDebugP("ChannelInPulses: %i", ParamMTR_ChannelInPulses);
    logDebugP("ChannelInDistance: %i", ParamMTR_ChannelInDistance);
    logDebugP("ChannelIgnoreZero: %i", ParamMTR_ChannelIgnoreZero);
    logDebugP("ChannelBackstop: %i", ParamMTR_ChannelBackstop);
    logDebugP("ChannelPulseType: %i", ParamMTR_ChannelPulseType);
    logDebugP("ChannelPulseCalculation: %i", ParamMTR_ChannelPulseCalculation);
    logDebugP("ChannelCalcWaitTime: %i", ParamMTR_ChannelCalcWaitTime);
    logDebugP("ChannelCalcAbortTime: %i", ParamMTR_ChannelCalcAbortTime);

    _mode = ParamMTR_ChannelMode;
    if (_mode == 2)
    {
        pulseDivider = ParamMTR_ChannelInPulses;
    }

    sendOutput(false);
}

void MeterChannel::loop()
{
    if (!_mode) return;

    if (_mode == 2 && ParamMTR_ChannelPulseCalculation) loopPulse();
    if (_mode == 3) loopTimer();
}

void MeterChannel::processInputKo(GroupObject &ko)
{
    if (!_mode) return;

    switch (MTR_KoCalcIndex(ko.asap()))
    {
        case MTR_KoChannelInput:
            processInputKoInput(ko);
            break;
        case MTR_KoChannelReset:
            processInputKoReset(ko);
            break;
        case MTR_KoChannelOptional:
            if (_mode != 2) processInputKoLock(ko);
            break;
    }
}

void MeterChannel::processInputKoReset(GroupObject &ko)
{
    logInfoP("Reset");
    _counter = 0;
    _reference = 0;
    sendOutput();
}

void MeterChannel::processInputKoLock(GroupObject &ko)
{
    _locked = ko.value(DPT_Switch);
    logTraceP("processInputKoLock: %s", _locked ? "lock" : "unlock");
}

void MeterChannel::processInputKoInput(GroupObject &ko)
{
    // Counter
    if (_mode == 1)
    {
    }

    // Impuls und True
    else if (_mode == 2 && ko.value(DPT_Switch))
    {
        _counter++;
        logTraceP("Impuls counter %i (%f)", _counter, (float)_counter / ParamMTR_ChannelInPulses);
        if (ParamMTR_ChannelPulseCalculation) pulse();
    }

    // Counter
    else if (_mode == 3)
    {
        if (ko.value(DPT_Switch))
            startTimer();
        else
            stopTimer();
    }
}

void MeterChannel::processPulseCalculation(float value, uint32_t duration, uint32_t pulses)
{
    logTraceP("processCalculation %f (%ims with %i pulses)", value, duration, pulses);
    if (ParamMTR_ChannelPulseType == 1)
    {
        KoMTR_ChannelOptional.value(value, DPT_Value_Power);
    }
    else if (ParamMTR_ChannelPulseType == 2)
    {
        KoMTR_ChannelOptional.value(value, DPT_Value_Volume_Flow);
    }

    sendOutput();
}

void MeterChannel::pulse()
{
    if (_startTime == 0)
    {
        _startTime = millis();
        return;
    }

    _counter++;
    _pulses++;
    _lastTime = millis();

    if (delayCheck(_startTime, ParamMTR_ChannelCalcWaitTime * 1000))
    {
        pulseCalculate();
    }
}

void MeterChannel::loopPulse()
{
    if (openknx.common.afterStartupDelay() && !_afterStartup)
    {
        sendOutput();

        _afterStartup = true;
    }

    // Wait time
    if (_pulses > 0 && delayCheck(_startTime, ParamMTR_ChannelCalcWaitTime * 1000)) pulseCalculate();

    // Abort time
    if (_calculationValue > 0 && _startTime > 0 && delayCheck(_startTime, ParamMTR_ChannelCalcAbortTime * 60000)) pulseCalculate();
}

void MeterChannel::pulseCalculate()
{
    const uint32_t duration = _lastTime - _startTime;

    // Same result and consume same time
    _calculationValue = 3600000.0 / duration * _pulses * 1000 / ParamMTR_ChannelInPulses;
    // _calculationValue = 1000.0 / (duration / (3600000.0 * _pulses / ParamMTR_ChannelInPulses));

    processPulseCalculation(_calculationValue, duration, _pulses);
    _startTime = _lastTime;
    _pulses = 0;
}

void MeterChannel::startTimer()
{
    _lastTime = millis();

    if (_running) return;

    logTraceP("Start timer");
    _startTime = _lastTime;
    _running = true;
}

void MeterChannel::stopTimer()
{
    logTraceP("Stop timer");

    // process last second(s) with ceil
    if (!_locked) _counter += ceil((millis() - _startTime) / 1000);

    _startTime = 0;
    _lastTime = 0;
    _running = false;

    sendOutput();
}

void MeterChannel::loopTimer()
{
    if (_running)
    {
        if (_lastTime && delayCheck(_lastTime, ParamMTR_ChannelInFallback * 1000))
        {
            logTraceP("Stop timer (Fallback)");
            stopTimer();
        }

        if (delayCheck(_startTime, 1000))
        {
            processTimerCalculation();
        }
    }
}

void MeterChannel::processTimerCalculation()
{
    uint8_t seconds = (millis() - _startTime) / 1000;

    if (!seconds) return;

    // move start time
    _startTime += seconds * 1000;

    // skip if locked and not add to counter
    if (!_locked) _counter += seconds;

    logTraceP("processTimerCalculation: %is (%is)", seconds, _counter);
}

void MeterChannel::save()
{
    openknx.flash.writeInt(_counter);
    openknx.flash.writeInt(_reference);
}

void MeterChannel::restore()
{
    uint32_t counter = openknx.flash.readInt();
    uint32_t reference = openknx.flash.readInt();

    if (!ParamMTR_ChannelMode) return;

    logDebugP("Restore counter %i reference %i", counter, reference);

    _counter = counter;
    if (ParamMTR_ChannelMode == 1) _reference = reference;
}

void MeterChannel::sendOutput(bool send /* = true */)
{
    logDebugP("sendCounter %i", _counter);
    if (ParamMTR_ChannelMode == 1 || ParamMTR_ChannelMode == 2)
    {
        // DPT 12.xxx
        if (ParamMTR_ChannelOutType == 0)
            if (send)
                KoMTR_ChannelOutput.value(_counter * ParamMTR_ChannelOutModifier / pulseDivider, DPT_Value_4_Ucount);
            else
                KoMTR_ChannelOutput.valueNoSend(_counter * ParamMTR_ChannelOutModifier / pulseDivider, DPT_Value_4_Ucount);

        // DPT 13.xxx
        else if (ParamMTR_ChannelOutType == 1)
            if (send)
                KoMTR_ChannelOutput.value(_counter * ParamMTR_ChannelOutModifier / pulseDivider, DPT_Value_4_Count);
            else
                KoMTR_ChannelOutput.valueNoSend(_counter * ParamMTR_ChannelOutModifier / pulseDivider, DPT_Value_4_Count);

        // DPT 14.xxx
        else if (ParamMTR_ChannelOutType == 2)
        {
            if (send)
                KoMTR_ChannelOutput.value((float)_counter * ParamMTR_ChannelOutModifier / pulseDivider, DPT_Value_Amplitude);
            else
                KoMTR_ChannelOutput.valueNoSend((float)_counter * ParamMTR_ChannelOutModifier / pulseDivider, DPT_Value_Amplitude);
        }
    }
    else if (ParamMTR_ChannelMode == 3)
    {
        // DPT 7.7
        if (ParamMTR_ChannelDurationType == 0)
            if (send)
                KoMTR_ChannelOutput.value(_counter / 3600, DPT_TimePeriodHrs);
            else
                KoMTR_ChannelOutput.valueNoSend(_counter / 3600, DPT_TimePeriodHrs);

        // DPT 12.102
        else if (ParamMTR_ChannelDurationType == 1)
            if (send)
                KoMTR_ChannelOutput.value(_counter / 3600, DPT_Value_4_Ucount);
            else
                KoMTR_ChannelOutput.valueNoSend(_counter / 3600, DPT_Value_4_Ucount);

        // DPT 12.101
        else if (ParamMTR_ChannelDurationType == 2)
            if (send)
                KoMTR_ChannelOutput.value(_counter / 60, DPT_Value_4_Ucount);
            else
                KoMTR_ChannelOutput.valueNoSend(_counter / 60, DPT_Value_4_Ucount);

        // DPT 12.100
        else if (ParamMTR_ChannelDurationType == 3)
            if (send)
                KoMTR_ChannelOutput.value(_counter, DPT_Value_4_Ucount);
            else
                KoMTR_ChannelOutput.valueNoSend(_counter, DPT_Value_4_Ucount);
    }
}

void MeterChannel::printConsoleCounter()
{
    if (_mode > 0) logInfoP("Internal Counter: %i (Reference %i)", _counter, _reference);
}