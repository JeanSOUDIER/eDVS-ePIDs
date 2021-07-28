#ifndef DETECTION_H
#define DETECTION_H
#include <atomic>
#include "Camera.hpp"
#include "../logger/logger.hpp"
#include "../BaseThread/BaseThread.hpp"
#include <iostream>
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
		detection(int AireMax, int AireMin, int Xmax, int Xmin, int Ymin, int Ymax, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, int num_file, float elim);
		~detection();

		Mat LireImage();
		float LirePosBille(void);
		void MakeVideo(void);
		void ReadPot(void);
		void SetCons(double a);


	private:
		void* ThreadRun();
		int m_AireMax;
		int m_AireMin;
		int m_Xmax;
		int m_Xmin;
		int m_Ymin;
		int m_Ymax;

		float m_elim;
		
		float m_k=167/79;
		float m_mil=66;
		
		camera *m_cam;
		logger *m_log;
		logger *m_log_cpu;
		logger *m_log_compute;

};

#endif
