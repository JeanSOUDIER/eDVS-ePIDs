#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <atomic>
#include <chrono>

#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

#include "../RS232_CPP/Usb.hpp"
#include "../BaseThread/BaseThread.hpp"
#include "../logger/logger.hpp"

class Camera : public BaseThread {
	public:
		Camera(const unsigned int Te);
		~Camera();

        float GetXClusterPose();
        float GetYClusterPose();
        long int GetLastT();
    protected:
        void* ThreadRun();

    private:
    	raspicam::RaspiCam_Cv *cap;

    	cosnt unsigned int m_Te;

    	std::atomic<float> m_x;
    	std::atomic<float> m_y;

        const float m_z0 = -150;
        const float m_fx = 151;
        const float m_fy = m_fx;
        const float m_kx = m_fx/m_z0;
        const float m_ky = m_fy/m_z0;
        const float m_u0 = 64;
        const float m_v0 = m_u0;

        logger *m_logTrack;
        logger *m_logCPU;
        logger *m_logTime;
};

#endif //CAMERA_H