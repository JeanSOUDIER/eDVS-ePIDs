#ifndef CPU_USAGE_H
#define CPU_USAGE_H

#include <iostream>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
//#include <thread>
#include <vector>

#if OS == OS_WINDOWS
#include<opencv2/opencv.hpp>
#define delay(x) cv::waitKey(x)
#include <windows.h>
#define MB 1048576
#else
#include <WiringPi.h>

const int NUM_CPU_STATES = 10;

enum CPUStates {
	S_USER = 0,
	S_NICE,
	S_SYSTEM,
	S_IDLE,
	S_IOWAIT,
	S_IRQ,
	S_SOFTIRQ,
	S_STEAL,
	S_GUEST,
	S_GUEST_NICE
};

typedef struct CPUData {
	std::string cpu;
	size_t times[NUM_CPU_STATES];
} CPUData;
#endif

#include "logger.hpp"

class CpuUsage {
	public:
		CpuUsage(int delay);
		~CpuUsage();

		void PrintStats();
	private:
		#if OS == OS_WINDOWS
			SYSTEMTIME m_sysTime;
			MEMORYSTATUSEX m_memStat;
		#else
			void ReadStatsCPU(std::vector<CPUData>& entries);
			std::vector<double> PrintStats(const std::vector<CPUData>& entries1, const std::vector<CPUData>& entries2);
			size_t GetIdleTime(const CPUData& e);
			size_t GetActiveTime(const CPUData& e);

			std::vector<CPUData> m_entry;
		#endif

		logger *m_log;
		int m_delay;
};

#endif // CPU_USAGE_H
