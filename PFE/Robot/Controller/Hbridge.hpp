#ifndef HBRIDGE_H
#define HBRIDGE_H

#include <iostream>

#include "HardCommand.hpp"

class Hbridge {
	public:
		Hbridge(const int pinL, const int pinR);
		~Hbridge();
		
		void Set(const float cmd);
	private:
		HardCommand *m_motR;
		HardCommand *m_motL;
};

#endif //HBRIDGE_H
