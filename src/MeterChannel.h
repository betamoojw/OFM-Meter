#pragma once
#include "OpenKNX.h"
// #include "MeterCalculator.h"
// #include "MeterTimerCounter.h"

class MeterChannel : public OpenKNX::Channel
{

  protected:
    uint32_t _reference = 0;
    uint32_t _counter = 0;
    uint32_t _lastOut = 0;
    // MeterCalculator *_calculator = nullptr;
    // MeterTimeCounter *_timeCounter = nullptr;
    void processInputKoInput(GroupObject &ko);
    void processInputKoReset(GroupObject &ko);
    void processInputKoLock(GroupObject &ko);

    float _calculationValue = 0;
    uint16_t _pulses = 0;
    uint32_t _startTime = 0;
    uint32_t _lastTime = 0;
    uint8_t _mode = 0;
    uint16_t pulseDivider = 1;
    bool _locked = false;
    bool _running = false;
    bool _afterStartup = false;

    void sendOutput(bool send = true);

    void pulse();
    void loopPulse();
    void pulseCalculate();
    void processPulseCalculation(float value, uint32_t duration, uint32_t pulses);

    void loopTimer();
    void startTimer();
    void stopTimer();
    void processTimerCalculation();

    bool counterTypeSigned();
    bool referenceTypeSigned();

  public:
    MeterChannel(uint8_t index);

    void setup() override;
    void loop() override;
    void printConsoleCounter();
    void processInputKo(GroupObject &ko) override;
    void processTimeCounter(uint32_t seconds);
    const std::string name() override;

    void save();
    void restore();
};
