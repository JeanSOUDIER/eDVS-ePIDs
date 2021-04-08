#ifndef DVS_H
#define DVS_H

#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include <string>
#include <cmath>
#include <wiringPi.h>

#include "../RS232_CPP/Usb.hpp"
#include "../BaseThread/BaseThread.hpp"
#include "../logger/logger.hpp"

#define DVS_PACKET_TYPE 4

class DVS : public BaseThread {
    public:
        DVS(const int nb_usb, const int bdrate = 12000000);
        DVS(const std::string nb_usb, const int bdrate = 12000000);
        ~DVS();

        float GetXClusterPose();
        float GetYClusterPose();
        int GetRCluster();
        long int GetLastT();
        int GetThreshold();
        bool GetEvent();
    protected:
        void* ThreadRun();

    private:
        void Restart();
    	void toLengthRead();
        void Configuration();

        int m_len;

        Usb *m_usb;

        unsigned int m_Told = 0;

        std::atomic<float> m_XCluster;
        std::atomic<float> m_YCluster;
        float m_XClustPose;
        float m_YClustPose;
        float m_XClustPoseOld;
        float m_YClustPoseOld;

        const int m_R = 3;
        const int m_safty = 2;
        const int m_Rmax = (m_R*m_safty)*(m_R*m_safty);
        const int m_RmaxM = -m_Rmax;
        const float m_alpha = 0.99;
        const float m_alpha_m1 = 1-m_alpha;
        const int m_thresEvent = 10;
        const int m_thresEventM = -m_thresEvent;

        std::atomic<long int> m_lastTimestamp;
        std::atomic<bool> m_event;

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
        logger *m_logTime;
};

#endif //DVS_H
