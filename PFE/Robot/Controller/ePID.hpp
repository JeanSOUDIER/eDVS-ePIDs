#ifndef EPID_H
#define EPID_H

#include <iostream>
#include <atomic>
#include <chrono>

#include "../BaseThread/BaseThread.hpp"
#include "../eDVS/DVS.hpp"
#include "HardCommand.hpp"
#include "Hbridge.hpp"
#include "../MotorWheel/MotorWheel.hpp"
#include "../logger/logger.hpp"

#include "look_up_table.hpp"

class ePID : public BaseThread {
	public:
		ePID(std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float Kp, const float Ki, const float Kd, const float N, DVS* eDVS_4337 = NULL, const float hnom = 1000, const float alpha_i = 10, const float alpha_d = 10);
		~ePID();

		void SetPoint(float sp);
	private:
		void* ThreadRun();
		void ComputePID();

		const float m_kp;
		const float m_ki;
		const float m_kdN;
		const float m_alpha_i;
		const float m_alpha_d;
		const float m_hnom;

		float m_eOld = 0;
		float m_yOld = 0;
		unsigned long m_lastT = 0;
		float m_ui = 0;
		float m_ud = 0;
		float m_elim;

		DVS* m_eDVS_4337;

		/*Hbridge *m_Motor;
		HardCommand *m_PWM;
		MotorWheel *m_Arduino;*/

		logger *m_log;
		logger *m_logCPU;
		logger *m_logCPUhard;
		
		unsigned int m_cptEvts = 0;
};

#endif //EPID_H
