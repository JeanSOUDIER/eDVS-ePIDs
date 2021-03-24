#ifndef EPID_H
#define EPID_H

#include <atomic>

#include "../BaseThread/BaseThread.hpp"
#include "../eDVS/DVS.hpp"
#include "HardCommand.hpp"
#include "../MotorWheel/MotorWheel.hpp"
#include "../logger/logger.hpp"

class ePID : public BaseThread {
	public:
		ePID(float Kp, float Ki, float Kd, float hmax = 200000, float hnom = 20000, unsigned int N = 100, float gamma = 0, float beta = 1, DVS* eDVS_4337 = NULL);
		~ePID();

		void SetPoint(float sp);
	private:
		void* ThreadRun();
		void ComputePID();

		float m_kp;
		float m_ki;
		float m_kd;
		float m_gamma;
		float m_beta;
		float m_hmax;
		float m_hnom;
		unsigned int m_N;

		float m_yOld = 0;
		unsigned long m_lastT = 0;
		float m_ui = 0;
		float m_ud = 0;
		float m_elim;

		DVS* eDVS_4337;

		std::atomic<float> m_ysp;

		DVS* m_eDVS_4337;

		HardCommand *m_PWM;
		MotorWheel *m_Arduino;

		logger *m_log;
		logger *m_logCPU;
};

#endif //EPID_H
