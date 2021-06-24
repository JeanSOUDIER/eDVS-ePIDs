#ifndef CAMERA_H
#define CAMERA_H
#include "logger.hpp"
#include<iostream>
#include <unistd.h>
#include <ctime>
#include <fstream>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <string.h>
#include <raspicam/raspicam_cv.h>
using namespace cv;
using namespace std;

class camera {
public:
	camera();
	~camera();

	Mat TakePic(void);
	void ReleaseCam(void);
	void OpenCam(void);

private:
	int m_a;
	
    raspicam::RaspiCam_Cv *cap;
};

#endif