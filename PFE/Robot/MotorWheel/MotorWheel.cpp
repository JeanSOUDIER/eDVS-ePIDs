#include "MotorWheel.hpp"

MotorWheel::MotorWheel(const std::string nb_usb, const int bdrate, const float e_lim, const int middle_point)
    : m_elim(e_lim), m_middle(middle_point) {
    m_usb = new Usb(nb_usb, bdrate);
    std::cout << "MotorWheel start" << std::endl;

    m_mutexR.store(false);
    m_mutexW.store(false);

    g_event[1].store(false);
    g_setpoint[1].store(0);
    g_feedback[1].store(0);
}

MotorWheel::MotorWheel(const int nb_usb, const int bdrate, const float e_lim, const int middle_point)
    : m_elim(e_lim), m_middle(middle_point) {
    m_usb = new Usb(nb_usb, bdrate);

    std::cout << "e_lim pot = " << m_elim << std::endl;

    std::cout << "MotorWheel start" << std::endl;

    m_mutexR.store(false);
    m_mutexW.store(false);

    g_event[1].store(false);
    g_setpoint[1].store(0);
    g_feedback[1].store(0);
}

MotorWheel::~MotorWheel() {
    delete m_usb;
}

/*void MotorWheel::GoPos(const double x, const double y, const double a) {
    const double r = sqrt(x*x+y*y);
        double gamma;
        if(x == 0 && y == 0) {
            gamma = 0;
        } else if(y == 0 && x < 0) {
            gamma = M_PI;
        } else {
            gamma = 2*atan(y/(x+r));
        }
        gamma += M_PI;
        gamma = std::fmod(gamma,2*M_PI);
        gamma -= M_PI;
        if(gamma != 0) { //go to the angle
            SetSpeed(-SPEED_CST*Utility::sign(gamma),SPEED_CST*Utility::sign(gamma));
        delay(std::abs(gamma)*2*SPEED_ANGLE);
        }
    if(r != 0) { //go to the distance
        SetSpeed(SPEED_CST*Utility::sign(r),SPEED_CST*Utility::sign(r));
        std::cout << r*SPEED_NORM << std::endl;
        delay(r*SPEED_NORM);
    }
    if(a != 0) { //go to the end angle
        SetSpeed(-SPEED_CST*Utility::sign(a),SPEED_CST*Utility::sign(a));
        delay(std::abs(a)*SPEED_ANGLE);
    }
    SetSpeed(0, 0);
}*/

/*void MotorWheel::SetSpeed(std::vector<int> speed) {
    SetSpeed(speed.at(0), speed.at(1));
}*/

void MotorWheel::ReadPose() {
    std::vector<unsigned char> buffer;
    while(m_mutexW.load()) {}
    m_mutexR.store(true);
    buffer = m_usb->ReadBytes(4000);
    m_mutexR.store(false);
    std::copy(buffer.begin(), buffer.end(), back_inserter(m_buf));
    while(m_buf.size() > 3) {
        if(m_buf.at(0) == 255) {
            if((m_buf.at(1)+m_buf.at(2))%256 == m_buf.at(3)) {
                int temp = m_buf.at(1);
                temp = temp << 8;
                temp += m_buf.at(2);
                m_y = temp;
                m_y = (m_y-m_middle)*0.065f;
                const float e = g_setpoint[1].load() - m_y;
                if(std::fabs(e) >= m_elim) {
                    g_feedback[1].store(m_y);
                    if(!g_event[1].load()) {
                        g_event[1].store(true);
                        g_cv[1].notify_one();
                    }
                }
                //std::cout << "mesure " << temp << " " << m_y << std::endl;
                m_buf.erase(m_buf.begin(), m_buf.begin() + 4);
            } else {
                std::cout << "error cc" << std::endl;
                m_buf.erase(m_buf.begin());
            }
        } else {
            std::cout << "error sb" << std::endl;
            m_buf.erase(m_buf.begin());
        }
    }
}

void MotorWheel::SetHbridge(int vel) {
    if(std::fabs(vel) > 70) {
        if(vel > 0) {
            if(vel > 256) {vel = 256;}
            SetSpeed(vel + 0x1000);
        } else {
            vel *= -1;
            if(vel > 256) {vel = 256;}
            SetSpeed(vel);
        }
    } else {
        SetSpeed(0);
    }
}

void MotorWheel::SetLim(int lim) {
    if(lim > 256) {lim = 256;}
    if(lim < 0) {lim = 0;}
    SetSpeed(lim + 0x4000);
}

void MotorWheel::SetMiddlePoint(int point) {
    if(point > 1023) {point = 1023;}
    if(point < 0) {point = 0;}
    SetSpeed(point + 0x8000);
}

//void MotorWheel::SetBegin(int val) {m_temp = val;}

void MotorWheel::SetSpeed(int vel) {
    if(vel > 65535) {vel = 65535;std::cout << "speed sat H" << std::endl;}
    if(vel < 0) {vel = 0;std::cout << "speed sat L" << std::endl;}
    const unsigned char Lc = static_cast<unsigned char>(vel>>8);
    const unsigned char Ld = static_cast<unsigned char>(vel%256);
    const int cc = Lc+Ld;
    const unsigned char c = static_cast<unsigned char>(cc%256);
    const std::vector<char> sending{static_cast<char>(255), static_cast<char>(Lc), static_cast<char>(Ld), static_cast<char>(c)};
    while(m_mutexR.load()) {}
    m_mutexW.store(true);
    m_usb->SendBytes(sending);
    m_mutexW.store(false);
}
