#ifndef HARDCOMMAND_H
#define HARDCOMMAND_H

#include <iostream>
#include <chrono>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <unistd.h>
#include <wiringPi.h>

#include "../BaseThread/BaseThread.hpp"

class HardCommand : public BaseThread {
	public:
		HardCommand(int pin);
		~HardCommand();

		void pinMode();
		void analogWrite(int duty);
		void digitalWrites(bool state);
		void frequencyWrite(int pulse);
	private:
		void* ThreadRun();

		int m_pin;
		int m_pulse;

		const std::string *m_cmdMode;
		const std::string *m_cmdWrite0;
		const std::string *m_cmdWrite1;
};

#endif //HARDCOMMAND_H
