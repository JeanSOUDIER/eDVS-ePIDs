#include "DVS.hpp"

DVS::DVS(const std::string nb_usb, const int bdrate, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float thresEvent)
	: BaseThread("DVS"), m_thresEvent(thresEvent) {
	m_usb = new Usb(nb_usb, bdrate);
	Configuration(begin_timestamp, num_file);
}

DVS::DVS(const int nb_usb, const int bdrate, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float thresEvent)
	: BaseThread("DVS"), m_thresEvent(thresEvent) {
    m_usb = new Usb(nb_usb, bdrate);
	Configuration(begin_timestamp, num_file);
}

void DVS::Configuration(std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file) {
	m_XClustPose = 64;
	m_YClustPose = 64;
	m_XClustPoseOld = 64;
	m_YClustPoseOld = 64;

	//config
	toLengthRead();
	char form = DVS_PACKET_TYPE + '0';
	std::vector<char> str = { '!', 'E', form, '\n' };
	std::cout << str.at(2) << std::endl;

	if (m_usb->GetActive()) {
		Restart();
		std::cout << "Config DVS" << std::endl;
		m_usb->SendBytes("E-\n");           //disable event sending
		m_usb->SendBytes("!L2\n");          //LED blinking
		m_usb->SendBytes("!U0\n");          //UART echo mode none
		m_usb->SendBytes(str);              //select data format 2, 3-6, 4, 5, 6 bytes
		m_usb->SendBytes("!B0=54\n");       //bias cas
		m_usb->SendBytes("!B1=1108364\n");  //bias injGnd
		m_usb->SendBytes("!B2=16777245\n"); //bias reqPd
		m_usb->SendBytes("!B3=8159221\n");  //bias puX
		m_usb->SendBytes("!B4=111\n");      //bias diffOff
		m_usb->SendBytes("!B5=159147\n");   //bias req
		m_usb->SendBytes("!B6=0\n");        //bias refr
		m_usb->SendBytes("!B7=16777215\n"); //bias puY
		m_usb->SendBytes("!B8=569762\n");   //bias diffOn
		m_usb->SendBytes("!B9=7538\n");     //bias diff
		m_usb->SendBytes("!B10=51\n");      //bias foll
		m_usb->SendBytes("!B11=3\n");       //bias Pr
		m_usb->SendBytes("!ET0\n");			//set timestamp to 0

		//clean
		std::vector<unsigned char> buffer;
		do {
			buffer = m_usb->ReadBytes(4000); //empty the DVS communication buffer
		} while (buffer.size());
		std::cout << "DVS start" << std::endl;
	} else {
		std::cout << "DVS not start" << std::endl;
	}

	m_log = new logger("DVS_points", begin_timestamp, num_file);
	m_logTrack = new logger("Cluster_points", begin_timestamp, num_file);
	m_logCPU = new logger("DVS_timing", begin_timestamp, num_file);
	m_logCPUread = new logger("Read_timing", begin_timestamp, num_file);

    g_event[0].store(false);
    g_setpoint[0].store(0);
    g_feedback[0].store(0);
}

DVS::~DVS() {
	StopThread();
	std::cout << "evts ePID sended : " << m_cptEvts << std::endl;
    delete m_usb;
	delete m_log;
	delete m_logCPU;
	delete m_logCPUread;
	delete m_logTrack;
}

void* DVS::ThreadRun() {
	if(!m_usb->GetActive()) {StopThread();} //if no USB plug, thee stop the thread

	std::vector<unsigned char> buf, buffer;
	int x, y;
	unsigned int t;
	unsigned char c, p, b;
	bool test = false;
	int n;

	m_usb->SendBytes("E+\n");           //enable event sending
	while(GetStartValue()) {
    	//read datas
		m_logCPUread->Tic();
        buffer = m_usb->ReadBytes(4000); //reading 4000 bytes
		std::copy(buffer.begin(), buffer.end(), back_inserter(buf));
		m_logCPUread->Tac();

		if(buf.size() > m_len) {
			m_logCPU->Tic();
			//extraction of the event
			#if DVS_PACKET_TYPE == 0
				x = buf.at(0) & 0x7F;
				y = buf.at(1) & 0x7F;
				c = (buf.at(0) & 0x80) >> 7;
				p = (buf.at(1) & 0x80) >> 7;
				t = 0;
				test = c;
			#elif DVS_PACKET_TYPE == 1
				y = buf.at(0) & 0x7F;
				x = buf.at(1) & 0x7F;
				t = buf.at(2) & 0x7F;
				c = (buf.at(0) & 0x80) >> 7;
				p = (buf.at(1) & 0x80) >> 7;
				b = (buf.at(2) & 0x80) >> 7;
				if (!b) {
					b = (buf.at(3) & 0x80) >> 7;
					t = (t << 8) + (buf.at(3) & 0x7F);
					m_len = 4;
					if (!b) {
						b = (buf.at(4) & 0x80) >> 7;
						t = (t << 8) + (buf.at(4) & 0x7F);
						m_len = 5;
						if (!b) {
							b = (buf.at(5) & 0x80) >> 7;
							t = (t << 8) + (buf.at(5) & 0x7F);
							m_len = 6;
						}
					}
				}
				c = c & b;
				test = c || (t < m_Told);
			#elif DVS_PACKET_TYPE == 2
				y = buf.at(0) & 0x7F;
				x = buf.at(1) & 0x7F;
				c = (buf.at(0) & 0x80) >> 7;
				p = (buf.at(1) & 0x80) >> 7;
				t = (buf.at(2) << 8) + buf.at(3);
				test = c || (t < m_Told);
			#elif DVS_PACKET_TYPE == 3
				y = buf.at(0) & 0x7F;
				x = buf.at(1) & 0x7F;
				c = (buf.at(0) & 0x80) >> 7;
				p = (buf.at(1) & 0x80) >> 7;
				t = (buf.at(2) << 16) + (buf.at(3) << 8) + buf.at(4);
				test = c || (t < m_Told);
			#else
				y = buf.at(0) & 0x7F;
				x = buf.at(1) & 0x7F;
				c = !((buf.at(0) & 0x80) >> 7);
				p = (buf.at(1) & 0x80) >> 7;
				t = (buf.at(2) << 24) + (buf.at(3) << 16) + (buf.at(4) << 8) + buf.at(5);
				test = c || (t < m_Told);
			#endif

			//if the message is valid
			if(test) {
				if(c) {
					buf.erase(buf.begin()); //try to find the beggining of the messages
					std::cerr << "Error control" << std::endl;
				}
				#if DVS_PACKET_TYPE > 0
					if(t < m_Told) {
						std::cerr << "Error timestamp " << t << " " << m_Told << std::endl;
						m_Told = 0;
					}
				#endif
			} else {
				//saving
				#if DVS_PACKET_TYPE > 0
					m_Told = t;
				#endif
				if(std::pow((m_XClustPose - x), 2.0f) + std::pow((m_YClustPose - y), 2.0f) < m_Rmax) { //test if the event is in the 2 DoF cluster
					m_log->WriteN({x, y, 1});
					m_log->Tac();
					m_XClustPose = m_XClustPose * m_alpha + x * m_alpha_m1; //update the cluster position
					m_YClustPose = m_YClustPose * m_alpha + y * m_alpha_m1;
					const float temp3 = g_setpoint[0].load();
					m_logTrack->WriteFN({m_XClustPose, m_YClustPose, temp3});
					m_logTrack->TacF();
					const float temp2 = m_XClustPose*m_kx + m_u0; //convert the pixel position in [mm]
					const float temp = temp3 - temp2;
					if(std::fabs(temp) >= m_thresEvent) { //compute the event function
					    g_feedback[0].store(temp2);
				    	g_event[0].store(true);
						g_cv[0].notify_one();
						m_cptEvts++;
						//std::cout << "(" << m_XClustPose << ":" << m_YClustPose << ")" << std::endl;
					}
				} else {
					m_log->WriteN({x, y, 0});
					m_log->Tac();
				}
			}

			//erase buffer
			buf.erase(buf.begin(), buf.begin() + m_len);
			#if DVS_PACKET_TYPE == 1
				m_len = 3;
			#endif
			m_logCPU->Tac();
		}
    }
	m_usb->SendBytes("E-\n");           //disable event sending

	return ReturnFunction();
}

void DVS::toLengthRead() {
	switch(DVS_PACKET_TYPE) {
    	case 0:
    		m_len = 2;
    		break;
    	case 1:
    		m_len = 6;
    		break;
    	case 2:
    		m_len = 4;
    		break;
    	case 3:
    		m_len = 5;
    		break;
    	case 4:
    		m_len = 6;
    		break;
    }
}

void DVS::Restart() {
    m_usb->SendBytes("E-\n");           //disable event sending
    std::cout << "Reset DVS" << std::endl;
    m_usb->SendBytes("R\n");
    delay(5000);
    m_usb->SendBytes("E+\n");           //disable event sending
}
