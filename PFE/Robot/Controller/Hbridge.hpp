#ifndef HBRIDGE_H
#define HBRIDGE_H

#include <iostream>

#include "HardCommand.hpp"

class Hbridge {
	public:
		Hbridge(const int pinL, const int pinR, const int Gain = 1);
		~Hbridge();
		
		void Set(const float cmd);
		void Stop();
	private:
		HardCommand *m_motR;
		HardCommand *m_motL;

		const int m_Gain;
};

#endif //HBRIDGE_H
