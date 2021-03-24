#include<iostream>

#include "Robot/eDVS/preview.hpp"
#include "Robot/Controller/ePID.hpp"
#include "Robot/logger/CpuUsage.hpp"

int main() {
    logger log("Time");
    log.Write({ 0, 0 });
    log.Tic();

    DVS* eDVS_4337 = new DVS("COM4", 12000000, 4, true);

    ePID myPID(1, 0, 0, 200000, 20000, 100, 0, 1, eDVS_4337);
    myPID.StartThread();

    preview myPrev(128, 128, 5, eDVS_4337);
    myPrev.StartThread();

    while(myPrev.GetStartValue()) {
        cv::waitKey(2000);
        myPID.SetPoint(5);
        cv::waitKey(2000);
        myPID.SetPoint(-5);
    }

    myPID.StopThread();

    log.Tac();
    return 0;
}
