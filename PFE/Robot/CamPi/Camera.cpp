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

camera::camera() {//constructeur
    cap = new raspicam::RaspiCam_Cv();
    cap->set(CAP_PROP_FPS,40);//La vitesse de la caméra est à 40 fps
    cap->set(CAP_PROP_FRAME_HEIGHT,128);//résolution de 128 pixels de haut
    cap->set(CAP_PROP_FRAME_WIDTH,128);//résolution de 128 pixels de large
    cap->open();//On ouvre la camera
}

camera::~camera() {//destructeur
    cap->release();
    delay(100);
    delete cap;
}

Mat camera::TakePic(void){//méthode pour prendre une photo
    Mat photo;
    cap->grab();//On prend une photo
    cap->retrieve(photo);//on récupère la photo
    return photo;
}

void camera::OpenCam(void){//Méthode pour ouvrir la caméra
    cap->open();

}
void camera::ReleaseCam(void){//Méthode pour "release" la caméra 
    cap->release();

}
