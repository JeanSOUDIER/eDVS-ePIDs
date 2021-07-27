#ifndef DETECTION_H
#define DETECTION_H
#include <atomic>
#include "camera.hpp"
#include "logger.hpp"
#include "BaseThread.hpp"
#include "pid.hpp"
#include "camera.hpp"
#include "logger.hpp"
#include "BaseThread.hpp"
#include "pid.hpp"
#include "pidN2.hpp"
#include <iostream>
//#include <opencv2/aruco.hpp>
#include <unistd.h>
#include <ctime>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string.h>
#include <raspicam/raspicam_cv.h>
#include <chrono>
#include <ctime>
using namespace cv;
using namespace std;

class detection : public BaseThread {
	public:
		detection(int AireMax, int AireMin, int Xmax, int Xmin, int Ymin, int Ymax, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp);
		~detection();

		Mat LireImage();
		void LirePosBille(void);
		void MakeVideo(void);
		void ReadPot(void);
		void SetCons(double a);
		// int GetPos();


	private:
		void* ThreadRun();
		int m_AireMax;
		int m_AireMin;
		int m_Xmax;
		int m_Xmin;
		int m_Ymin;
		int m_Ymax;
		
		float m_k=167/79;
		float m_mil=66;
        // float m_z0 = -311;
        // float m_fx = 3.04;
        // float m_fy = m_fx;
        // float m_kx = m_fx/m_z0;
        // float m_ky = m_fy/m_z0;
        // float m_u0 = 64;
        // float m_v0 = m_u0;
		
		//logger *m_log;
		camera *m_cam;
		pid *PIDBille;
		logger *m_log;
		logger *m_log_cpu;
		logger *m_log_compute;
		//const std::chrono::time_point<std::chrono::high_resolution_clock> m_begin_timestamp;

};

#endif
