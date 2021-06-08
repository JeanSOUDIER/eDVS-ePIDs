#include "Usb.hpp"

Usb::Usb() 
    : m_port_nr(0), m_bdrate(0) {
    m_active = false;
}

Usb::Usb(const std::string nb_usb, const int baudrate)
    : m_port_nr(RS232_GetPortnr(nb_usb.c_str())), m_bdrate(baudrate) {Config();}

Usb::Usb(const int nb_usb, const int baudrate)
    : m_port_nr(nb_usb), m_bdrate(baudrate) {Config();}

void Usb::Config() {
    if(RS232_OpenComport(m_port_nr,m_bdrate,"8N1",0)) { // 8 data bits, no parity, 1 stop bit
        std::cout << "Can not open comport\n" << m_port_nr << " " << m_bdrate << std::endl;
        m_active = false;
    } else {
        m_active = true;
    }
}

Usb::~Usb() {
    RS232_CloseComport(m_port_nr);
}

void Usb::SendBytes(const std::string &s) {
    std::vector<char> v;
    std::copy(s.begin(), s.end(), std::back_inserter(v));
    SendBytes(v);
}

void Usb::SendBytes(const std::vector<char> &data) {
    unsigned char msg[data.size()];
    std::copy(data.begin(), data.end(), msg);
    RS232_SendBuf(m_port_nr, &msg[0], data.size()); //test
    //RS232_SendBuf(m_port_nr, &data[0], data.size());
    RS232_flushRXTX(m_port_nr);
    /*std::copy(data.begin(), data.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    std::copy(data.begin(), data.end(), std::ostream_iterator<char>(std::cout, ""));
    std::cout << std::endl;*/
}

std::vector<unsigned char> Usb::ReadBytes(const int n) {
    unsigned char raw_bytes[n];
    const int n_r = RS232_PollComport(m_port_nr, raw_bytes, n);
    //if(n_r) {std::cout << "n_r = " << n_r << std::endl;}
    /*std::vector<unsigned char> ret(n_r);
    for(unsigned int i=0;i<ret.size();i++) {
        ret.at(i) = static_cast<unsigned char>(raw_bytes[i]);
        //if(ret.at(i)) {std::cout << ret.at(i);} else {std::cout << "\0"; }
    }*/
    std::vector<unsigned char> ret(raw_bytes,raw_bytes+n_r);
    return ret;
}

int Usb::ReadBytes(const int n, unsigned char *buf) {
    const int n_r = RS232_PollComport(m_port_nr, buf, n);
    return n_r;
}

const int Usb::GetBdRate(void) {return m_bdrate;}
const int Usb::GetPortNb(void) {return m_port_nr;}
bool Usb::GetActive(void) {return m_active;}

void Usb::SetActive(bool state) {m_active = state;}
