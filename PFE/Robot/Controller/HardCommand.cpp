#include "HardCommand.hpp"

HardCommand::HardCommand(int pin)
	: BaseThread("PFM"), m_pin(pin) {

	std::string cmd1 = "gpio mode ", cmd2 = " out";
	std::stringstream strs;
	strs << m_pin;
	cmd1 = cmd1 + strs.str() + cmd2;
	m_cmdMode = new const std::string(cmd1);

	cmd1 = "gpio write ";
	cmd2 = " 0";
	cmd2 = cmd1 + strs.str() + cmd2;
	m_cmdWrite0 = new const std::string(cmd2);

	cmd2 = " 1";
	cmd1 = cmd1 + strs.str() + cmd2;
	m_cmdWrite1 = new const std::string(cmd1);

	pinMode();

	m_pfm.store(false);
	StartThread();

	std::cout << "HardCommand Start" << std::endl;
}

HardCommand::~HardCommand() {
	StopThread();
}

void HardCommand::pinMode() {
	system((char*)(*m_cmdMode).c_str());
}

void HardCommand::analogWrite(const int duty) {
	std::string cmd1 = "gpio pwm ";
	std::stringstream strs, strs1;
	strs << m_pin;
	strs1 << duty;
	cmd1 = cmd1 + strs.str() + " " + strs1.str();
	const char* cmd = (char*)cmd1.c_str();
	system(cmd);
}

void HardCommand::digitalWrites(const bool state) {
	if(state) {
		system((char*)(*m_cmdWrite1).c_str());
	} else {
		system((char*)(*m_cmdWrite0).c_str());
	}
}

void HardCommand::frequencyWrite(const int pulse) {
	m_pulse = pulse;
	system((char*)(*m_cmdWrite1).c_str());
	m_pfm.store(true);
}

void* HardCommand::ThreadRun() {
	while (GetStartValue()) {
		while(!m_pfm.load()) {}
		m_pfm.store(false);
		auto begin_timestamp = std::chrono::high_resolution_clock::now();
		auto current_timestamp = std::chrono::high_resolution_clock::now();
		while(std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - begin_timestamp).count() < m_pulse) {
			if(m_pfm.load()) {
				m_pfm.store(false);
				begin_timestamp = std::chrono::high_resolution_clock::now();
			}
			current_timestamp = std::chrono::high_resolution_clock::now();
		}
		system((char*)(*m_cmdWrite0).c_str());
	}
	return ReturnFunction();
}
