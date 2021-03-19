#include "ePID.hpp"

HardCommand::HardCommand(int pin)
	: BaseThread("PFM") {
	m_pin = pin;
	std::cout << "HardCommand Start" << std::endl;
}

HardCommand::~HardCommand() {}

void HardCommand::pinMode() {
	#if OS == OS_WINDOWS
		std::cout << "no PWM mode" << std::endl;
	#else
		std::string cmd1 = "gpio mode ", cmd2 = " pwm";
		std::stringstream strs;
		strs << m_pin;
		cmd1 = cmd1 + strs.str() + cmd2;
		char* cmd = (char*)cmd1.c_str();
		system(cmd);
	#endif
}

void HardCommand::analogWrite(int duty) {
	#if OS == OS_WINDOWS
		std::cout << "no PWM mode, val = " << duty << std::endl;
	#else
		std::string cmd1 = "gpio pwm ";
		std::stringstream strs, strs1;
		strs << m_pin;
		strs1 << duty;
		cmd1 = cmd1 + strs.str() + " " + strs1.str();
		char* cmd = (char*)cmd1.c_str();
		system(cmd);
	#endif
}

void HardCommand::digitalWrite(bool state) {
	#if OS == OS_WINDOWS
		std::cout << "no PWM mode, state = " << state << std::endl;
	#else
		digitalWrite(m_pin, state);
	#endif
}

void HardCommand::frequencyWrite(int pulse) {
	#if OS == OS_WINDOWS
		std::cout << "no PFM mode, val = " << pulse << std::endl;
	#else
		m_pulse = pulse;
		digitalWrite(m_pin, true);
		StartThread();
	#endif
}

void* HardCommand::ThreadRun() {
	auto begin_timestamp = std::chrono::high_resolution_clock::now();
	auto current_timestamp = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - begin_timestamp).count() < m_pulse) {
		current_timestamp = std::chrono::high_resolution_clock::now();
	}
	#if OS == OS_WINDOWS
		std::cout << "no PFM mode" << std::endl;
	#else
		digitalWrite(m_pin, false);
	#endif
	return ReturnFunction();
}
