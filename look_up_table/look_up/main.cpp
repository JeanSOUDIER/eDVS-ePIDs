#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <functional>

#include "look_up.hpp"

//#define PRINT
#define DATA_TYPE double

#ifdef PRINT
#include "look_up_table.hpp"
#endif

int main() {
    #ifndef PRINT
        std::function<DATA_TYPE(DATA_TYPE)> f_exp = [=](const DATA_TYPE& in) {return static_cast<DATA_TYPE>(std::exp(in));};
        look_up<DATA_TYPE> table("look_up_table", f_exp, 0.001f, -80, 80);
    #else
        std::cout << expo[static_cast<int>(5.0*epsilon)] << std::endl;
    #endif

    return 0;
}
