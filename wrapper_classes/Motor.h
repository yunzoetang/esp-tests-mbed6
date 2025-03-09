#include "mbed.h"

#ifndef MOTOR_H
#define MOTOR_H

class Motor {
protected:
    PwmOut pwm;
    DigitalOut bipolar;
    DigitalOut direction;
    float period;
    bool reverse;

public:
    Motor(PinName pwm, PinName bipolar, float period, PinName direction, bool reverse) :
        pwm(pwm), bipolar(bipolar), period(period), direction(direction), reverse(reverse) {
        this->bipolar.write(0);
        this->pwm.period(period);
        this->pwm.write(1);
        if (this->reverse==false){
        this->direction.write(1);  // init forward mode
        }
        else {this->direction.write(0);}
    }

    void stop() {
        pwm.write(1);
    }

    void setSpeed(float speed) {
        pwm.write(1 - speed);
    }

    void switchToForward() {
        direction.write(1);
    }

    void switchToBackward() {
        direction.write(0);
    }
};

class Motors
{
protected:
    Motor left;
    Motor right;

public:
    Motors(Motor left, Motor right):
        left(left),
        right(right)
    {}

    // speed from 0 to 1
    void setSpeed(float speed) {
        left.setSpeed(speed);
        right.setSpeed(speed);
    }

    void turnRight(float speed, int time) {
        left.setSpeed(speed);
        right.stop();
        thread_sleep_for(time);
    }

    void turnLeft(float speed, int time) {
        left.stop();
        right.setSpeed(speed);
        thread_sleep_for(time);
    }

    void turnAround(float speed, int time) {
        right.switchToBackward();
        left.setSpeed(speed);
        right.setSpeed(speed);
        thread_sleep_for(time);
        right.switchToForward();
    }
};

#endif
