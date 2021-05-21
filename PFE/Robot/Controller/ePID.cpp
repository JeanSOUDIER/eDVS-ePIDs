#include "ePID.hpp"

ePID::ePID(std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float Kp, const float Ki, const float Kd, const float N, DVS* eDVS_4337, const float hnom, const float alpha_i, const float alpha_d)
	: BaseThread("ePID"), m_kp(Kp), m_ki(Ki/2), m_kdN(Kd*N), m_hnom(hnom), m_alpha_i(alpha_i), m_alpha_d(alpha_d) {

	m_eDVS_4337 = eDVS_4337;

	m_elim = m_eDVS_4337->GetThreshold();

	//m_PWM = new HardCommand(0);
	//m_Arduino = new MotorWheel(3, 115200);
	//m_Motor = new Hbridge(28, 29);

	m_log = new logger("ePID_points", begin_timestamp, num_file);
	m_logCPU = new logger("ePID_timing", begin_timestamp, num_file);
	m_logCPUhard = new logger("hard_timing", begin_timestamp, num_file);

	m_eDVS_4337->StartThread();

	std::cout << "ePID Start" << std::endl;
}

ePID::~ePID() {
	std::cout << "evts ePID computed : " << m_cptEvts << std::endl;
	delete m_eDVS_4337;
	delete m_log;
	delete m_logCPU;
	delete m_logCPUhard;
	/*delete m_Arduino;
	delete m_PWM;
	delete m_Motor;*/
}

void ePID::SetPoint(float sp) {
	m_eDVS_4337->SetSetPoint(sp);
}

void* ePID::ThreadRun() {
	while(GetStartValue() && m_eDVS_4337->GetStartValue()) {
		if(m_eDVS_4337->GetEvent()) {
			ComputePID();
		}
	}
	return ReturnFunction();
}

void ePID::ComputePID() {
	m_logCPU->Tic();
	//Get inputs
	const float y = m_eDVS_4337->GetXClusterPose();
	const float e = m_eDVS_4337->GetError();
	//std::cout << "Ysp = " << ysp << " Y = " << y << " e = " << e << std::endl;

	//Compute time
	const unsigned int temp = m_eDVS_4337->GetLastT();
	const unsigned int hact = temp - m_lastT;
	//std::cout << "hact = " << hact << " temp = " << temp << " last temp = " << m_lastT << std::endl;

	//Up
	const float up = m_kp*e;
	//std::cout << "up = " << up << std::endl;

	//Ui
	const float hacti = hact*std::exp(m_alpha_i*(m_hnom-hact));
	const float he = (hacti-m_hnom)*m_elim + m_hnom*(e+m_eOld);
	m_ui += m_ki*he;
	//std::cout << "he = " << he << " ui = " << m_ui << std::endl;

	//Ud
	const float hd = m_hnom + (hact-m_hnom)*std::exp(m_alpha_d*(m_hnom-hact));
	m_ud = m_ud/(1+m_kdN*m_hnom/hd) - m_kdN/(1+m_kdN*m_hnom/hd)*(y-m_yOld);
	//std::cout << "Yold = " << m_yOld << " ud = " << m_ud << std::endl;

	const float u = up + m_ui + m_ud;
	//std::cout << "u = " << u << std::endl;

	//Update
	m_eOld = e;
	m_yOld = y;
	m_lastT = temp;
	
	m_logCPU->Tac();
	m_log->WriteFN({ y, e+y, u});
	m_log->TacF();

	//apply command
	m_logCPUhard->Tic();
	//m_PWM->analogWrite(u);
	//m_Arduino->SetSpeed(u);
	//m_Motor->Set(u);
	m_logCPUhard->Tac();
	m_cptEvts++;
}
