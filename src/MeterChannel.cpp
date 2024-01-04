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

    _mode = ParamMTR_ChannelMode;
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
    _counter = 0;
}

void MeterChannel::processInputKoLock(GroupObject &ko)
{
    _locked = ko.value(DPT_Switch);
    logDebugP("processInputKoLock: %s", _locked ? "lock" : "unlock");
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
    logDebugP("processCalculation %f (%ims with %i pulses)", value, duration, pulses);
    if (ParamMTR_ChannelPulseType == 1)
    {
        KoMTR_ChannelOptional.value(value, DPT_Value_Power);
    }
    else if (ParamMTR_ChannelPulseType == 2)
    {
        KoMTR_ChannelOptional.value(value, DPT_Value_Volume_Flow);
    }
}

void MeterChannel::pulse()
{
    if (_startTime == 0)
    {
        _startTime = millis();
        return;
    }

    _pulses++;
    _lastTime = millis();

    if (delayCheck(_startTime, ParamMTR_ChannelCalcWaitTime * 1000))
    {
        pulseCalculate();
    }
}

void MeterChannel::loopPulse()
{
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
    logDebugP("timerStop %is", _counter);

    // process last second(s) with ceil
    if (!_locked) _counter += ceil((millis() - _startTime) / 1000);

    _startTime = 0;
    _lastTime = 0;
    _running = false;

    if (ParamMTR_ChannelDurationType == 0)
    {
        KoMTR_ChannelOutut.value(_counter / 3600, DPT_TimePeriodHrs);
    }
    else if (ParamMTR_ChannelDurationType == 1)
    {
        KoMTR_ChannelOutut.value(_counter / 3600, DPT_Value_4_Ucount);
    }
    else if (ParamMTR_ChannelDurationType == 2)
    {
        KoMTR_ChannelOutut.value(_counter / 60, DPT_Value_4_Ucount);
    }
    else if (ParamMTR_ChannelDurationType == 3)
    {
        KoMTR_ChannelOutut.value(_counter, DPT_Value_4_Ucount);
    }
}

void MeterChannel::loopTimer()
{
    if (_running)
    {
        if (_lastTime && delayCheck(_lastTime, ParamMTR_ChannelInFallback * 1000))
        {
            logErrorP("Stop timer (Fallback)");
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
