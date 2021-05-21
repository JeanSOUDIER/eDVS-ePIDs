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
	MotorWheel blink("ttyUSB_Teensy", 115200);

    for(int i=0;i<10;i++) {
        blink.SetHbridge(100);
        delay(500);
        std::cout << "change sens" << std::endl;
        blink.SetHbridge(-100);
        delay(500);
    }
    blink.SetHbridge(0);
	

    /*std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();
    logger l("Time",begin_timestamp);
    const int num = l.GetNumFile();

    ePID myPID(begin_timestamp, num, -0.0990/2, -0.0460/10, -0.0252*100, 1749, new DVS("ttyUSB_DVS", 12000000, begin_timestamp, num));
    Hbridge Motor(28, 29); //38, 40
    myPID.StartThread();

    Motor.Set(-500);
    while(!kbhit() && myPID.GetStartValue()) {
        delay(1500);
	    Motor.Set(1000);
        myPID.SetPoint(5);
        delay(1500);
	    Motor.Set(-1000);
        myPID.SetPoint(-5);
    }
    Motor.Set(-600);
    delay(1000);

    Motor.Stop();
	myPID.StopThread();*/

    return 0;
}
