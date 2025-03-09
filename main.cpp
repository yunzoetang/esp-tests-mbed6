#include "mbed.h"
#include "C12832.h"
#include "Potentiometer.h"
#include "LED.h"
#include "QEI.h"
#include "Menu.h"
#include "Sensor.h"

C12832 lcd(D11, D13, D12, D7, D10);
Menu menu(lcd);
SamplingPotentiometer leftHand(A0, 3.3, 20);
SamplingPotentiometer rightHand(A1, 3.3, 20);
InterruptIn buttonUp(A2), buttonDown(A3), buttonFire(D4);
BufferedSerial hm10(PA_11, PA_12, 9600), pc(USBTX, USBRX, 9600);
LED redLED(D5);
QEI encL(PC_2, PC_3, NC, 512), encR(PB_14, PB_13, NC, 512);
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

        lcd.locate(0,10);
        lcd.printf("leftSpeed: %02f", leftSpeed);

        lcd.locate(0,20);
        lcd.printf("rightSpeed: %02f", rightSpeed);

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
                
        thread_sleep_for(100);

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

void runTextLocater() {
    char buf[32] = {0};
    uint32_t num = 0;
    int x = 0, y = 0, pre_x = 0, pre_y = 0;

    while (!menu.exitRequested()) {
        if (hm10.readable()) {
            num = hm10.read(buf, sizeof(buf));
            lcd.locate(x,y);
            lcd.write(buf, num);
        }

        x = leftHand.getCurrentSampleNorm() * 128.9;
        y = rightHand.getCurrentSampleNorm() * 22.9;

        if ((pre_x != x) || (pre_y != y)) {

            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("%03d, %02d", x, y);

            lcd.locate(x,y);
            lcd.write(buf, num);
        }

        pre_x = leftHand.getCurrentSampleNorm() * 128.9;
        pre_y = rightHand.getCurrentSampleNorm() * 30.9;

        thread_sleep_for(100);
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

int main() {
    buttonUp.rise(&onButtonUp);
    buttonDown.rise(&onButtonDown);
    buttonFire.rise(&onButtonFire);

    // add more tests here
    menu.addMenuItem("Motors Test", runMotorsTest);
    menu.addMenuItem("Bluetooth Test", runBluetoothTest);
    menu.addMenuItem("Encoders Test", runEncodersTest);
    menu.addMenuItem("Sensors Test", runSensorsTest);
    menu.addMenuItem("Text Locater", runTextLocater);

    menu.run();
}