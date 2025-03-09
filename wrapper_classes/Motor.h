#include "QEI.h"
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
    Motor(PinName pwm, PinName bipolar, float period, PinName direction,
                bool reverse)
            : pwm(pwm), bipolar(bipolar), period(period), direction(direction),
                reverse(reverse) {
        this->bipolar.write(0);
        this->pwm.period(period);
        this->pwm.write(1);
        if (this->reverse == false) {
            this->direction.write(1); // init forward mode
        } else {
            this->direction.write(0);
        }
    }

    void stop() { pwm.write(1); }

    void setSpeed(float speed) { pwm.write(1 - speed); }

    void switchToForward() { direction.write(1); }

    void switchToBackward() { direction.write(0); }
};

class Motors {
protected:
    Motor &left;
    Motor &right;
    QEI &encL;
    QEI &encR;

public:
    Motors(Motor &left, Motor &right, QEI &encL, QEI &encR)
            : left(left), right(right), encL(encL), encR(encR) {}

    void resetEncoders(void) {
        encL.reset();
        encR.reset();
    }

    void setSpeed(float speed) {
        left.setSpeed(speed);
        right.setSpeed(speed);
    }

    void goStraight(float targetSpeed, int pulseTarget) {
        float Kp = 0.04; // Proportional gain
        this->resetEncoders();
        while (encR.getPulses() <= pulseTarget) {
            int prevPulseL = encL.getPulses();
            int prevPulseR = encR.getPulses();

            thread_sleep_for(100);

            int currPulseL = encL.getPulses();
            int currPulseR = encR.getPulses();

            float speedL = (currPulseL - prevPulseL) / (512 * (0.1 / 60)); // RPM
            float speedR = (currPulseR - prevPulseR) / (512 * (0.1 / 60)); // RPM

            float leftSpeedAdjust = 1.0f;
            float rightSpeedAdjust = 1.0f;

            float error = speedL - speedR;

            leftSpeedAdjust -= Kp * error;
            rightSpeedAdjust += Kp * error;

            leftSpeedAdjust = std::max(0.0f, std::min(leftSpeedAdjust, 1.0f));
            rightSpeedAdjust = std::max(0.0f, std::min(rightSpeedAdjust, 1.0f));

            left.setSpeed(targetSpeed * leftSpeedAdjust);
            right.setSpeed(targetSpeed * rightSpeedAdjust);
        }
        this->setSpeed(0.0f);
        while (encL.getPulses() <= encR.getPulses()) {
            left.setSpeed(targetSpeed);
        }
        this->setSpeed(0.0f);
    }

    void turnRight(float speed, float pulsetarget) {
        this->resetEncoders();
        while (encL.getPulses() <= pulsetarget) {
            left.setSpeed(speed);
            right.stop();
        }
        this->setSpeed(0.0f);
    }

    void turnLeft(float speed, float pulsetarget) {
        this->resetEncoders();
        while (encR.getPulses() <= pulsetarget) {
            left.stop();
            right.setSpeed(speed);
        }
        this->setSpeed(0.0f);
    }

    void turnAround(float speed, float pulsetarget) {
        this->resetEncoders();
        left.switchToBackward();
        while (encR.getPulses() <= pulsetarget) {
            left.setSpeed(speed);
            right.setSpeed(speed);
        }
        left.switchToForward();
        this->setSpeed(0.0f);
    }
};

#endif
