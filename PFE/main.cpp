#include <iostream>
#include <wiringPi.h>

#include "Robot/logger/logger.hpp"
#include "Robot/Controller/Hbridge.hpp"

#include <opencv2/opencv.hpp>

int main() {
    Hbridge Motor(28, 29, 1); //38, 40

    for(int i=0;i<10;i++) {
    	std::cout << i << std::endl;
	    Motor.Set(200);
	    delay(2000);
	    Motor.Set(-200);
	    delay(2000);
    }

    return 0;
}
