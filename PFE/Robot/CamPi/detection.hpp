#ifndef DETECTION_H
#define DETECTION_H

#include "camera.hpp"
#include "logger.hpp"
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

class detection {
	public:
		detection(int AireMax, int AireMin, int Xmax, int Xmin, int Ymin, int Ymax);
		~detection();

		Mat LireImage();
		int LirePosBille();

	private:
		int m_AireMax;
		int m_AireMin;
		int m_Xmax;
		int m_Xmin;
		int m_Ymin;
		int m_Ymax;
		
		//logger *m_log;
		camera *m_cam;
		//const std::chrono::time_point<std::chrono::high_resolution_clock> m_begin_timestamp;

};

#endif
