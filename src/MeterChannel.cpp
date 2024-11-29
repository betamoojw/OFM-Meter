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

    // Debug
    logTraceP("ChannelMode: %u", ParamMTR_ChannelMode);
    logTraceP("ChannelLock: %u", ParamMTR_ChannelLock);
    logTraceP("ChannelInModifier: %f", ParamMTR_ChannelInModifier);
    logTraceP("ChannelOutModifier: %f", ParamMTR_ChannelOutModifier);
    logTraceP("ChannelInType: %u", ParamMTR_ChannelInType);
    logTraceP("ChannelOutType: %u", ParamMTR_ChannelOutType);
    logTraceP("ChannelDurationType: %u", ParamMTR_ChannelDurationType);
    logTraceP("ChannelInFallback: %u", ParamMTR_ChannelInFallback);
    logTraceP("ChannelInPulses: %u", ParamMTR_ChannelInPulses);
    logTraceP("ChannelInDistance: %u", ParamMTR_ChannelInDistance);
    logTraceP("ChannelIgnoreZero: %u", ParamMTR_ChannelIgnoreZero);
    logTraceP("ChannelBackstop: %u", ParamMTR_ChannelBackstop);
    logTraceP("ChannelPulseType: %u", ParamMTR_ChannelPulseType);
    logTraceP("ChannelCalcWaitTime: %u", ParamMTR_ChannelCalcWaitTime);
    logTraceP("ChannelCalcAbortTime: %u", ParamMTR_ChannelCalcAbortTime);
    logTraceP("ChannelSendOnChange: %u", ParamMTR_ChannelSendOnChange);

    _mode = ParamMTR_ChannelMode;
    if (_mode == 0)
    {
        // Reset
        _counter = 0;
        _reference = 0;
    }
    else if (_mode == 1)
    {
        _outModifier = ParamMTR_ChannelOutModifier;
    }
    else if (_mode == 2)
    {
        _outModifier = ParamMTR_ChannelOutModifier;
        _reference = 0; // unused
    }
    else if (_mode == 3)
    {
        _reference = 0;
    }
}

void MeterChannel::loop()
{

    if (!_mode) return;
    if (!openknx.common.afterStartupDelay()) return;

    if (_firstRun)
    {
        // after counter read from flash
        _firstRun = false;
        sendOutput(true);

        if (_mode == 1 && !KoMTR_ChannelInput.initialized())
            KoMTR_ChannelInput.requestObjectRead();

        if (_mode == 2 && ParamMTR_ChannelPulseType && _calculationValue == 0)
            processPulseCalculation(0, 0, 0);
    }

    if (_mode == 2 && ParamMTR_ChannelPulseType) loopPulse();
    if (_mode == 3) loopTimer();

    if (_mode == 1 || _mode == 2)
    {
        int32_t diff = (_counter - _lastOut);
        if (ParamMTR_ChannelSendOnChange && abs(diff) >= ParamMTR_ChannelSendOnChange)
        {
            sendOutput();
        }
        else if (diff != 0)
        {
            sendOutput(false);
        }
    }
}

void MeterChannel::processInputKo(GroupObject &ko)
{
    if (!_mode) return;

    uint16_t koNumber = ko.asap();
    int8_t koIndex = MTR_KoCalcIndex(koNumber);
    // logInfoP("koNumber %i -> koIndex %i", koNumber, koIndex);

    // External KO -> to Channel Mapping
    if (ParamMTR_ChannelInSourceKo > 0 && ParamMTR_ChannelInSourceKo == koNumber)
    {
        koIndex = MTR_KoChannelInput;
    }

    switch (koIndex)
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
    if (ko.value(DPT_Switch))
    {
        logInfoP("Reset");
        _counter = 0;
        _reference = 0;
        sendOutput();
    }
}

void MeterChannel::processInputKoLock(GroupObject &ko)
{
    _locked = ko.value(DPT_Switch);
    logTraceP("processInputKoLock: %s", _locked ? "lock" : "unlock");

    if (_mode == 3 && _locked)
    {
        processTimerCalculation();
        stopTimer();
    }
}

void MeterChannel::processInputKoInput(GroupObject &ko)
{
    uint32_t value = 0;
    int32_t diff = 0;

    // Counter
    if (_mode == 1)
    {
        // DPT 12.xxx
        if (ParamMTR_ChannelInType == 0)
        {
            value = (uint32_t)ko.value(DPT_Value_4_Ucount) * ParamMTR_ChannelInModifier;
            diff = (value - _reference);
            _reference = value;
            logTraceP("New reference %u (diff %i)", _reference, diff);
        }

        // DPT 13.xxx && DPT 14.xxx
        else if (ParamMTR_ChannelInType == 1 || ParamMTR_ChannelInType == 2)
        {
            if (ParamMTR_ChannelInType == 1)
                value = (int32_t)ko.value(DPT_Value_4_Count) * ParamMTR_ChannelInModifier;
            else
                value = (int32_t)((float)ko.value(DPT_Value_Amplitude) * ParamMTR_ChannelInModifier);

            diff = ((int32_t)value - (int32_t)_reference);
            _reference = (int32_t)value;
            logTraceP("New reference %i (diff %i)", _reference, diff);
        }

        if (diff == 0) return;
        if (ParamMTR_ChannelBackstop && diff < 0) return;
        if (ParamMTR_ChannelInDistance > 0 && ParamMTR_ChannelInDistance < abs(diff)) return;
        if (value == 0 && ParamMTR_ChannelIgnoreZero) return;

        logTraceP("Add counter diff %i", diff);

        if (counterTypeSigned())
        {
            _counter = (int32_t)_counter + diff;
        }
        else
        {
            _counter += diff;
        }
    }

    // Impuls und True
    else if (_mode == 2 && ko.value(DPT_Switch))
    {
        _counter++;
        logTraceP("Impuls counter %u (%f)", _counter, (float)_counter / ParamMTR_ChannelInPulses);
        if (ParamMTR_ChannelPulseType) pulse();
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
    logTraceP("processCalculation %f (%ums with %u pulses)", value, duration, pulses);
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
        // first start
        _startTime = millis();
        return;
    }

    _pulses++;
    _lastTime = millis();

    /*
     * Prüf nun ob die Mindestwartezeit bereits vorbei ist. Wenn ja dann berechne die Leistung
     */
    if (delayCheck(_startTime, ParamMTR_ChannelCalcWaitTime * 1000))
    {
        pulseCalculate();
    }
}

void MeterChannel::loopPulse()
{
    // Verarbeitung läuft
    if (_startTime > 0)
    {

        if (delayCheck(_startTime, ParamMTR_ChannelCalcWaitTime * 1000)) pulseCalculate();
        if (delayCheck(_startTime, ParamMTR_ChannelCalcAbortTime * 60000)) abortPulseCalculate();
    }
}

void MeterChannel::abortPulseCalculate()
{
    logTraceP("abortPulseCalculate");
    logIndentUp();
    const uint32_t duration = _lastTime - _startTime;
    processPulseCalculation(0, duration, 0);
    logIndentDown();
    _startTime = 0;
    _pulses = 0;
}

void MeterChannel::pulseCalculate()
{
    if (!_pulses) return;

    logTraceP("pulseCalculate");
    logIndentUp();
    const uint32_t duration = _lastTime - _startTime;
    _calculationValue = 3600000.0 / duration * _pulses * 1000 / ParamMTR_ChannelInPulses;

    processPulseCalculation(_calculationValue, duration, _pulses);
    _startTime = _lastTime;
    _pulses = 0;
    logIndentDown();
}

void MeterChannel::startTimer()
{
    if (_locked) return;
    
    _lastTime = millis();

    if (_startTime) return;

    logTraceP("Start timer");
    _startTime = _lastTime;
    _reference = 0;
}

void MeterChannel::stopTimer()
{
    if (!_startTime) return;

    logTraceP("Stop timer");

    _startTime = 0;
    _lastTime = 0;
    _reference = 0;

    sendOutput();
}

void MeterChannel::loopTimer()
{
    if (!_startTime) return;

    processTimerCalculation();

    if (_lastTime && delayCheck(_lastTime, ParamMTR_ChannelInFallback * 1000))
    {
        logTraceP("Stop timer (Fallback)");
        stopTimer();
    }
}

void MeterChannel::processTimerCalculation()
{
    if (!_startTime) return;
    
    uint32_t duration = (millis() - _startTime) / 1000;

    if (duration == _reference) return;

    _counter = _counter - _reference + duration;
    _reference = duration;

    logTraceP("processTimerCalculation: %us (%us)", _counter, _reference);
    sendOutput(false);
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

    logDebugP("Restore counter %u reference %u", counter, reference);

    _counter = counter;
    if (ParamMTR_ChannelMode == 1) _reference = reference;
}

void MeterChannel::sendOutput(bool send /* = true */)
{
    if (send)
        _lastOut = _counter;

    if (ParamMTR_ChannelMode == 1 || ParamMTR_ChannelMode == 2)
    {
        // DPT 12.xxx
        if (ParamMTR_ChannelOutType == 0)
            if (send)
                KoMTR_ChannelOutput.value(_counter * _outModifier, DPT_Value_4_Ucount);
            else
                KoMTR_ChannelOutput.valueNoSend(_counter * _outModifier, DPT_Value_4_Ucount);

        // DPT 13.xxx
        else if (ParamMTR_ChannelOutType == 1)
            if (send)
                KoMTR_ChannelOutput.value(_counter * _outModifier, DPT_Value_4_Count);
            else
                KoMTR_ChannelOutput.valueNoSend(_counter * _outModifier, DPT_Value_4_Count);

        // DPT 14.xxx
        else if (ParamMTR_ChannelOutType == 2)
        {
            if (send)
                KoMTR_ChannelOutput.value((float)_counter * _outModifier, DPT_Value_Amplitude);
            else
                KoMTR_ChannelOutput.valueNoSend((float)_counter * _outModifier, DPT_Value_Amplitude);
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
    if (!_mode) return;

    if (_mode == 3)
        logInfoP("Counter %us", _counter);
    else if (counterTypeSigned() && referenceTypeSigned())
        logInfoP("Counter %i - Reference %i", (int32_t)_counter, (int32_t)_reference);
    else if (counterTypeSigned() && !referenceTypeSigned())
        logInfoP("Counter %i - Reference %u", (int32_t)_counter, _reference);
    else if (!counterTypeSigned() && referenceTypeSigned())
        logInfoP("Counter %u - Reference %i", _counter, (int32_t)_reference);
    else
        logInfoP("Counter %u - Reference %u", _counter, _reference);
}

bool MeterChannel::counterTypeSigned()
{
    // Standardzähler ohne Rücklaufsperre
    if (_mode == 1 && !ParamMTR_ChannelBackstop) return true;

    return false;
}

bool MeterChannel::referenceTypeSigned()
{
    // Standardzähler mit DPT13 oder 14
    if (_mode == 1 && (ParamMTR_ChannelInType == 1 || ParamMTR_ChannelInType == 2)) return true;

    return false;
}

uint32_t MeterChannel::counter()
{
    return _counter;
}

uint32_t MeterChannel::reference()
{
    return _reference;
}

void MeterChannel::counter(uint32_t value)
{
    if (counterTypeSigned())
        logInfoP("Set new counter %i", value);
    else
        logInfoP("Set new counter %u", value);

    _counter = value;
}

void MeterChannel::reset(bool full /*= false*/)
{
    if (full)
        logInfoP("Reset counter + reference");
    else
        logInfoP("Reset counter");

    _counter = 0;
    if (full) _reference = 0;

    sendOutput();
}