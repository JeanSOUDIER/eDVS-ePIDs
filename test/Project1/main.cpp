#include <iostream>
#include <wiringPi.h>

#include "Robot/logger/logger.hpp"
#include "Robot/Controller/Hbridge.hpp"

int main() {
    Hbridge Motor(28, 29); //38, 40

    Motor.Set(100);
    delay(2000);
    Motor.Set(-100);
    delay(2000);

    return 0;
}
