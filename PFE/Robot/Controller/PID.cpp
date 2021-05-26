#include "PID.hpp"

PID::PID(const unsigned int Te, const float Kp, const float Ki, const float Kd, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const unsigned int nb_corrector, const unsigned int N, const float beta)
	: BaseThread("PID"), m_Te(Te), m_kp(Kp), m_ki(Ki*Te), m_kd(Kd), m_nb_corrector(nb_corrector), m_N(N*Te), m_beta(beta), m_kdN(Kd*N) {

	m_Arduino = new MotorWheel("ttyUSB_Teensy", 115200);
	//m_Motor = new Hbridge(28, 29);

	m_log = new logger("PID_points"+std::to_string(m_nb_corrector)+"_", begin_timestamp, num_file);
	m_logCPU = new logger("PID_timing"+std::to_string(m_nb_corrector)+"_", begin_timestamp, num_file);
	m_logCPUhard = new logger("hard_timingTT"+std::to_string(m_nb_corrector)+"_", begin_timestamp, num_file);

	std::cout << "PID Start" << std::endl;
}

PID::~PID() {
	delete m_log;
	delete m_logCPU;
	delete m_logCPUhard;
	delete m_Arduino;
	//delete m_Motor;
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
	m_Arduino->SetHbridge(0);
	return ReturnFunction();
}

void PID::ComputePID() {
	m_logCPU->Tic();
	//Get inputs
	const float ysp = g_setpoint[m_nb_corrector].load();
	float y;
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		y = m_Arduino->ReadPose();
	} else {
		y = g_feedback[m_nb_corrector].load();
	}
	const float e = ysp - y;
	std::cout << "Ysp = " << ysp << " Y = " << y << " e = " << e << std::endl;

	//Up
	const float up = m_kp * (m_beta * ysp - y);
	//std::cout << "up = " << up << std::endl;

	//Ui
	m_ui += m_ki * e * m_Te/1000.0f;
	//std::cout << " ui = " << m_ui << std::endl;

	//Ud
	m_ud = m_kd * m_ud / (m_N + m_kd) - m_kdN / (m_N + m_kd) * (y - m_yOld);
	//std::cout << "Yold = " << m_yOld << " ud = " << m_ud << std::endl;

	float u = up + m_ui + m_ud;
	std::cout << "u = " << u << std::endl;

	//Update
	m_yOld = y;
	
	m_logCPU->Tac();
	m_log->WriteFN({y, ysp, u});
	m_log->TacF();

	m_logCPUhard->Tic();
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		m_Arduino->SetHbridge(u);
	} else {
		g_setpoint[m_nb_corrector+1].store(u);
	}
	m_logCPUhard->Tac();
}

int PID::read() {
	return m_Arduino->ReadPose();
}
