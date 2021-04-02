#ifndef PREVIEW_H
#define PREVIEW_H

#include <iostream>
#include<opencv2/opencv.hpp>
#include <vector>

#include "pointDVS.hpp"
#include "DVS.hpp"
#include "../BaseThread/BaseThread.hpp"

class preview : public BaseThread {
    public:
        preview();
        preview(const int x_len, const int y_len, const int zoom, DVS* eDVS_4337 = NULL);
        ~preview();

        bool print(std::vector<pointDVS<unsigned int>> points);
        bool print();
        void PrintText(cv::InputOutputArray img, std::vector<std::string> str);

    private:
        bool sign(long int i);
        void* ThreadRun();

    	int m_xlen;
    	int m_ylen;
        int m_zoom;

        const int m_lignes = 3;
        const int m_htxt = m_lignes*40;

        DVS *m_eDVS_4337;
};

#endif //PREVIEW_H
