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

int main() {
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();
    logger l("Time",begin_timestamp);
    l.Write({ 0, 0 });
    const int num = l.GetNumFile();

	//DVS CamTrack("ttyUSB_DVS", 12000000, begin_timestamp, num, 0);
    l.Tic();
	/*CamTrack.StartThread();

	g_setpoint[0].store(0);
    g_event[0].store(true);
	//PID PIDbille(1, 0.07735/coeff, 0.003288/coeff, 0.4455*200, begin_timestamp, num, 0, 10.43);
    PID PIDbille(1, 0.07735, 0.003288*2, 0.4455*50, begin_timestamp, num, 0, 10.43);
	//ePID PIDbille(begin_timestamp, num, 0.004217*30, 0.0001974*30, 0.02212*30, 5.454, 0, 2.5);
    PIDbille.StartThread();*/

	g_setpoint[1].store(0);
    g_event[1].store(true);
	//Te Kp Ki Kd x x x N
	ePID PIDmot(begin_timestamp, num, 3, 10, 0.8, 10, 1, 0.4, 1, 10, 10);
	//PID PIDmot(1, 3, 15, 0.8, begin_timestamp, num, 1, 10);
	PIDmot.Read();
	PIDmot.StartThread();
    g_cv[1].notify_one();
	//MotorWheel m_Arduino("ttyUSB_Teensy", 115200);

    /*delay(1000);
	g_setpoint[0].store(-30);
	g_event[0].store(true);*/
	//while(!kbhit()) {
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin_timestamp).count() < 5000) {
		g_setpoint[1].store(3.1415);
    	g_event[1].store(true);
	    g_cv[1].notify_one();
    	//delay(5000);
    	for(int i=0;i<5000;i++) {delay(1);PIDmot.Read();}
		g_setpoint[1].store(-3.1415);
    	g_event[1].store(true);
	    g_cv[1].notify_one();
    	//delay(5000);
    	for(int i=0;i<5000;i++) {delay(1);PIDmot.Read();}
    	/*PIDmot.Read();
    	delay(1);*/
		/*g_setpoint[0].store(-30);
		g_event[0].store(true);
		delay(20000);
		g_setpoint[0].store(30);
		g_event[0].store(true);
		delay(20000);*/
	}
    PIDmot.Read();
    l.Tac();
    /*g_setpoint[0].store(0);
    g_event[0].store(true);
    for(int i=0;i<1000;i++) {delay(1);PIDmot.Read();}

    PIDbille.StopThread();
    for(int i=0;i<100;i++) {delay(1);PIDmot.Read();}*/

    g_setpoint[1].store(0);
    g_event[1].store(true);
    g_cv[1].notify_one();
    //delay(1000);
    for(int i=0;i<5000;i++) {delay(1);PIDmot.Read();}


    PIDmot.StopThread();
    g_event[1].store(true);
    g_cv[1].notify_one();
    for(int i=0;i<100;i++) {delay(1);PIDmot.Read();}
    //CamTrack.StopThread();
	//delay(100);

    return 0;
}
