#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#define PRINT

#ifdef PRINT
#include "look_up_table.hpp"
#endif

int main() {
    #ifndef PRINT
        std::ofstream file = std::ofstream("look_up_table.hpp");
        std::string data_type = "float";
        float lim_min = 0;
        float lim_max = 80;
        float epsilon = 0.01;

        file << "const float lim_min = " << static_cast<float>(lim_min) << ", lim_max = " << static_cast<float>(lim_max) << ", epsilon = " << static_cast<float>(1.0f/epsilon) << ";\n";
        file << "const " << data_type << " expo[] = {\n";
        double i_last;
        for(double i=lim_min;i<lim_max-epsilon;i+=epsilon) {
            if(data_type == "float") {
                file << "   " << static_cast<float>(std::exp(i)) << ",\n";
            } else if(data_type == "double") {
                file << "   " << static_cast<double>(std::exp(i)) << ",\n";
            }
            i_last = i;
        }
        if(data_type == "float") {
            file << "   " << static_cast<float>(std::exp(i_last+epsilon)) << "\n};";
        } else if(data_type == "double") {
            file << "   " << static_cast<double>(std::exp(i_last+epsilon)) << "\n};";
        }
        file.close();
        std::cout << "done" << std::endl;
    #else
        std::cout << expo[static_cast<int>(5.0*epsilon)] << std::endl;
    #endif
    return 0;
}
