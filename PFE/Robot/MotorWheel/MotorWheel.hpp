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
#include "../logger/logger.hpp"

/*

                     MotorWheel class

    SOUDIER Jean  (jean.soudier@insa-strasbourg.fr)


    Provides a class for formatted messages witha Teensy 4.0.

    Features :

    • Send and read data from USB

    Functions :

    • MotorWheel                     | Constructor with arguments nb_usb (the number or the name of the USB port) and baudrate (the baudrate) and the logger parameters
    • ReadPose()                     | Function to read the potentiometer value
    • SetHbridge(vel)                | Function to set the motor speed
    • SetSpeed(vel)                  | Function to send bytes
    • SetLim(lim)                    | Function to set the threshold of the sensor SOD (Send-On-Delta)
    • SetMiddlePoint(point)          | Function to set the initial point of of the SOD

*/

class MotorWheel {
    public:
        MotorWheel(const int nb_usb, const int bdrate, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float e_lim = 0, const int middle_point = 265);
        MotorWheel(const std::string nb_usb, const int bdrate, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float e_lim = 0, const int middle_point = 265);
        ~MotorWheel();

        void ReadPose();
        void SetHbridge(int vel);
        void SetSpeed(int vel);
        void SetLim(int lim);
        void SetMiddlePoint(int point);
    private:
        Usb *m_usb;

        std::vector<unsigned char> m_buf;
        float m_y = 0;

        const float m_elim;
        const int m_middle;

        logger *m_log;

        std::atomic<bool> m_mutexR, m_mutexW;
};

#endif //MOTORWHEEL_H
