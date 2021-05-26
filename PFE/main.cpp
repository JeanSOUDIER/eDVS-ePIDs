#include <iostream>
#include <ncurses.h>
#include <wiringPi.h>
#include <chrono>

#include "Robot/BaseThread/BaseThread.hpp"
#include "Robot/Controller/Hbridge.hpp"
#include "Robot/eDVS/DVS.hpp"
#include "Robot/Controller/ePID.hpp"
#include "Robot/Controller/PID.hpp"
#include "Robot/MotorWheel/MotorWheel.hpp"

bool kbhit() {
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

void SetPointRef(const float sp) {
    g_setpoint[0].store(sp);
    g_event[0].store(true);
}

int main() {
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();
    logger l("Time",begin_timestamp);
    const int num = l.GetNumFile();

	/*DVS myDVS("ttyUSB_DVS", 12000000, begin_timestamp, num);
	myDVS.StartThread();

    ePID myPID(begin_timestamp, num, -0.0990/2, -0.0460/10, -0.0252*100, 1749, 0, 2);
    myPID.StartThread();

    //Hbridge Motor(28, 29); //38, 40
    MotorWheel Motor("ttyUSB_Teensy", 115200);

    while(!kbhit() && g_working.load()) {
        delay(500);
        Motor.SetHbridge(100);
        SetPointRef(5);
        delay(500);
        Motor.SetHbridge(-100);
        SetPointRef(-5);
    }
    g_working.store(false);

    Motor.SetHbridge(-100);
    SetPointRef(-5);
    delay(500);

    Motor.SetHbridge(0);
    SetPointRef(0);
	myPID.StopThread();*/

	g_setpoint[1].store(500);
    g_event[1].store(true);
	//ePID PIDmot(begin_timestamp, num, 0.1, 0, 0, 100, 1, 2, 0.001, 10, 10);
	//Te Kp Ki Kd x x x N
	PID PIDmot(1, 2, 5, 0, begin_timestamp, num, 1, 100);
    PIDmot.StartThread();
	//MotorWheel m_Arduino("ttyUSB_Teensy", 115200);

    delay(1000);
	while(!kbhit()) {
		//std::cout << m_Arduino.ReadPose() << std::endl;
		g_setpoint[1].store(580);
    	g_event[1].store(true);
    	delay(1000);
		g_setpoint[1].store(420);
    	g_event[1].store(true);
    	delay(1000);
	}
	g_setpoint[1].store(400);
    g_event[1].store(true);
    delay(1000);

    PIDmot.StopThread();
	delay(1000);

    return 0;
}
