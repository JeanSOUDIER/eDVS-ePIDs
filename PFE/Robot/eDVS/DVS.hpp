#ifndef DVS_H
#define DVS_H

#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include <string>
#include <cmath>
#include <wiringPi.h>
#include <chrono>

#include "../RS232_CPP/Usb.hpp"
#include "../BaseThread/BaseThread.hpp"
#include "../logger/logger.hpp"

#define DVS_PACKET_TYPE     4

class DVS : public BaseThread {
    public:
        DVS(const int nb_usb, const int bdrate, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float thresEvent = 2);
        DVS(const std::string nb_usb, const int bdrate, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float thresEvent = 2);
        ~DVS();

        int GetRCluster();
        int GetThreshold();
    protected:
        void* ThreadRun();

    private:
        void Restart();
    	void toLengthRead();
        void Configuration(std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file);

        int m_len;

        Usb *m_usb;

        unsigned int m_Told = 0;

        float m_XClustPose;
        float m_YClustPose;
        float m_XClustPoseOld;
        float m_YClustPoseOld;

        const int m_R = 5;
        const int m_safty = 2;
        const int m_Rmax = m_R*m_safty;
        const float m_alpha = 0.99;
        const float m_alpha_m1 = 1-m_alpha;
        const int m_thresEvent;

        const float m_z0 = -150;
        const float m_fx = 151;
        const float m_fy = m_fx;
        const float m_kx = m_fx/m_z0;
        const float m_ky = m_fy/m_z0;
        const float m_u0 = 64;
        const float m_v0 = m_u0;

        logger *m_log;
        logger *m_logTrack;
        logger *m_logCPU;
        logger *m_logCPUread;
        
        unsigned int m_cptEvts = 0;
};

#endif //DVS_H
