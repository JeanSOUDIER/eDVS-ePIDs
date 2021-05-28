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
	PID PIDbille(1, 1, 0.1, 2, begin_timestamp, num, 0, 1000);
    PIDbille.StartThread();

	//g_setpoint[1].store(400);
    //g_event[1].store(true);
	//ePID PIDmot(begin_timestamp, num, 2, 5, 0, 100, 1, 5, 0.001, 10, 10);
	//Te Kp Ki Kd x x x N
	PID PIDmot(1, 2, 5, 0, begin_timestamp, num, 1, 100);
    PIDmot.StartThread();
	//MotorWheel m_Arduino("ttyUSB_Teensy", 115200);

    delay(1000);
	while(!kbhit()) {
    //while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin_timestamp).count() < 2000) {
		/*g_setpoint[1].store(480);
    	g_event[1].store(true);
    	delay(500);
		g_setpoint[1].store(320);
    	g_event[1].store(true);
    	delay(500);*/
    	g_setpoint[0].store(10);
    	g_event[0].store(true);
    	delay(2000);
    	/*delay(5000);
		g_setpoint[0].store(-10);
    	g_event[0].store(true);
    	delay(5000);*/
	}
	l.Tac();
	//g_setpoint[1].store(400);
    //g_event[1].store(true);
    g_setpoint[0].store(0);
    g_event[0].store(true);
    delay(8000);

    PIDmot.StopThread();
    PIDbille.StopThread();
    CamTrack.StopThread();
	delay(100);

    return 0;
}
