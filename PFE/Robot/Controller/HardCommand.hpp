#ifndef HARDCOMMAND_H
#define HARDCOMMAND_H

#include <iostream>
#include <chrono>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <unistd.h>
#include <atomic>
#include <wiringPi.h>

#include "../BaseThread/BaseThread.hpp"

class HardCommand : public BaseThread {
	public:
		HardCommand(int pin);
		~HardCommand();

		void pinMode();
		void analogWrite(const int duty);
		void digitalWrites(const bool state);
		void frequencyWrite(const int pulse);
	private:
		void* ThreadRun();

		const int m_pin;
		int m_pulse;

		const std::string *m_cmdMode;
		const std::string *m_cmdWrite0;
		const std::string *m_cmdWrite1;

		std::atomic<bool> m_pfm;
};

#endif //HARDCOMMAND_H
