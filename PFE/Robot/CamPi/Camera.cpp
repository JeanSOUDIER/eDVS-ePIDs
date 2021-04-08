#include "Camera.hpp"

Camera::Camera(const unsigned int Te)
	: BaseThread("CamPi"), m_Te(Te) {
	cap = new raspicam::RaspiCam_Cv();

	if(!cap->open()) {
        std::cout << "Could not initialize capturing...\n";
    } else {
    	m_x.store(0);
    	m_y.store(0);
    	StartThread();
    }

	m_logTrack = new logger("Region_points");
	m_logCPU = new logger("Camera_timing");

    m_logTime = new logger("Time");
    m_logTime->Write({ 0, 0 });
	m_logTime->Tic();

    std::cout << "Camera Pi Start" << std::endl;
}

Camera::~Camera() {
	delete cap;
	delete m_logCPU;
	delete m_logTrack;
	delete m_logTime;
}

void* Camera::ThreadRun() {
	while (GetStartValue()) {
		auto begin_timestamp = std::chrono::high_resolution_clock::now();
		Process();
		auto current_timestamp = std::chrono::high_resolution_clock::now();
		while(std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - begin_timestamp).count() < m_Te) {
			current_timestamp = std::chrono::high_resolution_clock::now();
		}
	}
    cap.release();
	return ReturnFunction();
}

void Camera::Process() {
	m_logCPU->Tic();
	cv::Mat output, gray;

	//getImg
	cap->grab();
    cap->retrieve(output);

    //to gray
    cv::cvtColor(output, gray, cv::CV_BGR2GRAY);

    //to segmentation
    cv::threshold(gray, gray, 0, 255, cv::CV_THRESH_OTSU);

    //to regionprops
    std::vector<std::vector<cv::Point>> cnt;
    std::vector<cv::Vec4i> hier;
    cv::Rect rectan;
    cv::findContours(gray, cnt, hier, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    for(size_t i=0;i<cnt.size();i++) {
        if(cv::contourArea(cnt.at(i)) > 5000 && cv::contourArea(cnt.at(i)) < 90000) {
            rectan = cv::boundingRect(cnt.at(i));
            break;
        }
    }

    m_x.store(rectan.x+rectan.height/2);
    m_y.store(rectan.y+rectan.width/2);
	
	m_logTrack->WriteFN({m_x.load(), m_y.load(), 0});
	m_logTrack->TacF();
	m_logCPU->Tac();
}

float Camera::GetXClusterPose() {return m_x.load();}

float Camera::GetYClusterPose() {return m_y.load();}

long int Camera::GetLastT() {return m_Te;}