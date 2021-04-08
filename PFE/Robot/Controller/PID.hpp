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
		PID(const unsigned int Te, const float Kp, const float Ki, const float Kd, const unsigned int N = 100, const float beta = 1);
		~PID();
	
		void SetPoint(float sp);
	private:
		void* ThreadRun();
		void ComputePID();

		const unsigned int m_Te;
		const float m_kp;
		const float m_ki;
		const float m_kd;
		const float m_beta;
		const unsigned int m_N;
		
		float m_yOld = 0;
		float m_ui = 0;
		float m_ud = 0;

		DVS* m_eDVS_4337;

		std::atomic<float> m_ysp;

		HardCommand *m_PWM;
		Hbeidge *m_Motor;
		MotorWheel *m_Arduino;

		logger *m_log;
		logger *m_logCPU;
};




#endif //PID_H