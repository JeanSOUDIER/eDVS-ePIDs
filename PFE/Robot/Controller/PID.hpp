#ifndef PID_H
#define PID_H

#include <iostream>
#include <atomic>
#include <chrono>

#include "../BaseThread/BaseThread.hpp"
#include "../eDVS/DVS.hpp"
#include "../MotorWheel/MotorWheel.hpp"
#include "../logger/logger.hpp"

/*

                     PID class

    SOUDIER Jean  (jean.soudier@insa-strasbourg.fr)


    Provides a class for a PID controller

    Features :

    • Compute a PID controller

    Functions :

    • PID                            | Constructor with arguments sampling time, Kp, Ki, Kd, logger, corrector number, N
    • Read()                         | Function to read the potentiometer value
    • ThreadRun()                    | Function to handle the time-based working process
    • ComputePID()                   | Function to compute the PID

*/

class PID : public BaseThread {
	public:
		PID(const unsigned int Te, const float Kp, const float Ki, const float Kd, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const unsigned int nb_corrector, const unsigned int N = 100);
		~PID();

		void Read();
	private:
		void* ThreadRun();
		void ComputePID();

		const unsigned int m_Te;
		const float m_kp;
		const float m_ki;
		const float m_kd;
		const float m_kdN;
		const unsigned int m_N;
		const unsigned int m_nb_corrector;
		
		float m_yOld = 0;
		float m_ui = 0;
		float m_ud = 0;
		float m_uOld = 0;

		MotorWheel *m_Arduino;

		logger *m_log;
		logger *m_logCPU;
		logger *m_logCPUhard;
		
		unsigned int m_cptEvts = 0;
};

#endif //PID_H
