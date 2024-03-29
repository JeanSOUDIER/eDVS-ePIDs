#include "ePID.hpp"

#define MIDDLE_POINT 265

ePID::ePID(std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float Kp, const float Ki, const float Kd, const float N, const unsigned int nb_corrector, const float e_lim, const float hnom, const float alpha_i, const float alpha_d, const float h_nom_fact)
	: BaseThread("ePID"), m_kp(Kp), m_ki(Ki), m_kdN(Kd*N/1000000.0f*7.8f), m_nb_corrector(nb_corrector), m_elim(e_lim), m_hnom(hnom), m_alpha_i(alpha_i), m_alpha_d(alpha_d), m_hnom_fact(h_nom_fact), m_begin_timestamp(begin_timestamp) {

	m_log = new logger("ePID_points"+std::to_string(m_nb_corrector), begin_timestamp, num_file);
	m_logCPU = new logger("ePID_timing"+std::to_string(m_nb_corrector), begin_timestamp, num_file);
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) { //if the constroller is the last and apply the PWM
		m_Arduino = new MotorWheel("ttyUSB_Teensy", 115200, begin_timestamp, num_file, m_elim, MIDDLE_POINT);
		m_logCPUhard = new logger("hard_timing", begin_timestamp, num_file);

		m_Arduino->SetLim(2); //SOD threshold on the sensor
		m_Arduino->SetLim(2);
		m_Arduino->SetMiddlePoint(MIDDLE_POINT);
	} else {
		g_setpoint[m_nb_corrector+1].store(0);
		g_event[m_nb_corrector+1].store(true);
	}

	std::cout << "e_lim = " << m_elim << std::endl;

	std::cout << "ePID Start at " << m_nb_corrector << std::endl;
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
	std::unique_lock<std::mutex> lk(g_cv_mutex[m_nb_corrector]); //create an unique lock for the thread
	lk.unlock(); //unlock it
	const int wait_time = static_cast<int>(static_cast<float>((1000.0f*m_hnom)/m_hnom_fact)); //compute the time to wait between events
	std::cout << m_nb_corrector << " wait time " << wait_time << " " << m_hnom << " " << m_hnom_fact << std::endl;
	g_event[m_nb_corrector].store(false);
	while(GetStartValue()) {
		while(!g_event[m_nb_corrector].load()) { //while no event
			g_cv[m_nb_corrector].wait(lk);	     //wait
		}
		ComputePID();
		g_event[m_nb_corrector].store(false);	 //event done
		std::this_thread::sleep_for(std::chrono::microseconds(wait_time)); //wait
	}

	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		m_Arduino->SetHbridge(0); //stop the motor at the end
	}

	return ReturnFunction();
}

void ePID::ComputePID() {
	m_logCPU->Tic();

	//Get inputs
	const float ysp = g_setpoint[m_nb_corrector].load();
	const float y = g_feedback[m_nb_corrector].load();
	const float e = ysp - y;
	//std::cout << m_nb_corrector << " Ysp = " << ysp << " Y = " << y << " e = " << e << std::endl;

	//Compute time
	const float temp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin_timestamp).count()/1000.0f;
	const float hact = temp - m_lastT;
	//std::cout << m_nb_corrector << " hact = " << hact << " temp = " << temp << " last temp = " << m_lastT << std::endl;

	//Up
	const float up = m_kp*e;
	//std::cout << m_nb_corrector << " up = " << up << std::endl;

	
	if(hact > m_hnom) {
		//Ui
		const float hacti = hact*std::exp(m_alpha_i*(m_hnom-hact));
		const float he = (hacti-m_hnom)*m_elim + m_hnom*e;
		m_ui += m_ki*he/1000.0f;
		//std::cout << m_nb_corrector << " he = " << he << " hacti = " << hacti << " ui = " << m_ui << std::endl;

		//Ud
		const float hd = m_hnom + (hact-m_hnom)*std::exp(m_alpha_d*(m_hnom-hact));
		m_ud = m_ud/(1+m_kdN*m_hnom/hd) - m_kdN/(1+m_kdN*m_hnom/hd)*(y-m_yOld);
		//std::cout << m_nb_corrector << " Yold = " << m_yOld << " ud = " << m_ud << " hd = " << hd << std::endl;
	} else {
		//Ui
		const float he = (hact-m_hnom)*m_elim + m_hnom*e;
		m_ui += m_ki*he/1000.0f;
		//std::cout << m_nb_corrector << " he = " << he << " ui = " << m_ui << std::endl;

		//Ud
		m_ud = m_ud/(1+m_kdN*m_hnom/hact) - m_kdN/(1+m_kdN*m_hnom/hact)*(y-m_yOld);
		//std::cout << m_nb_corrector << " Yold = " << m_yOld << " ud = " << m_ud << std::endl;
	}

	float u = up + m_ui + m_ud;
	//std::cout << m_nb_corrector << " u = " << u << std::endl;

	//Update
	m_yOld = y;
	m_lastT = temp;
	
	m_logCPU->Tac();
	m_log->WriteFN({y, ysp, u});
	m_log->TacF();

	//apply command
	if(LENGTH_PID_CHAIN == m_nb_corrector+1) {
		if(y < -9.4248 || y > 9.4248) {u = 0;std::cout << "Emergency stop" << std::endl;}
		m_logCPUhard->Tic();
		m_Arduino->SetHbridge(u*21.25f); //ratio [U]/PWM
		m_logCPUhard->Tac();
		//std::cout << m_nb_corrector << " u = " << u*21.33f << std::endl;
	} else {
		if(u > 9.4248) {u = 9.4248;} //saturations
		if(u < -9.4248) {u = -9.4248;}
		if(std::isnan(u) || std::isinf(u)) {u = 0;std::cout << "error cmd" << std::endl;}
		g_setpoint[m_nb_corrector+1].store(u);
		g_event[m_nb_corrector+1].store(true);
		g_cv[m_nb_corrector+1].notify_one();
		//std::cout << m_nb_corrector << " u = " << u << std::endl;
	}
	m_cptEvts++;
}

void ePID::Read() {m_Arduino->ReadPose();} //function to read the potentiometer value
