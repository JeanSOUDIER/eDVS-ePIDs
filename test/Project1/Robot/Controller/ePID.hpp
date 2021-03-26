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
		ePID(const float Kp, const float Ki, const float Kd, const float hmax = 200000, const float hnom = 20000, const unsigned int N = 100, const float gamma = 0, const float beta = 1, DVS* eDVS_4337 = NULL);
		~ePID();

		void SetPoint(float sp);
	private:
		void* ThreadRun();
		void ComputePID();

		const float *m_kp;
		const float *m_ki;
		const float *m_kd;
		const float *m_gamma;
		const float *m_beta;
		const float *m_hmax;
		const float *m_hnom;
		const unsigned int *m_N;

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
