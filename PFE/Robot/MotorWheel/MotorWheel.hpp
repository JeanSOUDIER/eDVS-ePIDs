#ifndef MOTORWHEEL_H
#define MOTORWHEEL_H

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <atomic>

#include "../BaseThread/BaseThread.hpp"
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
        void SetLim(int lim);
        void SetBegin(int val);
    private:
        Usb *m_usb;

        std::vector<unsigned char> m_buf;
        int m_temp = 0;

        std::atomic<bool> m_mutexR, m_mutexW;
};

#endif //MOTORWHEEL_H
