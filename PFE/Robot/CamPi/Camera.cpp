#include "../logger/logger.hpp"
#include "Camera.hpp"
#include <iostream>
#include <opencv2/aruco.hpp>
#include <unistd.h>
#include <ctime>
#include <wiringPi.h>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string.h>
#include <raspicam/raspicam_cv.h>
#include <raspicam/raspicam.h>

using namespace cv;
using namespace std;

camera::camera() {
    cap = new raspicam::RaspiCam_Cv();
    cap->set(CAP_PROP_FPS,71);
    cap->set(CAP_PROP_FRAME_HEIGHT,720);//720//1080//480
    cap->set(CAP_PROP_FRAME_WIDTH,1280);//1280//1920//640
    cap->open();//On ouvre la camera
}

camera::~camera() {
    cap->release();
    delay(100);
    delete cap;
}

Mat camera::TakePic(void){
    Mat photo;
    cap->grab();//On prend une photo
    cap->retrieve(photo);//on récupère la photo
    // cap->release();
    return photo;
}

void camera::OpenCam(void){
    cap->open();

}
void camera::ReleaseCam(void){
    cap->release();

}