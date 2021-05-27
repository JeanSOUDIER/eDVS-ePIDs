#include "ePID.hpp"

ePID::ePID(std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float Kp, const float Ki, const float Kd, const float N, const unsigned int nb_corrector, const float e_lim, const float hnom, const float alpha_i, const float alpha_d)
	: BaseThread("ePID"), m_kp(Kp), m_ki(Ki), m_kdN(Kd*N), m_nb_corrector(nb_corrector), m_elim(e_lim), m_hnom(hnom), m_alpha_i(alpha_i), m_alpha_d(alpha_d), m_begin_timestamp(begin_timestamp) {

	m_Arduino = new MotorWheel("ttyUSB_Teensy", 115200);

	m_log = new logger("ePID_points"+std::to_string(m_nb_corrector), begin_timestamp, num_file);
	m_logCPU = new logger("ePID_timing"+std::to_string(m_nb_corrector), begin_timestamp, num_file);
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		m_logCPUhard = new logger("hard_timing", begin_timestamp, num_file);
	}

	m_Arduino->SetBegin(400);
	std::cout << "e_lim = " << m_elim << std::endl;
	m_Arduino->SetLim(m_elim);

	std::cout << "ePID Start" << std::endl;
}

ePID::~ePID() {
	std::cout << "evts ePID computed : " << m_cptEvts << std::endl;
	delete m_log;
	delete m_logCPU;
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		delete m_logCPUhard;
	}
	delete m_Arduino;
}

void* ePID::ThreadRun() {
	while(GetStartValue()) {
		if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
			g_feedback[m_nb_corrector].store(m_Arduino->ReadPose());
		}
		if(g_event[m_nb_corrector].load()) {
			ComputePID();
			g_event[m_nb_corrector].store(false);
		}
	}
	return ReturnFunction();
}

void ePID::ComputePID() {
	m_logCPU->Tic();
	//Get inputs
	const float ysp = g_setpoint[m_nb_corrector].load();
	const float y = g_feedback[m_nb_corrector].load();
	const float e = ysp-y;
	std::cout << "Ysp = " << ysp << " Y = " << y << " e = " << e << std::endl;

	//Compute time
	const float temp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin_timestamp).count()/1000000.0f;
	const float hact = temp - m_lastT;
	std::cout << "hact = " << hact << " temp = " << temp << " last temp = " << m_lastT << std::endl;

	//Up
	const float up = m_kp*e;
	//std::cout << "up = " << up << std::endl;

	//Ui
	const float hacti = hact*std::exp(m_alpha_i*(m_hnom-hact));
	const float he = (hacti-m_hnom)*m_elim + m_hnom*e;
	m_ui += m_ki*he;
	//std::cout << "he = " << he << " ui = " << m_ui << std::endl;

	//Ud
	const float hd = m_hnom + (hact-m_hnom)*std::exp(m_alpha_d*(m_hnom-hact));
	m_ud = m_ud/(1+m_kdN*m_hnom/hd) - m_kdN/(1+m_kdN*m_hnom/hd)*(y-m_yOld);
	//std::cout << "Yold = " << m_yOld << " ud = " << m_ud << std::endl;

	const float u = up + m_ui + m_ud;
	std::cout << "u = " << u << std::endl;

	//Update
	m_yOld = y;
	m_lastT = temp;
	
	m_logCPU->Tac();
	m_log->WriteFN({y, ysp, u});
	m_log->TacF();

	//apply command
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		m_logCPUhard->Tic();
		m_Arduino->SetHbridge(u);
		m_logCPUhard->Tac();
	} else {
		g_setpoint[m_nb_corrector+1].store(u);
	}
	m_cptEvts++;
}
