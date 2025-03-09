#include "mbed.h"

#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

class Potentiometer
{
    protected:
        AnalogIn inputSignal;
        float VDD, currentSampleNorm, currentSampleVolts;

    public:
        Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {}

        void sample(void)
        {
            currentSampleNorm = inputSignal.read();
            currentSampleVolts = currentSampleNorm * VDD;
        }
        
        float const getCurrentSampleVolts(void) { return currentSampleVolts; }
        float const getCurrentSampleNorm(void) { return currentSampleNorm; }

        float const amplitudeVolts(void) { return (inputSignal.read()*VDD); }
        float const amplitudeNorm(void) { return inputSignal.read(); }
};

class SamplingPotentiometer : public Potentiometer
{
    private:
        float samplingFrequency, samplingPeriod;
        Ticker sampler;

    public:
        SamplingPotentiometer(PinName p, float v, float fs)
            : Potentiometer(p, v), samplingFrequency(fs), samplingPeriod(1.0 / fs) {
            sampler.attach(callback(this, &SamplingPotentiometer::sample), samplingPeriod);
        };
};

#endif