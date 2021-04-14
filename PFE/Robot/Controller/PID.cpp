#include "PID.hpp"

PID::PID(const unsigned int Te, const float Kp, const float Ki, const float Kd, const unsigned int N, const float gamma, const float beta)
	: BaseThread("PID"), m_Te(Te), m_Kp(Kp), m_Ki(Ki*Te), m_Kd(Kd), m_N(N*Te), m_gamma(gamma), m_beta(beta), m_kdN(Kd*N) {

	m_eDVS_4337 = eDVS_4337;

	m_ysp.store(0);

	//m_PWM = new HardCommand(0);
	//m_Arduino = new MotorWheel(3, 115200);
	//m_Motor = new Hbridge(28, 29);

	m_log = new logger("PID_points");
	m_logCPU = new logger("PID_timing");

	std::cout << "PID Start" << std::endl;
}

PID::~PID() {
	//delete m_eDVS_4337;
	delete m_log;
	delete m_logCPU;
	delete m_Arduino;
	delete m_PWM;
	delete m_Motor;
}

void PID::SetPoint(float sp) {
	std::cout << "new set point = " << sp << std::endl;
	m_ysp.store(sp);
}

void* PID::ThreadRun() {
	while (GetStartValue()) {
		auto begin_timestamp = std::chrono::high_resolution_clock::now();
		ComputePID();
		auto current_timestamp = std::chrono::high_resolution_clock::now();
		while(std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - begin_timestamp).count() < m_Te) {
			current_timestamp = std::chrono::high_resolution_clock::now();
		}
	}
	return ReturnFunction();
}

void ePID::ComputePID() {
	m_logCPU->Tic();
	//Get inputs
	const float ysp = m_ysp.load();
	const float y = m_eDVS_4337->GetXClusterPose();
	const float e = ysp - y;
	//std::cout << "Ysp = " << ysp << " Y = " << y << " e = " << e << std::endl;

	//Up
	const float up = m_kp * (m_beta * ysp - y);
	//std::cout << "up = " << up << std::endl;

	//Ui
	m_ui = m_ki * e;
	//std::cout << " ui = " << m_ui << std::endl;

	//Ud
	m_ud = m_kd * m_ud / (m_N + m_kd) - m_kdN / (m_N + m_kd) * (y - m_yOld);
	//std::cout << "Yold = " << m_yOld << " ud = " << m_ud << std::endl;

	float u = up + m_ui + m_ud;
	//std::cout << "u = " << u << std::endl;

	//Update
	m_yOld = y;

	//m_PWM->analogWrite(u);
	//m_Arduino->SetSpeed(u);
	//m_Motor->Set(u);
	
	m_log->WriteFN({y, ysp, u});
	m_log->TacF();
	m_logCPU->Tac();
}
