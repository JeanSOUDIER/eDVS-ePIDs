#include "Hbridge.hpp"

Hbridge::Hbridge(const int pinL, const int pinR, const int Gain)
	: m_Gain(Gain) {
	m_motL = new HardCommand(pinL);
	m_motR = new HardCommand(pinR);
	std::cout << "Hbridge Start" << std::endl;
}

Hbridge::~Hbridge() {
	delete m_motR;
	delete m_motL;
}

void Hbridge::Set(const float cmd) {
	if(cmd > 0) {
		m_motR->digitalWrites(false);
		m_motL->frequencyWrite(m_Gain*cmd);
	} else {
		m_motL->digitalWrites(false);
		m_motR->frequencyWrite(-cmd);
	}
}
