#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <atomic>
#include <chrono>

#include <wiringPi.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <raspicam/raspicam_cv.h>

#include "../RS232_CPP/Usb.hpp"
#include "../BaseThread/BaseThread.hpp"
#include "../logger/logger.hpp"

class Camera : public BaseThread {
	public:
		Camera(const unsigned int Te, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp);
		~Camera();

        float GetXClusterPose();
        float GetYClusterPose();
        const long int GetLastT();
    protected:
        void* ThreadRun();

    private:
        void Process();

    	raspicam::RaspiCam_Cv *cap;

    	const unsigned int m_Te;

    	std::atomic<float> m_x;
    	std::atomic<float> m_y;

        const float m_z0 = -250;
        const float m_fx = 112;
        const float m_fy = m_fx;
        const float m_kx = m_fx/m_z0;
        const float m_ky = m_fy/m_z0;
        const float m_u0 = 960;
        const float m_v0 = 540;

        logger *m_logTrack;
        logger *m_logCPU;
        logger *m_logTime;
};

#endif //CAMERA_H
