#include <iostream>
#include <ncurses.h>
#include <wiringPi.h>
#include <chrono>
#include <atomic>

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

int main() {
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();
    logger l("Time",begin_timestamp);
    l.Write({ 0, 0 });
    const int num = l.GetNumFile();

	DVS CamTrack("ttyUSB_DVS", 12000000, begin_timestamp, num);
    l.Tic();
	CamTrack.StartThread();

	g_setpoint[0].store(0);
    g_event[0].store(true);
	PID PIDbille(1, 0.004217*15, 0.0001974*15, 0.02212*15, begin_timestamp, num, 0, 5.454);
	//ePID PIDbille(begin_timestamp, num, 0.004217*30, 0.0001974*30, 0.02212*30, 5.454, 0, 2.5);
    PIDbille.StartThread();

	//g_setpoint[1].store(0);
    //g_event[1].store(true);
	//ePID PIDmot(begin_timestamp, num, 2, 5, 0, 100, 1, 5, 0.001, 10, 10);
	//Te Kp Ki Kd x x x N
	PID PIDmot(1, 3, 20.5, 0, begin_timestamp, num, 1, 30, 0);
    PIDmot.StartThread();
	//MotorWheel m_Arduino("ttyUSB_Teensy", 115200);

    delay(1000);
	g_setpoint[0].store(-20);
	g_event[0].store(true);
	while(!kbhit()) {
    //while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin_timestamp).count() < 5000) {
		/*g_setpoint[1].store(3.1415);
    	g_event[1].store(true);
    	delay(5000);
		g_setpoint[1].store(-3.1415);
    	g_event[1].store(true);
    	delay(5000);*/
	}
    l.Tac();
    g_setpoint[0].store(0);
    g_event[0].store(true);
    delay(1000);

    PIDbille.StopThread();
    delay(100);

    g_setpoint[1].store(0);
    g_event[1].store(true);
    delay(5000);


    PIDmot.StopThread();
    delay(100);
    CamTrack.StopThread();
	delay(100);

    return 0;
}
