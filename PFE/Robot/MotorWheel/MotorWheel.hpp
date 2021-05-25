#ifndef MOTORWHEEL_H
#define MOTORWHEEL_H

#include <cmath>
#include <iostream>
#include <vector>
#include <string>

#include "../RS232_CPP/Usb.hpp"

class MotorWheel {
    public:
        MotorWheel(const int nb_usb, const int bdrate);
        MotorWheel(const std::string nb_usb, const int bdrate);
        ~MotorWheel();

        //void GoPos(const double x, const double y, const double a);
        //void SetSpeed(std::vector<int> speed);
        int ReadPose();
        void SetHbridge(int vel);
        void SetSpeed(int vel);
    private:
        Usb *m_usb;

        std::vector<unsigned char> m_buf;
        int m_temp = 0;
};

#endif //MOTORWHEEL_H
