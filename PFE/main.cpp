#include <iostream>
#include <ncurses.h>
#include <wiringPi.h>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <mutex>

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


void two_loop() {
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();
    logger l("Time",begin_timestamp);
    l.Write({ 0, 0 });
    const int num = l.GetNumFile();

	DVS CamTrack("ttyUSB_DVS", 12000000, begin_timestamp, num, 0.1);
    l.Tic();
	CamTrack.StartThread();

	g_setpoint[0].store(0);
    g_event[0].store(true);
    PID PIDbille(1, 0.07735, 0.003288*2, 0.4455*50, begin_timestamp, num, 0, 10.43);
	//ePID PIDbille(begin_timestamp, num, 0.07735, 0.003288, 0.4455, 10.43, 0, 0.1, 1, 10, 10);
    PIDbille.StartThread();
	g_cv[0].notify_one();

    //x x Kp Ki Kd N x e_lim h_nom alphaI alphaD
	ePID PIDmot(begin_timestamp, num, 3, 8, 0.8, 10, 1, 0.1, 1, 10, 10);
	//Te Kp Ki Kd x x x N
	//PID PIDmot(1, 3, 8, 0.8, begin_timestamp, num, 1, 10);
	PIDmot.Read();
	PIDmot.StartThread();

    delay(1000);
	g_setpoint[0].store(-30);
	g_event[0].store(true);
	g_cv[0].notify_one();
	//while(!kbhit()) {
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin_timestamp).count() < 10000) {
    	PIDmot.Read();
    	delay(1);
	}
    PIDmot.Read();
    l.Tac();
    g_setpoint[0].store(0);
    g_event[0].store(true);
	g_cv[0].notify_one();
    for(int i=0;i<1000;i++) {delay(1);PIDmot.Read();}

    PIDbille.StopThread();
    g_event[0].store(true);
    g_cv[0].notify_one();
    for(int i=0;i<100;i++) {delay(1);PIDmot.Read();}

    g_setpoint[1].store(0);
    g_event[1].store(true);
    g_cv[1].notify_one();
    for(int i=0;i<5000;i++) {delay(1);PIDmot.Read();}


    PIDmot.StopThread();
    g_event[1].store(true);
    g_cv[1].notify_one();
    for(int i=0;i<100;i++) {delay(1);PIDmot.Read();}
    CamTrack.StopThread();
	delay(100);
}

void one_loop() {
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();
    logger l("Time",begin_timestamp);
    l.Write({ 0, 0 });
    const int num = l.GetNumFile();

    l.Tic();

	g_setpoint[1].store(0);
    g_event[1].store(true);
    //x x Kp Ki Kd N x e_lim h_nom alphaI alphaD
	//ePID PIDmot(begin_timestamp, num, 3, 8, 0.8, 10, 1, 0.1, 1, 10, 10);
	//Te Kp Ki Kd x x x N
	PID PIDmot(1, 3, 8, 0.8, begin_timestamp, num, 1, 10);
	PIDmot.Read();
	PIDmot.StartThread();
    g_cv[1].notify_one();

    delay(1000);
	//while(!kbhit()) {
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin_timestamp).count() < 5000) {
		g_setpoint[1].store(3.1415);
    	g_event[1].store(true);
	    g_cv[1].notify_one();
    	for(int i=0;i<5000;i++) {delay(1);PIDmot.Read();}
		g_setpoint[1].store(-3.1415);
    	g_event[1].store(true);
	    g_cv[1].notify_one();
    	for(int i=0;i<5000;i++) {delay(1);PIDmot.Read();}
	}
    PIDmot.Read();
    l.Tac();

    g_setpoint[1].store(0);
    g_event[1].store(true);
    g_cv[1].notify_one();
    for(int i=0;i<5000;i++) {delay(1);PIDmot.Read();}


    PIDmot.StopThread();
    g_event[1].store(true);
    g_cv[1].notify_one();
    for(int i=0;i<100;i++) {delay(1);PIDmot.Read();}
}

int main() {
	//MotorWheel m_Arduino("ttyUSB_Teensy", 115200);
    two_loop();
    return 0;
}
