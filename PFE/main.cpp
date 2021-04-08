#include <iostream>
#include <wiringPi.h>
#include <ncurses.h>

#include "Robot/Controller/Hbridge.hpp"
#include "Robot/eDVS/DVS.hpp"
#include "Robot/Controller/ePID.hpp"

bool kbhit() {
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

int main() {
	ePID myPID(1, 0, 0, 200000, 20000, 100, 1, 0.1, new DVS("ttyUSB0", 12000000));
    Hbridge Motor(28, 29, 1); //38, 40
    myPID.StartThread();

    while(!kbhit() && myPID.GetStartValue()) {
        delay(2000);
	    Motor.Set(200);
        myPID.SetPoint(5);
        delay(2000);
	    Motor.Set(-200);
        myPID.SetPoint(-5);
    }

    Motor.Stop();
	myPID.StopThread();

    return 0;
}
