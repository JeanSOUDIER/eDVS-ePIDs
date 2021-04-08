#include "ePID.hpp"

ePID::ePID(const float Kp, const float Ki, const float Kd, const float hmax, const float hnom, const unsigned int N, const float beta, const float thresSetPoint, DVS *eDVS_4337)
	: BaseThread("ePID"), m_kp(Kp), m_ki(Ki), m_kd(Kd), m_hmax(hmax), m_hnom(hnom), m_N(N), m_beta(beta), m_thresSetPoint(thresSetPoint) {

	m_eDVS_4337 = eDVS_4337;

	m_elim = m_eDVS_4337->GetThreshold();
	m_ysp.store(0);

	//m_PWM = new HardCommand(0);
	//m_Arduino = new MotorWheel(3, 115200);
	//m_Motor = new Hbridge(28, 29);

	m_log = new logger("ePID_points");
	m_logCPU = new logger("ePID_timing");

	m_eDVS_4337->StartThread();

	std::cout << "ePID Start" << std::endl;
}

ePID::~ePID() {
	delete m_eDVS_4337;
	delete m_log;
	delete m_logCPU;
	/*delete m_Arduino;
	delete m_PWM;
	delete m_Motor;*/
}

void ePID::SetPoint(float sp) {
	if(-m_thresSetPoint > sp-m_ysp || m_thresSetPoint < sp-m_ysp) {
		std::cout << "new set point = " << sp << std::endl;
		m_ysp.store(sp);
		ComputePID();
	}
}

void* ePID::ThreadRun() {
	while (GetStartValue() && m_eDVS_4337->GetStartValue()) {
		if (m_eDVS_4337->GetEvent()) {
			ComputePID();
		}
	}
	return ReturnFunction();
}

void ePID::ComputePID() {
	m_logCPU->Tic();
	//Get inputs
	float ysp = m_ysp.load();
	float y = m_eDVS_4337->GetXClusterPose();
	float e = ysp - y;
	//std::cout << "Ysp = " << ysp << " Y = " << y << " e = " << e << std::endl;

	//Compute time
	unsigned int temp = m_eDVS_4337->GetLastT();
	unsigned int hact = temp - m_lastT;
	//std::cout << "hact = " << hact << " temp = " << temp << " last temp = " << m_lastT << std::endl;

	//Up
	float up = m_kp * (m_beta * ysp - y);
	//std::cout << "up = " << up << std::endl;

	//Ui
	float he;
	if (hact >= m_hmax) {
		float hacti = hact * std::exp(m_hnom - hact);
		//std::cout << "hacti = " << hacti << std::endl;
		he = (hacti - m_hnom) * m_elim + m_hnom * e;
	}
	else {
		he = hact * e;
	}
	m_ui += m_ki * he;
	//std::cout << "he = " << he << " ui = " << m_ui << std::endl;

	//Ud
	m_ud = m_kd * m_ud / (m_N * hact + m_kd) - m_kd * m_N / (m_N * hact + m_kd) * (y - m_yOld);
	//std::cout << "Yold = " << m_yOld << " ud = " << m_ud << std::endl;

	float u = up + m_ui + m_ud;
	//std::cout << "u = " << u << std::endl;

	//Update
	m_yOld = y;
	m_lastT = temp;

	m_log->WriteFN({ y, ysp, u});
	m_log->TacF();
	m_logCPU->Tac();

	//m_PWM->analogWrite(u);
	//m_Arduino->SetSpeed(u);
	//m_Motor->Set(u);
}
