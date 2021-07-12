#ifndef EPID_H
#define EPID_H

#include <iostream>
#include <atomic>
#include <chrono>
#include <string>
#include <condition_variable>
#include <thread>
#include <mutex>

#include "../BaseThread/BaseThread.hpp"
#include "../eDVS/DVS.hpp"
#include "HardCommand.hpp"
#include "Hbridge.hpp"
#include "../MotorWheel/MotorWheel.hpp"
#include "../logger/logger.hpp"

#include "look_up_table.hpp"

class ePID : public BaseThread {
	public:
		ePID(std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float Kp, const float Ki, const float Kd, const float N, const unsigned int nb_corrector, const float e_lim, const float hnom = 1000, const float alpha_i = 10, const float alpha_d = 10, const float h_nom_fact = 1);
		~ePID();

		void Read();
	private:
		void* ThreadRun();
		void ComputePID();

		const float m_kp;
		const float m_ki;
		const float m_kdN;
		const float m_alpha_i;
		const float m_alpha_d;
		const float m_hnom;
		const float m_hnom_fact;
		const unsigned int m_nb_corrector;
		const float m_elim;

		float m_yOld = 0;
		float m_lastT = 0;
		float m_ui = 0;
		float m_ud = 0;

		MotorWheel *m_Arduino;

		logger *m_log;
		logger *m_logCPU;
		logger *m_logCPUhard;
		
		unsigned int m_cptEvts = 0;
		const std::chrono::time_point<std::chrono::high_resolution_clock> m_begin_timestamp;
};

#endif //EPID_H
