#ifndef PREVIEW_H
#define PREVIEW_H

#include <iostream>
#include<opencv2/opencv.hpp>
#include <vector>

#include "pointDVS.hpp"
#include "DVS.hpp"

class preview {
    public:
        preview();
        preview(const int x_len, const int y_len, const int zoom);
        ~preview();

        bool print(std::vector<pointDVS<unsigned int>> points);
        bool print();
        void PrintText(cv::InputOutputArray img, std::vector<std::string> str);

    private:
        bool sign(long int i);

    	int m_xlen;
    	int m_ylen;
        int m_zoom;

        const int m_lignes = 3;
        const int m_htxt = m_lignes*40;

        DVS *eDVS_4337;
};

#endif //PREVIEW_H
