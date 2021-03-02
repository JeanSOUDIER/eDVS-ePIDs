#include "preview.hpp"

preview::preview()
	: preview(128, 128, 5) {}

preview::preview(const int x_len, const int y_len, const int zoom) {
    m_xlen = x_len;
    m_ylen = y_len;
	m_zoom = zoom;

	eDVS_4337 = new DVS("COM4", 12000000, 4, true);
	eDVS_4337->StartThread();
	std::cout << "Preview Start" << std::endl;
}

preview::~preview() {}

bool preview::print(std::vector<pointDVS<unsigned int>> points) {
	bool test = true;
	cv::Mat img(m_xlen*m_zoom, m_ylen * m_zoom, CV_8UC1, cv::Scalar(128, 128, 128));
	for(int i=0;i<points.size();i++) {
		if (points.at(i).getValidTimestamp()) {
			cv::Rect r(points.at(i).getX() * m_zoom, points.at(i).getY() * m_zoom, m_zoom, m_zoom);
			if (points.at(i).getPolarity()) {
				cv::rectangle(img, r, cv::Scalar(0, 0, 0), -1);
			} else {
				cv::rectangle(img, r, cv::Scalar(255, 255, 255), -1);
			}
		}
	}
	cv::imshow("eDVS-4337", img);
	if (cv::waitKey(100) >= 0) {
		test = false;
	}
	return test;
}

bool preview::print() {
	bool test = true;
	double cpt = 0;
	int last_id = 0;

	if(!eDVS_4337->GetStartValue()) {
		std::cout << "Thread DVS not start" << std::endl;
		test = false;
	}

	std::vector<long int> v = eDVS_4337->getPolarities();
	cv::Mat img(m_xlen*m_zoom+m_htxt, m_ylen*m_zoom, CV_8UC1, cv::Scalar(128, 128, 128));
	for (int i = 0; i < v.size(); i++) {
		cv::Rect r((i % m_xlen)*m_zoom+m_htxt, (int)(i / m_xlen)*m_zoom, m_zoom, m_zoom);
		if(v.at(i) != 0) {
			cpt++;
			last_id = i;
			cv::rectangle(img, r, cv::Scalar(255*sign(v.at(i)), 255*sign(v.at(i)), 255*sign(v.at(i))), -1);
		}
	}
	cv::Rect re(0, 0, m_ylen*m_zoom, m_htxt);
	cv::rectangle(img, re, cv::Scalar(200, 200, 200), -1);
	std::vector<std::string> str = {"Number of events : " + std::to_string(static_cast<int>(cpt)),
									"last timestamp : " + std::to_string(std::fabs(v.at(last_id) / 1000000.0)) + " s",
									std::to_string(cpt / 20.0) + " kevt/s"};
	PrintText(img, str);

	cv::imshow("eDVS-4337", img);
	if (cv::waitKey(20) >= 0) {
		test = false;
	}
	return test;
}

bool preview::sign(long int i) {
	return (i >= 0);
}

void preview::PrintText(cv::InputOutputArray img, std::vector<std::string> str) {
	for(int i=0;i<m_lignes;i++) {
		cv::Point pt(2, (i+1)*40-10);
		cv::putText(img, str.at(i), pt, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
	}
}
