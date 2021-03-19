#ifndef DVS_H
#define DVS_H

#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include <string>

#if OS == OS_WINDOWS
#include<opencv2/opencv.hpp>
#define delay(x) cv::waitKey(x)
#else
#include <WiringPi.h>
#endif

#include "../RS232_CPP/Usb.hpp"
#include "../RS232_CPP/rs232.hpp"
#include "filters.hpp"
#include "pointDVS.hpp"
#include "../BaseThread/BaseThread.hpp"

class DVS : public BaseThread {
    public:
        DVS(const int nb_usb, const int bdrate = 12000000, const unsigned char format = 4, const bool prev = false);
        DVS(const std::string nb_usb, const int bdrate = 12000000, const unsigned char format = 4, const bool prev = false);
        ~DVS();

        std::vector<long int> getPolarities();
        void Displayed();
        long int getTaff();
    protected:
        void* ThreadRun();

    private:
        void Restart();
    	void toLengthRead();
    	pointDVS<unsigned int> toDatas(std::vector<unsigned char> buf);
        bool toDatasAtomic(std::vector<unsigned char> buf);
        void Configuration(const unsigned char format, const bool prev);

        bool m_prev = false;
        unsigned char m_format;
        int m_len;

        Usb *m_usb;

        unsigned int m_Told = 0;

        std::array<std::atomic<long int>, 16384> m_pix;
        std::atomic<long int> m_Taff;
};

#endif //DVS_H
