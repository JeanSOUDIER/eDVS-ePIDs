#ifndef PID_H
#define PID_H

#include <iostream>
#include <atomic>
#include <chrono>

#include "../BaseThread/BaseThread.hpp"
#include "../eDVS/DVS.hpp"
#include "HardCommand.hpp"
#include "Hbridge.hpp"
#include "../MotorWheel/MotorWheel.hpp"
#include "../logger/logger.hpp"

class PID : public BaseThread {
	public:
		PID(const unsigned int Te, const float Kp, const float Ki, const float Kd, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const unsigned int nb_corrector, const unsigned int N = 100, const float beta = 1);
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
		const float m_beta;
		const unsigned int m_N;
		const unsigned int m_nb_corrector;
		
		float m_yOld = 0;
		float m_ui = 0;
		float m_ud = 0;
		float m_uOld = 0;

		//Hbridge *m_Motor;
		MotorWheel *m_Arduino;

		logger *m_log;
		logger *m_logCPU;
		logger *m_logCPUhard;
		
		unsigned int m_cptEvts = 0;
};

#endif //PID_H
