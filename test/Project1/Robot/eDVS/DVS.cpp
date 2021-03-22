#include "DVS.hpp"

DVS::DVS(const std::string nb_usb, const int bdrate, const unsigned char format, const bool prev)
	: BaseThread("DVS") {
	m_usb = new Usb(nb_usb, bdrate);
	Configuration(format, prev);
}

DVS::DVS(const int nb_usb, const int bdrate, const unsigned char format, const bool prev)
	: BaseThread("DVS") {
    m_usb = new Usb(nb_usb, bdrate);
	Configuration(format, prev);
}

void DVS::Configuration(const unsigned char format, const bool prev) {
	m_prev = prev;
	for (auto& item : m_pix) { std::atomic_init(&item, 0); }
	m_event.store(false);
	m_XClustPose = 64;
	m_YClustPose = 64;
	m_XClustPoseOld = 64;
	m_YClustPoseOld = 64;

	//config
	m_format = format;
	if (m_format > 5 || m_format < 0) { m_format = 4; }
	toLengthRead();
	char form = m_format + '0';
	std::vector<char> str = { '!', 'E', form, '\n' };

	if (m_usb->GetActive()) {
		Restart();

		std::cout << "Config DVS" << std::endl;
		m_usb->SendBytes("E-\n");           //disable event sending
		m_usb->SendBytes("!L2\n");          //LED blinking
		m_usb->SendBytes("!U0\n");          //UART echo mode none
		m_usb->SendBytes(str);              //select data format 2, 3-6, 4, 5, 6
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
			buffer = m_usb->ReadBytes(4000);
		} while (buffer.size());
		std::cout << "DVS start" << std::endl;
	} else {
		std::cout << "DVS not start" << std::endl;
	}
	m_log = new logger("DVS_points");
	m_logTrack = new logger("Cluster_points");
	m_logCPU = new logger("DVS_timing");
}

DVS::~DVS() {
    delete m_usb;
	delete m_log;
}

void* DVS::ThreadRun() {
	if(!m_usb->GetActive()) {StopThread();}

	std::vector<unsigned char> buf, buffer;
    std::vector<pointDVS<unsigned int>> data;
    pointDVS<unsigned int> d;

    auto begin_timestamp = std::chrono::high_resolution_clock::now();
    auto current_timestamp = std::chrono::high_resolution_clock::now();

	m_usb->SendBytes("E+\n");           //enable event sending
	while (GetStartValue()) {
    	//read datas
        buffer = m_usb->ReadBytes(4000);
		std::copy(buffer.begin(), buffer.end(), back_inserter(buf));

		if(buf.size() > m_len) {
			m_logCPU->Tic();
			d = toDatas(buf);
			if(!d.getValidTimestamp()) {buf.erase(buf.begin());} //test bug
			buf.erase(buf.begin(), buf.begin() + m_len);
			if(m_format == 1) {m_len = 3;}
			m_logCPU->Tac();
			//std::cout << d << std::endl;
			if(d.getValidTimestamp()) {data.push_back(d);}

        	//filter

			current_timestamp = std::chrono::high_resolution_clock::now();
			if(std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - begin_timestamp).count() >= 20) { // && data.size()
				begin_timestamp = std::chrono::high_resolution_clock::now();
				if(m_prev) {
					for(int i=0;i<16384;i++) { //store(true)
						m_pix[i].store(0);
					}
					for(int i=0;i<data.size();i++) {
						if(data.at(i).getPolarity()) {
							m_pix[(unsigned char)(data.at(i).getX()) * 128 + (unsigned char)(data.at(i).getY())].store(data.at(i).getTimestamp());
						} else {
							m_pix[(unsigned char)(data.at(i).getX()) * 128 + (unsigned char)(data.at(i).getY())].store(data.at(i).getTimestamp()*-1);
						}
					}
					data.clear();
				}
			}
		}
    }
	m_usb->SendBytes("E-\n");           //disable event sending

	return ReturnFunction();
}

std::vector<long int> DVS::getPolarities() {
	std::vector<long int> returnValue(m_pix.size());
	for(int i=0;i<m_pix.size();i++) {
		returnValue.at(i) = m_pix[i].load();
	}
	return returnValue;
}

void DVS::toLengthRead() {
	switch(m_format) {
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

pointDVS<unsigned int> DVS::toDatas(std::vector<unsigned char> buf) {
    long int x, y, t;
    unsigned char c, p, b;
	bool test = false;
	switch(m_format) {
		case 0:
			y = buf.at(0) & 0x7F;
			x = buf.at(1) & 0x7F;
			c = (buf.at(0) & 0x80) >> 7;
			p = (buf.at(1) & 0x80) >> 7;
			t = 0;
			test = c;
			break;
		case 1:
			y = buf.at(0) & 0x7F;
			x = buf.at(1) & 0x7F;
			t = buf.at(2) & 0x7F;
			c = (buf.at(0) & 0x80) >> 7;
			p = (buf.at(1) & 0x80) >> 7;
			b = (buf.at(2) & 0x80) >> 7;
			if(!b) {
				b = (buf.at(3) & 0x80) >> 7;
				t = (t<<8) + (buf.at(3)&0x7F);
				m_len = 4;
				if(!b) {
					b = (buf.at(4) & 0x80) >> 7;
					t = (t<<8) + (buf.at(4)&0x7F);
					m_len = 5;
					if(!b) {
    					b = (buf.at(5) & 0x80) >> 7;
    					t = (t<<8) + (buf.at(5)&0x7F);
						m_len = 6;
    				}
				}
			}
			c = c & b;
			test = (c && (t >= m_Told));
			break;
		case 2:
			y = buf.at(0) & 0x7F;
			x = buf.at(1) & 0x7F;
			c = (buf.at(0) & 0x80) >> 7;
			p = (buf.at(1) & 0x80) >> 7;
			t = (buf.at(2) << 8) + buf.at(3);
			test = (c && (t >= m_Told));
			break;
		case 3:
			y = buf.at(0) & 0x7F;
			x = buf.at(1) & 0x7F;
			c = (buf.at(0) & 0x80) >> 7;
			p = (buf.at(1) & 0x80) >> 7;
			t = (buf.at(2) << 16) + (buf.at(3) << 8) + buf.at(4);
			test = (c && (t >= m_Told));
			break;
		case 4:
			y = buf.at(0) & 0x7F;
			x = buf.at(1) & 0x7F;
			c = (buf.at(0) & 0x80) >> 7;
			p = (buf.at(1) & 0x80) >> 7;
			t = (buf.at(2) << 24) + (buf.at(3) << 16) + (buf.at(4) << 8) + buf.at(5);
			test = (c && (t >= m_Told));
			break;
	}
	pointDVS<unsigned int> pt(x, y, t, p, test);
	if(!test) {
		if(!c) {
			if (m_format && (t < m_Told)) {
				std::cerr << "Error timestamp " << t << " " << m_Told << std::endl;
				m_Told = 0;
			}
			std::cerr << "Error control" << std::endl;
		} else if(m_format && (t < m_Told)) {
			std::cerr << "Error timestamp " << t << " " << m_Told << std::endl;
			m_Told = 0;
		}
	} else {
		m_Told = t;
		m_log->Write({x, y, t}, false);
		m_log->Tac();
		if(std::fabs((m_XClustPose-x)+(m_YClustPose-y)) < m_Rmax) {
			m_XClustPose = m_XClustPose*m_alpha + x*m_alpha_m1;
			m_YClustPose = m_YClustPose*m_alpha + y*m_alpha_m1;
			m_logTrack->WriteD({m_XClustPose, m_YClustPose, static_cast<double>(t)}, false);
			m_logTrack->Tac();
			if(std::fabs((m_XClustPoseOld-m_XClustPose)+(m_YClustPoseOld-m_YClustPose)) > m_thresEvent) {
				m_XClustPoseOld = m_XClustPose;
				m_YClustPoseOld = m_YClustPose;
				m_XCluster.store((m_XClustPose-64)*m_fx);
				m_YCluster.store((m_YClustPose-64)*m_fy);
				m_lastTimestamp.store(t);
				m_event.store(true);
				std::cout << "(" << m_XClustPose << ":" << m_YClustPose << ")" << std::endl;
			}
		}
    }

	return pt;
}

void DVS::Restart() {
    m_usb->SendBytes("E-\n");           //disable event sending
    std::cout << "Reset DVS" << std::endl;
    m_usb->SendBytes("R\n");
    delay(5000);
    m_usb->SendBytes("E+\n");           //disable event sending
}

float DVS::GetXClusterPose() {return m_XCluster.load();}

float DVS::GetYClusterPose() {return m_YCluster.load();}

int DVS::GetRCluster() {return m_Rmax;}

long int DVS::GetLastT() {return m_lastTimestamp.load();}

int DVS::GetThreshold() {return m_thresEvent;}

bool DVS::GetEvent() {
	bool a = m_event.load();
	m_event.store(false);
	return a;
}
