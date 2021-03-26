#include<iostream>
#include <conio.h>
#include <ctime>

#if OS == OS_WINDOWS
#include<opencv2/opencv.hpp>
#include <Windows.h>
#define delay(x) Sleep(x)
#endif

//#include "Robot/eDVS/preview.hpp"
#include "Robot/eDVS/DVS.hpp"
#include "Robot/Controller/ePID.hpp"

int main() {
    logger log("Time");
    log.Write({ 0, 0 });
    log.Tic();

    DVS* eDVS_4337 = new DVS("COM4", 12000000);

    ePID myPID(1, 0, 0, 200000, 20000, 100, 0, 1, eDVS_4337);
    myPID.StartThread();
    eDVS_4337->StartThread();

    /*preview myPrev(128, 128, 5, eDVS_4337);
    myPrev.StartThread();

    while(myPrev.GetStartValue()) {
        cv::waitKey(2000);
        myPID.SetPoint(5);
        cv::waitKey(2000);
        myPID.SetPoint(-5);
    }*/

    int cpt = 0, toggle = 5;
    while(!GetKeyState(27) && eDVS_4337->GetStartValue()) {
        delay(20);
        cpt++;
        if (cpt > 100) {
            myPID.SetPoint(toggle);
            toggle *= -1;
            cpt = 0;
        }
    }

    myPID.StopThread();
    log.Tac();
    return 0;
}
