#include "mbed.h"
#include <vector>

#ifndef SENSOR_H
#define SENSOR_H

class Buffer
{
    private:
        float buf[8];
        int size, count, in, out;
    public:
        Buffer()
            : in(0), out(0), count(0), size(8) {}

        void put (float i) {
            buf[in] = i;
            if (count<8) {++count;}
            if(in == 7) {in=0;}
            else {++in;}
        }

        float const getAverage() {
            float sum = 0;
            for (int i = 0; i < count; i++) {
                sum += buf[i];
            }
            return (float) sum / count;
        }

        float const getAverageNoAnomalies() {
            float sum = 0;
            float sorted[8];

            std::copy(buf, buf + size, sorted);
            std::sort(sorted, sorted + size);

            for (int i = 1; i < count - 1; i++) {
                sum += sorted[i];
            }
            return (float) sum / count;
        }
};

class Sensor
{
    protected:
        Buffer buf;
        DigitalOut control;
        AnalogIn inputSignal;
        float VDD, currentSampleNorm, currentSampleVolts;
        int index; 

    public:
        Sensor(PinName i, float v, PinName c)
            : control(c), inputSignal(i), VDD(v) {
                control.write(1);
            }
            
        void sample(void)
        {
            currentSampleNorm = inputSignal.read();
            currentSampleVolts = currentSampleNorm * VDD;
            buf.put(currentSampleNorm);
        }

        float const amplitudeVolts() { return inputSignal.read() * VDD; }
        float const amplitudeNorm() { return inputSignal.read(); }
        
        float const getAverage() { return buf.getAverageNoAnomalies(); }
        float const getCurrentSampleVolts(void) { return currentSampleVolts; }
        float const getCurrentSampleNorm(void) { return currentSampleNorm; }

};

class SamplingSensor: public Sensor
{
    protected:
        float samplingFrequency, samplingPeriod;
        Ticker sampler;

    public:
        SamplingSensor(PinName i, float v, PinName c, float fs)
            : Sensor(i, v, c), samplingFrequency(fs), samplingPeriod(1.0 / fs) {
            sampler.attach(callback(this, &SamplingSensor::sample), samplingPeriod);
        };

};

class Sensors
{
    protected:
        float samplingFrequency, samplingPeriod;
        Ticker sampler;
        int index, size;
        Sensor sensors[5];
    public:
        Sensors(Sensor &s1, Sensor &s2, Sensor &s3, Sensor &s4, Sensor &s5, float fs)
            : samplingFrequency(fs), samplingPeriod(1.0 / fs), sensors{s1, s2, s3, s4, s5}, size(sizeof(sensors) / sizeof(sensors[0])) {
            sampler.attach(callback(this, &Sensors::sample), samplingPeriod);
            }

        void sample() {
            for (int i = -2; i < size - 2; i++) {
                index += i * sensors[i].amplitudeNorm();
            }
        }

        int const getIndex() { return index; }
};

#endif