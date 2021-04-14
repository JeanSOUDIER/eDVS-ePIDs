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

int main() {
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();

	ePID myPID(begin_timestamp, 1, 0, 0, 200000, 20000, 100, 1, 0.1, new DVS("ttyUSB0", 12000000, begin_timestamp));
    Hbridge Motor(28, 29); //38, 40
    myPID.StartThread();

    Motor.Set(-500);
    while(!kbhit() && myPID.GetStartValue()) {
        delay(2000);
	    Motor.Set(1000);
        myPID.SetPoint(5);
        delay(2000);
	    Motor.Set(-1000);
        myPID.SetPoint(-5);
    }
    Motor.Set(-500);
    delay(1000);

    Motor.Stop();
	myPID.StopThread();

    return 0;
}
