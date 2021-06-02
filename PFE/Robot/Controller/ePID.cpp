#include "ePID.hpp"

#define MIDDLE_POINT 265

ePID::ePID(std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float Kp, const float Ki, const float Kd, const float N, const unsigned int nb_corrector, const float e_lim, const float hnom, const float alpha_i, const float alpha_d)
	: BaseThread("ePID"), m_kp(Kp), m_ki(Ki), m_kdN(Kd*N), m_nb_corrector(nb_corrector), m_elim(e_lim), m_hnom(hnom), m_alpha_i(alpha_i), m_alpha_d(alpha_d), m_begin_timestamp(begin_timestamp) {

	m_log = new logger("ePID_points"+std::to_string(m_nb_corrector), begin_timestamp, num_file);
	m_logCPU = new logger("ePID_timing"+std::to_string(m_nb_corrector), begin_timestamp, num_file);
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		m_Arduino = new MotorWheel("ttyUSB_Teensy", 115200);
		m_logCPUhard = new logger("hard_timing", begin_timestamp, num_file);

		m_Arduino->SetLim(m_elim);
		m_Arduino->SetLim(m_elim);
		m_Arduino->SetMiddlePoint(MIDDLE_POINT);
	} else {
		g_setpoint[m_nb_corrector+1].store(0);
		g_event[m_nb_corrector+1].store(true);
	}

	std::cout << "e_lim = " << m_elim << std::endl;

	std::cout << "ePID Start" << std::endl;
}

ePID::~ePID() {
	std::cout << m_nb_corrector << " evts ePID computed : " << m_cptEvts << std::endl;
	delete m_log;
	delete m_logCPU;
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		delete m_Arduino;
		delete m_logCPUhard;
	}
}

void* ePID::ThreadRun() {
	while(GetStartValue()) {
		if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
			g_feedback[m_nb_corrector].store(m_Arduino->ReadPose());
			g_event[m_nb_corrector].store(true);
		}
		if(g_event[m_nb_corrector].load()) {
			ComputePID();
			g_event[m_nb_corrector].store(false);
		}
	}
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		m_Arduino->SetHbridge(0);
	}
	return ReturnFunction();
}

void ePID::ComputePID() {
	m_logCPU->Tic();
	//Get inputs
	const float ysp = g_setpoint[m_nb_corrector].load();
	float y;
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		y = m_Arduino->ReadPose();
		//std::cout << m_nb_corrector << " mesure = " << y << std::endl;
		y = (y-MIDDLE_POINT)*0.065f;//0.0614f;
	} else {
		y = g_feedback[m_nb_corrector].load();
	}
	const float e = ysp - y;
	std::cout << m_nb_corrector << " Ysp = " << ysp << " Y = " << y << " e = " << e << std::endl;

	//Compute time
	const float temp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin_timestamp).count()/1000000.0f;
	const float hact = temp - m_lastT;
	std::cout << m_nb_corrector << " hact = " << hact << " temp = " << temp << " last temp = " << m_lastT << std::endl;

	//Up
	const float up = m_kp*e;
	//std::cout << m_nb_corrector << " up = " << up << std::endl;

	//Ui
	const float hacti = hact*std::exp(m_alpha_i*(m_hnom-hact));
	const float he = (hacti-m_hnom)*m_elim + m_hnom*e;
	m_ui += m_ki*he;
	//std::cout << m_nb_corrector << " he = " << he << " ui = " << m_ui << std::endl;

	//Ud
	const float hd = m_hnom + (hact-m_hnom)*std::exp(m_alpha_d*(m_hnom-hact));
	m_ud = m_ud/(1+m_kdN*m_hnom/hd) - m_kdN/(1+m_kdN*m_hnom/hd)*(y-m_yOld);
	//std::cout << m_nb_corrector << " Yold = " << m_yOld << " ud = " << m_ud << std::endl;

	float u = up + m_ui + m_ud;
	std::cout << m_nb_corrector << " u = " << u << std::endl;

	//Update
	m_yOld = y;
	m_lastT = temp;
	
	m_logCPU->Tac();
	m_log->WriteFN({y, ysp, u});
	m_log->TacF();

	//apply command
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		m_logCPUhard->Tic();
		m_Arduino->SetHbridge(u*21.33f);
		m_logCPUhard->Tac();
		//std::cout << m_nb_corrector << " u = " << u << std::endl;
	} else {
		//if(std::fabs(e) < 2.5) {u = 0;}
		if(u > 9.4248) {u = 9.4248;}
		if(u < -9.4248) {u = -9.4248;}
		if(std::isnan(u) || std::isinf(u)) {u = 0;std::cout << "error cmd" << std::endl;}
		g_setpoint[m_nb_corrector+1].store(u);
		g_event[m_nb_corrector+1].store(true);
		//std::cout << m_nb_corrector << " u = " << u << std::endl;
	}
	m_cptEvts++;
}
