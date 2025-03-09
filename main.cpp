#include "mbed.h"
#include "C12832.h"
#include "QEI.h"

#include "LED.h"
#include "Menu.h"
#include "Motor.h"
#include "Potentiometer.h"
#include "Sensor.h"

C12832 lcd(D11, D13, D12, D7, D10);
Menu menu(lcd);
SamplingPotentiometer leftHand(A0, 3.3, 20);
SamplingPotentiometer rightHand(A1, 3.3, 20);
InterruptIn buttonUp(A2), buttonDown(A3), buttonFire(D4);
BufferedSerial hm10(PA_11, PA_12, 9600), pc(USBTX, USBRX, 9600);
LED redLED(D5);
QEI encL(PC_2, PC_3, NC, 512), encR(PB_14, PB_13, NC, 512);
DigitalOut enableMotors(PA_13);
Motor leftMotor(PC_8, PC_12, 0.005f, PB_1, false);
Motor rightMotor(PC_6, PC_10, 0.005f, PB_15, true);
Motors motors(leftMotor, rightMotor, encL, encR);
SamplingSensor sensor1(PC_3, 3.3, PC_11, 10);
SamplingSensor sensor2(PC_2, 3.3, PD_2, 10);
SamplingSensor sensor3(PC_5, 3.3, PA_14, 10);
SamplingSensor sensor4(PB_1, 3.3, PB_7, 10);
SamplingSensor sensor5(PC_5, 3.3, PA_14, 10);

// Interrupt Handlers
void onButtonUp() { menu.moveUp(); }
void onButtonDown() { menu.moveDown(); }
void onButtonFire() { menu.select(); }

// define tests here
void runMotorsTest() {
    float leftSpeed = 0, rightSpeed = 0;
    while (!menu.exitRequested()) {
        leftSpeed = leftHand.getCurrentSampleNorm();  // get pot value ranging from 0 to 1
        rightSpeed = rightHand.getCurrentSampleNorm();  // get pot value ranging from 0 to 1

        leftMotor.setSpeed(leftSpeed);
        rightMotor.setSpeed(rightSpeed);

        lcd.locate(0,10);
        lcd.printf("leftSpeed: %.2f", leftSpeed);

        lcd.locate(0,20);
        lcd.printf("rightSpeed: %.2f", rightSpeed);

        thread_sleep_for(100);
    }
}

void runBluetoothTest() {
    char buf[32] = {0};
    uint32_t num = 0;

    lcd.locate(0, 0);
    lcd.printf("Waiting for Bluetooth data..");

    while (!menu.exitRequested()) {
        if (hm10.readable()) {
            num = hm10.read(buf, sizeof(buf));
            pc.write(buf, num);
            lcd.write(buf, num);
            if (buf[0]=='r') { redLED.toggle(); }
        }
        thread_sleep_for(100);
    }
}

void runEncodersTest() {
    int prevPulseL, prevPulseR, currPulseL, currPulseR;
    float speedL, speedR, distanceL, distanceR;

    float SampleTime = 0.1;

    while (!menu.exitRequested()) {
        prevPulseL = encL.getPulses();
        prevPulseR = encR.getPulses();
                
        thread_sleep_for(SampleTime*1000);

        currPulseL = encL.getPulses();
        currPulseR = encR.getPulses();

        speedL = (currPulseL - prevPulseL) / (512 * (SampleTime / 60)); // RPM
        speedR = (currPulseR - prevPulseR) / (512 * (SampleTime / 60)); // RPM

        distanceL = currPulseL / 20.9049;
        distanceR = currPulseR / 20.9049;
 
        lcd.locate(34,1);
        lcd.printf("R");
        lcd.locate(64,1);
        lcd.printf("L");
        lcd.locate(94,1);
        lcd.printf("Distance");

        lcd.locate(1,10);
        lcd.printf("Pulses:");
        lcd.locate(1,22);
        lcd.printf("RPM:");

        lcd.locate(34,10);
        lcd.printf("%i", currPulseR);
        lcd.locate(34,22);
        lcd.printf("%.2f", speedR);
        lcd.locate(64,10);
        lcd.printf("%i", currPulseL);
        lcd.locate(64,22);
        lcd.printf("%.2f", speedL);

        // distance
        lcd.locate(94,10);
        lcd.printf("%.2f", distanceL);
        lcd.locate(94,22);
        lcd.printf("%.2f", distanceR);
    }
}

void runSensorsTest() {
    while (!menu.exitRequested()) {
        // lcd.locate(0,0);
        // lcd.printf("s1: %02f", sensor1.getCurrentSampleNorm());
        // lcd.locate(0,10);
        // lcd.printf("s2: %02f", sensor2.getCurrentSampleNorm());
        // lcd.locate(0,20);
        // lcd.printf("s3: %02f", sensor3.getCurrentSampleNorm());
        lcd.locate(57,0);
        lcd.printf("s4: %02f", sensor4.getCurrentSampleNorm());
        // lcd.locate(57,10);
        // lcd.printf("s5: %02f", sensor5.getCurrentSampleNorm());
        thread_sleep_for(500);  
    }
}

void runSquareTest() {
    float SampleTime = 0.1;
    float speed = 0.25;
    const int targetPulsesStraight = 700;
    const int targetPulsesTurnRight=570;
    const int targetPulsesTurnLeft=550;
    const int targetPulsesTurnAround=620;
    bool flag = true;

    for (int i = 0; i < 4; i++) {
        motors.goStraight(speed,targetPulsesStraight);
        thread_sleep_for(100);
        if (i < 3) { 
            motors.turnRight(speed, targetPulsesTurnRight);
        }
    }
    thread_sleep_for(300);
    motors.turnAround(speed, targetPulsesTurnAround);
    thread_sleep_for(200);
    for (int i = 0; i < 4; i++) {
        motors.goStraight(speed,targetPulsesStraight);
        thread_sleep_for(100);
        if (i < 3) {
            motors.turnLeft(speed, targetPulsesTurnLeft);
            thread_sleep_for(100);
        }
    }
    motors.setSpeed(0);
}

int main() {
    buttonUp.rise(&onButtonUp);
    buttonDown.rise(&onButtonDown);
    buttonFire.rise(&onButtonFire);

    enableMotors.write(1);

    // add more tests here
    menu.addMenuItem("Motors Test (TD1.1)", runMotorsTest);
    menu.addMenuItem("Encoders Test (TD1.3)", runEncodersTest);
    menu.addMenuItem("Square Test (TD1.6)", runSquareTest);
    menu.addMenuItem("Sensors Test (TD2.2)", runSensorsTest);
    menu.addMenuItem("Bluetooth Test (TD2.6)", runBluetoothTest);

    menu.run();
}