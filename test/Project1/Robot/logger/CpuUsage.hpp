#ifndef CPU_USAGE_H
#define CPU_USAGE_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

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

class CpuUsage {
	public:
		CpuUsage();
		~CpuUsage();

		void ReadStatsCPU(std::vector<CPUData> &entries);
		void PrintStats(const std::vector<CPUData> & entries1, const std::vector<CPUData> & entries2);
	private:
		GetIdleTime(const CPUData & e);
		GetActiveTime(const CPUData & e);
};


#endif // CPU_USAGE_H

/*
std::vector<CPUData> entries1;
	std::vector<CPUData> entries2;

	// snapshot 1
	ReadStatsCPU(entries1);

	// 100ms pause
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// snapshot 2
	ReadStatsCPU(entries2);

	// print output
	PrintStats(entries1, entries2);
*/
