#include "PID.hpp"

PID::PID(const unsigned int Te, const float Kp, const float Ki, const float Kd, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const unsigned int nb_corrector, const unsigned int N, const float beta)
	: BaseThread("PID"), m_Te(Te), m_kp(Kp), m_ki(Ki*Te), m_kd(Kd), m_nb_corrector(nb_corrector), m_N(N*Te), m_beta(beta), m_kdN(Kd*N) {

	//m_PWM = new HardCommand(0);
	//m_Arduino = new MotorWheel(3, 115200);
	//m_Motor = new Hbridge(28, 29);

	m_log = new logger("PID_points"+std::to_string(m_nb_corrector)+"_", begin_timestamp, num_file);
	m_logCPU = new logger("PID_timing"+std::to_string(m_nb_corrector)+"_", begin_timestamp, num_file);
	m_logCPUhard = new logger("hard_timingTT"+std::to_string(m_nb_corrector)+"_", begin_timestamp, num_file);

	g_command[m_nb_corrector].store(0);

	std::cout << "PID Start" << std::endl;
}

PID::~PID() {
	delete m_log;
	delete m_logCPU;
	delete m_logCPUhard;
	delete m_Arduino;
	delete m_PWM;
	delete m_Motor;
}

void* PID::ThreadRun() {
	while (g_working.load()) {
		auto begin_timestamp = std::chrono::high_resolution_clock::now();
		ComputePID();
		auto current_timestamp = std::chrono::high_resolution_clock::now();
		while(std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - begin_timestamp).count() < m_Te) {
			current_timestamp = std::chrono::high_resolution_clock::now();
		}
	}
	return ReturnFunction();
}

void PID::ComputePID() {
	m_logCPU->Tic();
	//Get inputs
	const float ysp = g_setpoint[m_nb_corrector].load();
	const float y = g_feedback[m_nb_corrector].load();
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
	g_command[m_nb_corrector].store(u);
	
	m_logCPU->Tac();
	m_log->WriteFN({y, ysp, u});
	m_log->TacF();

	m_logCPUhard->Tic();
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		//m_PWM->analogWrite(u);
		//m_Arduino->SetSpeed(u);
		//m_Motor->Set(u);
	}
	m_logCPUhard->Tac();
}
