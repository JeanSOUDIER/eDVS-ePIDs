#ifndef MOTORWHEEL_H
#define MOTORWHEEL_H

#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <mutex>

#include "../BaseThread/BaseThread.hpp"
#include "../RS232_CPP/Usb.hpp"

class MotorWheel {
    public:
        MotorWheel(const int nb_usb, const int bdrate, const float e_lim = 0, const int middle_point = 265);
        MotorWheel(const std::string nb_usb, const int bdrate, const float e_lim = 0, const int middle_point = 265);
        ~MotorWheel();

        //void GoPos(const double x, const double y, const double a);
        //void SetSpeed(std::vector<int> speed);
        void ReadPose();
        void SetHbridge(int vel);
        void SetSpeed(int vel);
        void SetLim(int lim);
        //void SetBegin(int val);
        void SetMiddlePoint(int point);
    private:
        Usb *m_usb;

        std::vector<unsigned char> m_buf;
        float m_y = 0;

        const float m_elim;
        const int m_middle;

        std::atomic<bool> m_mutexR, m_mutexW;
};

#endif //MOTORWHEEL_H
