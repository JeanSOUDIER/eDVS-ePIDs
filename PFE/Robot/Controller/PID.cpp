#include "PID.hpp"

#define MIDDLE_POINT 265

PID::PID(const unsigned int Te, const float Kp, const float Ki, const float Kd, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const unsigned int nb_corrector, const unsigned int N)
	: BaseThread("PID"), m_Te(Te), m_kp(Kp), m_ki(Ki*Te), m_kd(Kd), m_nb_corrector(nb_corrector), m_N(N*Te), m_kdN(Kd*N) {

	m_log = new logger("PID_points"+std::to_string(m_nb_corrector), begin_timestamp, num_file);
	m_logCPU = new logger("PID_timing"+std::to_string(m_nb_corrector), begin_timestamp, num_file);
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) { //if the constroller is the last and apply the PWM
		m_Arduino = new MotorWheel("ttyUSB_Teensy", 115200, begin_timestamp, num_file);
		m_logCPUhard = new logger("hard_timing", begin_timestamp, num_file);

		m_Arduino->SetLim(0);
		m_Arduino->SetLim(0);
		m_Arduino->SetMiddlePoint(MIDDLE_POINT);
	} else {
		g_setpoint[m_nb_corrector+1].store(0);
		g_event[m_nb_corrector+1].store(true);
	}


	std::cout << "PID Start at " << m_nb_corrector << std::endl;
}

PID::~PID() {
	std::cout << m_nb_corrector << " evts PID computed : " << m_cptEvts << std::endl;
	delete m_log;
	delete m_logCPU;
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		delete m_Arduino;
		delete m_logCPUhard;
	} else {
		g_setpoint[m_nb_corrector+1].store(0);
	}
}

void* PID::ThreadRun() {
	const int wait_time = m_Te*1000;
	while (GetStartValue()) {
		ComputePID();
		std::this_thread::sleep_for(std::chrono::microseconds(wait_time)); //wait
	}
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		m_Arduino->SetHbridge(0); //stop the motor at the end
	}
	return ReturnFunction();
}

void PID::ComputePID() {
	m_logCPU->Tic();
	//Get inputs
	const float ysp = g_setpoint[m_nb_corrector].load();
	const float y = g_feedback[m_nb_corrector].load();
	const float e = ysp - y;
	//std::cout << m_nb_corrector << " Ysp = " << ysp << " Y = " << y << " e = " << e << std::endl;

	//Up
	const float up = m_kp * e;
	//std::cout << m_nb_corrector << " up = " << up << std::endl;

	//Ui
	m_ui += m_ki * e * (m_Te/1000.0f);
	//std::cout << m_nb_corrector << " ui = " << m_ui << std::endl;

	//Ud
	m_ud = m_kd * m_ud / (m_N + m_kd) - m_kdN / (m_N + m_kd) * (y - m_yOld);
	//std::cout << m_nb_corrector << " Yold = " << m_yOld << " ud = " << m_ud << std::endl;

	float u = up + m_ui + m_ud;
	//std::cout << m_nb_corrector << " u = " << u << std::endl;

	//Update
	m_yOld = y;
	//m_uOld = u;
	
	m_logCPU->Tac();
	m_log->WriteFN({y, ysp, u});
	m_log->TacF();

	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		if(y < -9.4248 || y > 9.4248) {u = 0;std::cout << "Emergency stop" << std::endl;}
		m_logCPUhard->Tic();
		m_Arduino->SetHbridge(u*21.25f); //ratio [U]/PWM
		m_logCPUhard->Tac();
		//std::cout << m_nb_corrector << " u = " << u << std::endl;
	} else {
		if(u > 9.4248) {u = 9.4248;} //saturations
		if(u < -9.4248) {u = -9.4248;}
		if(std::isnan(u) || std::isinf(u)) {u = 0;std::cout << "error cmd" << std::endl;}
		g_setpoint[m_nb_corrector+1].store(u);
		if(!g_event[m_nb_corrector+1].load()) {
			g_event[m_nb_corrector+1].store(true);
			g_cv[m_nb_corrector+1].notify_one();
		}
		g_event[m_nb_corrector+1].store(true);
		//std::cout << m_nb_corrector << " u = " << u << std::endl;
	}
	m_cptEvts++;
}

void PID::Read() {m_Arduino->ReadPose();} //function to read the potentiometer value
