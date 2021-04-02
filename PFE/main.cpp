/*#include <iostream>
#include <wiringPi.h>

#include "Robot/logger/logger.hpp"
#include "Robot/Controller/Hbridge.hpp"

#include <opencv2/opencv.hpp>

int main() {
    Hbridge Motor(28, 29); //38, 40

    Motor.Set(100);
    delay(2000);
    Motor.Set(-100);
    delay(2000);

    return 0;
}*/

#include <iostream>
#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

int main(int argc, char** argv)
{

        cv::Mat output, frame;
        int x = 787, y = 305;
        cv::Rect R(x, y, 128,128);
        raspicam::RaspiCam_Cv cap;


        if(!cap.open()) {
            std::cout << "Could not initialize capturing...\n";
            return 0;
        }


        while(1){
            cap.grab();
            cap.retrieve(output);
            
            frame = cv::Mat(output, R);
            
            cv::imshow("webcam input", frame);
            char c = (char)cv::waitKey(10);
            switch(c) {
                case 83:
                    //rigth
                    x++;
                    std::cout << "(" << x << ";" << y << ")" << std::endl;
                    R = cv::Rect(x, y, 128, 128);
                    break;
                case 81:
                    //left
                    x--;
                    std::cout << "(" << x << ";" << y << ")" << std::endl;
                    R = cv::Rect(x, y, 128, 128);
                    break;
                case 84:
                    //up
                    y++;
                    std::cout << "(" << x << ";" << y << ")" << std::endl;
                    R = cv::Rect(x, y, 128, 128);
                    break;
                case 82:
                    //down
                    y--;
                    std::cout << "(" << x << ";" << y << ")" << std::endl;
                    R = cv::Rect(x, y, 128, 128);
                    break;
            }
            if( c == 27 ) break;
        }
        
        cap.release();
}
