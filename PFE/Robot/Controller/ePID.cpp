#include "ePID.hpp"

ePID::ePID(std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const float Kp, const float Ki, const float Kd, const float hmax, const float hnom, const unsigned int N, const float beta, const float thresSetPoint, DVS *eDVS_4337)
	: BaseThread("ePID"), m_kp(Kp), m_ki(Ki), m_kd(Kd), m_hmax(hmax), m_hnom(hnom), m_N(N), m_beta(beta), m_thresSetPoint(thresSetPoint), m_kdN(Kd*N) {

	m_eDVS_4337 = eDVS_4337;

	m_elim = m_eDVS_4337->GetThreshold();
	m_ysp.store(0);
	m_event.store(false);

	//m_PWM = new HardCommand(0);
	//m_Arduino = new MotorWheel(3, 115200);
	//m_Motor = new Hbridge(28, 29);

	m_log = new logger("ePID_points", begin_timestamp);
	m_logCPU = new logger("ePID_timing", begin_timestamp);

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
		m_event.store(true);
	}
}

void* ePID::ThreadRun() {
	while (GetStartValue() && m_eDVS_4337->GetStartValue()) {
		if (m_eDVS_4337->GetEvent() || m_event.load()) {
			ComputePID();
		}
	}
	return ReturnFunction();
}

void ePID::ComputePID() {
	m_logCPU->Tic();
	m_event.store(false);
	//Get inputs
	const float ysp = m_ysp.load();
	const float y = m_eDVS_4337->GetXClusterPose();
	const float e = ysp - y;
	//std::cout << "Ysp = " << ysp << " Y = " << y << " e = " << e << std::endl;

	//Compute time
	const unsigned int temp = m_eDVS_4337->GetLastT();
	const unsigned int hact = temp - m_lastT;
	//std::cout << "hact = " << hact << " temp = " << temp << " last temp = " << m_lastT << std::endl;

	//Up
	const float up = m_kp * (m_beta * ysp - y);
	//std::cout << "up = " << up << std::endl;

	//Ui
	if (hact >= m_hmax) {
		//const float hacti = hact * lut::expo[(-hact+lut::lim_min)*lut::epsilon];
		const float hacti = hact * std::exp(-hact);
		//std::cout << "hacti = " << hacti << std::endl;
		const float he = (hacti - m_hnom) * m_elim + m_hnom * e;
		m_ui += m_ki * he;
	} else {
		const float he = hact * e;
		m_ui += m_ki * he;
	}
	//std::cout << "he = " << he << " ui = " << m_ui << std::endl;

	//Ud
	m_ud = ((m_kd * m_ud) / (m_N * hact + m_kd)) - (m_kdN / (m_N * hact + m_kd)) * (y - m_yOld);
	//std::cout << "Yold = " << m_yOld << " ud = " << m_ud << std::endl;

	const float u = up + m_ui + m_ud;
	//std::cout << "u = " << u << std::endl;

	//Update
	m_yOld = y;
	m_lastT = temp;
	
	//m_PWM->analogWrite(u);
	//m_Arduino->SetSpeed(u);
	//m_Motor->Set(u);

	m_log->WriteFN({ y, ysp, u});
	m_log->TacF();
	m_logCPU->Tac();
}
