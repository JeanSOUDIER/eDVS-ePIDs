#include<iostream>
#include "Robot/eDVS/preview.hpp"
#include "Robot/Controller/ePID.hpp"

int main() {
    DVS* eDVS_4337 = new DVS("COM4", 12000000, 4, true);

    ePID myPID(1, 0, 0, 200000, 20000, 100, 0, 1, eDVS_4337);
    myPID.StartThread();

    preview myPrev(128, 128, 5, eDVS_4337);
    myPrev.StartThread();

    while(myPrev.GetStartValue()) {}

    myPID.StopThread();

    return 0;
}
