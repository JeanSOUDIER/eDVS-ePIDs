#include "CpuUsage.hpp"

CpuUsage::CpuUsage(int delay) {
	m_delay = delay;
	#if OS == WINDOWS
		m_memStat.dwLength = sizeof(m_memStat);
		GlobalMemoryStatusEx(&m_memStat);

		//Retrieves data so that we have a way to Get it to output when using the pointers
		GetSystemTime(&m_sysTime);

		m_log = new logger("CPU_usage");
	#else
		ReadStatsCPU(m_entry);
	#endif
}

CpuUsage::~CpuUsage() {
	delete m_log;
}

#if OS ~= OS_WINDOWS
size_t CpuUsage::GetIdleTime(const CPUData & e) {
	return	e.times[S_IDLE] + 
			e.times[S_IOWAIT];
}

size_t CpuUsage::GetActiveTime(const CPUData & e) {
	return	e.times[S_USER] +
			e.times[S_NICE] +
			e.times[S_SYSTEM] +
			e.times[S_IRQ] +
			e.times[S_SOFTIRQ] +
			e.times[S_STEAL] +
			e.times[S_GUEST] +
			e.times[S_GUEST_NICE];
}

std::vector<double> CpuUsage::PrintStats(const std::vector<CPUData>& entries1, const std::vector<CPUData>& entries2) {
	const size_t NUM_ENTRIES = entries1.size();
	std::vector<double> v;

	for (size_t i = 0; i < NUM_ENTRIES; ++i)
	{
		const CPUData& e1 = entries1[i];
		const CPUData& e2 = entries2[i];

		//std::cout.width(3);
		//std::cout << e1.cpu << "] ";

		const float ACTIVE_TIME = static_cast<float>(GetActiveTime(e2) - GetActiveTime(e1));
		const float IDLE_TIME = static_cast<float>(GetIdleTime(e2) - GetIdleTime(e1));
		const float TOTAL_TIME = ACTIVE_TIME + IDLE_TIME;

		v.push_back(100.f * ACTIVE_TIME / TOTAL_TIME);
		v.push_back(100.f * IDLE_TIME / TOTAL_TIME);

		/*std::cout << "active: ";
		std::cout.setf(std::ios::fixed, std::ios::floatfield);
		std::cout.width(6);
		std::cout.precision(2);
		std::cout << (100.f * ACTIVE_TIME / TOTAL_TIME) << "%";

		std::cout << " - idle: ";
		std::cout.setf(std::ios::fixed, std::ios::floatfield);
		std::cout.width(6);
		std::cout.precision(2);
		std::cout << (100.f * IDLE_TIME / TOTAL_TIME) << "%" << std::endl;*/
	}
	return v;
}

void CpuUsage::ReadStatsCPU(std::vector<CPUData>& entries) {
	std::ifstream fileStat("/proc/stat");

	std::string line;

	const std::string STR_CPU("cpu");
	const std::size_t LEN_STR_CPU = STR_CPU.size();
	const std::string STR_TOT("tot");

	while (std::getline(fileStat, line)) {
		// cpu stats line found
		if (!line.compare(0, LEN_STR_CPU, STR_CPU)) {
			std::istringstream ss(line);

			// store entry
			entries.emplace_back(CPUData());
			CPUData& entry = entries.back();

			// read cpu label
			ss >> entry.cpu;

			// remove "cpu" from the label when it's a processor number
			if (entry.cpu.size() > LEN_STR_CPU)
				entry.cpu.erase(0, LEN_STR_CPU);
			// replace "cpu" with "tot" when it's total values
			else
				entry.cpu = STR_TOT;

			// read times
			for (int i = 0; i < NUM_CPU_STATES; ++i)
				ss >> entry.times[i];
		}
	}
}
#endif

void CpuUsage::PrintStats() {
	#if OS == OS_WINDOWS
		double t = static_cast<double>(m_sysTime.wMilliseconds);
		double m = static_cast<double>(m_memStat.dwMemoryLoad);
		m_log->WriteD({t, m});
		//myfile << sysTime.wHour << ":" << sysTime.wMinute << ":" << sysTime.wMilliseconds << ", " << memStat.dwMemoryLoad << "%, " << memStat.ullTotalPhys / MB << ", " << memStat.ullAvailPhys / MB << ", " << memStat.ullTotalPageFile / MB << ", " << memStat.ullAvailPageFile / MB << ", " << memStat.ullTotalVirtual / MB << ", " << memStat.ullAvailVirtual / MB << ", " << memStat.ullAvailExtendedVirtual / MB << "\n";
	#else
		std::vector<CPUData> entries;
		ReadStatsCPU(entries);
		m_log->WriteD(PrintStats(entries1, entries2));
	#endif
	delay(m_delay);
}
