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
#include "Robot/eDVS/DVS.hpp"
#include "Robot/Controller/ePID.hpp"
#include "Robot/Controller/PID.hpp"
#include "Robot/MotorWheel/MotorWheel.hpp"

#define EVENT_BASED

bool kbhit() { //function to get if a key is pressed without blocking the program
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

void Triggering() { //function to trigger the oscilloscope with an I/O pin
	std::cout << "trig start" << std::endl;
	system("gpio mode 28 out");
	system("gpio write 28 0");
	delay(100);
	system("gpio write 28 1");
	std::cout << "trig end" << std::endl;
}

std::vector<float> ComputeTrajSmooth(float start_point, float end_point, float time, float Ts, float A_max, float V_max) { //function to create a vector for a smooth trajectory
    // float Ts = 0.002;
    // float J_max = 1;
    // float A_max = 3;
    // float V_max = 16;

    float A4 = 35;
    float A5 = -84;
    float A6 = 70;
    float A7 = -20;

    float SP_max = 35/16;

    float SPP_max = 85*std::sqrt(5)/25;

    float delta = std::fabs(std::fabs(start_point)-std::fabs(end_point));

    float T_min_speed = std::fabs(delta)*SP_max/V_max;
    float T_min_acc = std::sqrt(std::fabs(delta)*SPP_max/A_max);

    float T = std::max(T_min_speed,T_min_acc);

    float sens = 1;
    if(end_point < start_point) {sens = -1;}

    std::vector<float> t, traj, s;
    for(int i=0;i<static_cast<int>(time/Ts);i++) {
        t.push_back(i*Ts);
        s.push_back(A4*std::pow(t.at(i)/T,4) + A5*std::pow(t.at(i)/T,5) + A6*std::pow(t.at(i)/T,6) + A7*std::pow(t.at(i)/T,7));
        traj.push_back(start_point+sens*delta*s.at(i));
    }

    return traj;
}

void two_loop() { //function with the two controllers
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp = std::chrono::high_resolution_clock::now();
    logger l("Time",begin_timestamp);
    l.Write({ 0, 0 });
    const int num = l.GetNumFile();

    const float Kp_ball = 0.07735;
    const float Ki_ball = 0.003288;
    const float Kd_ball = 0.4455*50;
    const float N_ball = 10.43;
    const float Te_ball = 20;
    const float ai_ball = 100000;
    const float ad_ball = 100000;
    const float fact_ball = 1;
    #ifdef EVENT_BASED
        const float elim_ball = 3;
    #else
        const float elim_ball = 0;
    #endif
    
    const float Kp_motor = 6.54;
    const float Ki_motor = 19.72;
    const float Kd_motor = 0.20;
    const float N_motor = 364.15;
    const float Te_motor = 1;
    const float ai_motor = 1000000;
    const float ad_motor = 1000000;
    const float fact_motor = 5;
    #ifdef EVENT_BASED
        const float elim_motor = 0.7;
    #else
        const float elim_motor = 0;
    #endif

    std::ofstream param_file = std::ofstream("files/param_"+std::to_string(num)+".txt");
    param_file << "Ball : Kp " << Kp_ball << ",Ki " << Ki_ball << ",Kd " << Kd_ball << ",N " << N_ball << ",Te " << Te_ball << ",ai " << ai_ball << ",ad " << ad_ball << ",elim " << elim_ball << ",fact " << fact_ball << "\n";
    param_file << "Motor : Kp " << Kp_motor << ",Ki " << Ki_motor << ",Kd " << Kd_motor << ",N " << N_motor << ",Te " << Te_motor << ",ai " << ai_motor << ",ad " << ad_motor << ",elim " << elim_motor << ",fact " << fact_motor << "\n";
    param_file.close();

    std::vector<float> risedown = ComputeTrajSmooth(0, -30, 4, 0.002, 12, 26);
    std::vector<float> riseup = ComputeTrajSmooth(-30, 0, 4, 0.002, 12, 26);

	DVS CamTrack("ttyUSB_DVS", 12000000, begin_timestamp, num, elim_ball);
    l.Tic();
    Triggering();
	CamTrack.StartThread();

	g_setpoint[0].store(0);
    #ifdef EVENT_BASED
        ePID PIDbille(begin_timestamp, num, Kp_ball, Ki_ball, Kd_ball, N_ball, 0, elim_ball, Te_ball, ai_ball, ad_ball, fact_ball);
    #else
        PID PIDbille(20, Kp_ball, Ki_ball, Kd_ball, begin_timestamp, num, 0, N_ball);
    #endif

    PIDbille.StartThread();

    #ifdef EVENT_BASED
    	if(!g_event[0].load()) {
    		g_event[0].store(true);
        	g_cv[0].notify_one();
    	}
    #endif

    #ifdef EVENT_BASED
        ePID PIDmot(begin_timestamp, num, Kp_motor, Ki_motor, Kd_motor, N_motor, 1, elim_motor, Te_motor, ai_motor, ad_motor, fact_motor);
    #else
        PID PIDmot(Te_motor, Kp_motor, Ki_motor, Kd_motor, begin_timestamp, num, 1, N_motor);
    #endif
	PIDmot.Read();
	PIDmot.StartThread();

    /*for(int i=0;i<1000;i++) {delay(1);PIDmot.Read();}
	g_setpoint[0].store(-30);
	if(!g_event[0].load()) {
		g_event[0].store(true);
    	g_cv[0].notify_one();
	}*/
	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> start1;
	//while(!kbhit()) {
    /*while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < 6000) {
    	PIDmot.Read();
	}*/
    unsigned int cpt = 0;
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() < 17000) {
        start1 = std::chrono::high_resolution_clock::now();

        g_setpoint[0].store(risedown.at(cpt));
        #ifdef EVENT_BASED
            if(std::fabs(risedown.at(cpt)-g_feedback[0].load()) > elim_ball) {
                if(!g_event[0].load()) {
                    g_event[0].store(true);
                    g_cv[0].notify_one();
                }
            }
        #endif

        cpt++;
        if(cpt >= risedown.size()) {cpt--;}

        while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start1).count() < 2) {
            PIDmot.Read();
        }
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
	delay(100);
}

void one_loop() { //function with the motor controller
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
