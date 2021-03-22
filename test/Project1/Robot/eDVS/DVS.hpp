#ifndef DVS_H
#define DVS_H

#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include <string>
#include <cmath>

#if OS == OS_WINDOWS
#include<opencv2/opencv.hpp>
#define delay(x) cv::waitKey(x)
#else
#include <WiringPi.h>
#endif

#include "../RS232_CPP/Usb.hpp"
#include "../RS232_CPP/rs232.hpp"
#include "filters.hpp"
#include "pointDVS.hpp"
#include "../BaseThread/BaseThread.hpp"
#include "../logger/logger.hpp"

class DVS : public BaseThread {
    public:
        DVS(const int nb_usb, const int bdrate = 12000000, const unsigned char format = 4, const bool prev = false);
        DVS(const std::string nb_usb, const int bdrate = 12000000, const unsigned char format = 4, const bool prev = false);
        ~DVS();

        std::vector<long int> getPolarities();
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
    	pointDVS<unsigned int> toDatas(std::vector<unsigned char> buf);
        void Configuration(const unsigned char format, const bool prev);

        bool m_prev = false;
        unsigned char m_format;
        int m_len;

        Usb *m_usb;

        unsigned int m_Told = 0;

        std::array<std::atomic<long int>, 16384> m_pix;

        std::atomic<float> m_XCluster;
        std::atomic<float> m_YCluster;
        float m_XClustPose;
        float m_YClustPose;
        float m_XClustPoseOld;
        float m_YClustPoseOld;

        const int m_R = 3;
        const int m_safty = 2;
        const int m_Rmax = (m_R*m_safty)*(m_R*m_safty);
        const float m_alpha = 0.99;
        const float m_alpha_m1 = 1-m_alpha;
        const int m_thresEvent = 20;

        std::atomic<long int> m_lastTimestamp;
        std::atomic<bool> m_event;

        const float m_z0 = 150;
        const float m_Gx = 151;
        const float m_Gy = m_Gx;
        const float m_fx = m_z0/m_Gx;
        const float m_fy = m_z0 / m_Gy;

        logger *m_log;
};

#endif //DVS_H
