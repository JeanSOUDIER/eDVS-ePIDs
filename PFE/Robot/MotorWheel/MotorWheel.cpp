#include "MotorWheel.hpp"

MotorWheel::MotorWheel(const int nb_usb, const int bdrate) {
    m_usb = new Usb(nb_usb, bdrate);
    std::cout << "MotorWheel start" << std::endl;
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

void MotorWheel::SetSpeed(int vel) {
    if(vel > 1023) {vel = 1023;std::cout << "speed sat H" << std::endl;}
    if(vel < 0) {vel = 0;std::cout << "speed sat L" << std::endl;}
    const unsigned char Lc = static_cast<unsigned char>(vel>>8);
    const unsigned char Ld = static_cast<unsigned char>(vel%256);
    const int cc = Lc+Ld;
    const unsigned char c = static_cast<unsigned char>(cc%256);
    const std::vector<char> sending{static_cast<char>(255), static_cast<char>(Lc), static_cast<char>(Ld), static_cast<char>(c)};
    m_usb->SendBytes(sending);
}
