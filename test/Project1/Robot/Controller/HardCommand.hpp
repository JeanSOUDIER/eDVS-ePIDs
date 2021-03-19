#ifndef HARDCOMMAND_H
#define HARDCOMMAND_H

#if OS != OS_WINDOWS
#include <wiringPi.h>
#endif
#include <chrono>

#include "../BaseThread/BaseThread.hpp"

class HardCommand : public BaseThread {
	public:
		HardCommand(int pin);
		~HardCommand();

		void pinMode();
		void analogWrite(int duty);
		void digitalWrite(bool state);
		void frequencyWrite(int pulse);
	private:
		void* ThreadRun();

		int m_pin;
		int m_pulse;
};

#endif //HARDCOMMAND_H
