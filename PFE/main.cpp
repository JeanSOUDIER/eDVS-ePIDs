#include <iostream>
#include <wiringPi.h>
#include <ncurses.h>
#include <chrono>

#include "Robot/Controller/Hbridge.hpp"
#include "Robot/eDVS/DVS.hpp"
#include "Robot/Controller/ePID.hpp"

bool kbhit() {
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

#include "Robot/MotorWheel/MotorWheel.hpp"

int main() {
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();
    logger l("Time",begin_timestamp);
    const int num = l.GetNumFile();

    ePID myPID(begin_timestamp, num, -0.0990/2, -0.0460/10, -0.0252*100, 1749, new DVS("ttyUSB_DVS", 12000000, begin_timestamp, num));
    //Hbridge Motor(28, 29); //38, 40
    MotorWheel Motor("ttyUSB_Teensy", 115200);
    myPID.StartThread();

    while(!kbhit() && myPID.GetStartValue()) {
        delay(500);
        Motor.SetHbridge(100);
        myPID.SetPoint(5);
        delay(500);
        Motor.SetHbridge(-100);
        myPID.SetPoint(-5);
    }

    Motor.SetHbridge(-100);
    myPID.SetPoint(5);
    delay(500);

    Motor.SetHbridge(0);
	myPID.StopThread();

    return 0;
}
