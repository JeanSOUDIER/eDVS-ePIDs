#include <iostream>
#include <ncurses.h>
#include <wiringPi.h>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <exception>

#include "Robot/BaseThread/BaseThread.hpp"
#include "Robot/Controller/Hbridge.hpp"
#include "Robot/eDVS/DVS.hpp"
#include "Robot/Controller/ePID.hpp"
#include "Robot/Controller/PID.hpp"
#include "Robot/MotorWheel/MotorWheel.hpp"
#include "Robot/GTsensor/VL53L0X.hpp"

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
    //logger GTsensor("GTsensor", begin_timestamp, num);

    //VL53L0X sensor(begin_timestamp, num);
	DVS CamTrack("ttyUSB_DVS", 12000000, begin_timestamp, num, 3);
    l.Tic();
    //sensor.StartThread();
	CamTrack.StartThread();

	g_setpoint[0].store(0);
    //PID PIDbille(20, 0.07735, 0.003288, 0.4455*50, begin_timestamp, num, 0, 10.43);
	ePID PIDbille(begin_timestamp, num, 0.07735, 0.003288, 0.4455*50, 10.43, 0, 3, 20, 100000, 100000, 5);
    PIDbille.StartThread();
	if(!g_event[0].load()) {
		g_event[0].store(true);
    	g_cv[0].notify_one();
	}

	//Te Kp Ki Kd x x x N
	//PID PIDmot(1, 6.54, 19.72, 0.20, begin_timestamp, num, 1, 364.15);
    //x x Kp Ki Kd N x e_lim h_nom alphaI alphaD fact
    ePID PIDmot(begin_timestamp, num, 6.54, 19.72, 0.20, 364.15, 1, 0.7, 1, 1000000, 1000000, 5);
	PIDmot.Read();
	PIDmot.StartThread();

    for(int i=0;i<1000;i++) {delay(1);PIDmot.Read();}
	g_setpoint[0].store(-30);
	if(!g_event[0].load()) {
		g_event[0].store(true);
    	g_cv[0].notify_one();
	}
	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
	//while(!kbhit()) {
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < 6000) {
    	PIDmot.Read();
	}
    PIDmot.Read();
    l.Tac();
    std::cout << "end loop" << std::endl;
    /*g_setpoint[0].store(0);
    if(!g_event[0].load()) {
		g_event[0].store(true);
    	g_cv[0].notify_one();
	}
    for(int i=0;i<1000;i++) {delay(1);PIDmot.Read();}*/

    PIDbille.StopThread();
    g_event[0].store(true);
    g_cv[0].notify_one();
    for(int i=0;i<10;i++) {delay(1);PIDmot.Read();}

    g_setpoint[1].store(0);
    if(!g_event[1].load()) {
		g_event[1].store(true);
    	g_cv[1].notify_one();
	}
    for(int i=0;i<5000;i++) {delay(1);PIDmot.Read();}


    PIDmot.StopThread();
    g_event[1].store(true);
    g_cv[1].notify_one();
    for(int i=0;i<100;i++) {delay(1);PIDmot.Read();}
    CamTrack.StopThread();
    //sensor.StopThread();
	delay(100);
}

void one_loop() {
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();
    logger l("Time",begin_timestamp);
    l.Write({ 0, 0 });
    const int num = l.GetNumFile();

    l.Tic();

	g_setpoint[1].store(0);
    //x x Kp Ki Kd N x e_lim h_nom alphaI alphaD fact
	//ePID PIDmot(begin_timestamp, num, 1.31, 3.94, 0.0395, 364.15, 1, 0.5, 1, 1000000, 1000000);
    ePID PIDmot(begin_timestamp, num, 6.54, 19.72, 0.20, 364.15, 1, 0.5, 1, 1000000, 1000000, 10);
	//Te Kp Ki Kd x x x N
	//PID PIDmot(1, 1.31, 3.94, 0.0395, begin_timestamp, num, 1, 364.15);
	PIDmot.Read();
	PIDmot.StartThread();
	if(!g_event[1].load()) {
		g_event[1].store(true);
    	g_cv[1].notify_one();
	}

    for(int i=0;i<1000;i++) {delay(1);PIDmot.Read();}
    std::chrono::time_point<std::chrono::high_resolution_clock> start1 = std::chrono::high_resolution_clock::now();
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	//while(!kbhit()) {
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start1).count() < 35000) {
		start = std::chrono::high_resolution_clock::now();
    	g_setpoint[1].store(3.1415);
    	if(!g_event[1].load()) {
			g_event[1].store(true);
	    	g_cv[1].notify_one();
		}
    	while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < 5000) {PIDmot.Read();}

		start = std::chrono::high_resolution_clock::now();
    	g_setpoint[1].store(-3.1415);
    	if(!g_event[1].load()) {
			g_event[1].store(true);
	    	g_cv[1].notify_one();
		}
    	while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < 5000) {PIDmot.Read();}

	}
    PIDmot.Read();
    l.Tac();

    g_setpoint[1].store(0);
    if(!g_event[1].load()) {
		g_event[1].store(true);
    	g_cv[1].notify_one();
	}
    for(int i=0;i<5000;i++) {delay(1);PIDmot.Read();}


    PIDmot.StopThread();
    g_event[1].store(true);
    g_cv[1].notify_one();
    for(int i=0;i<100;i++) {delay(1);PIDmot.Read();}
}

int main() {
	//MotorWheel m_Arduino("ttyUSB_Teensy", 115200);
	//one_loop();
    two_loop();

    // detection DV1(30,0,105,80,0,122);
    // Pos=DV1.LirePosBille();
    return 0;
}
