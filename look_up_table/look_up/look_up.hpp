#ifndef LOOK_UP_TABLE_H
#define LOOK_UP_TABLE_H

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <cmath>

template<class T>
class look_up {
	public:
		look_up(std::string file_name, std::function<T(T)> func, const double epsilon = 0.01f, const double lim_min = 0.0f, const double lim_max = 100.0f) {
			file_name = file_name+".hpp";
			std::ofstream file = std::ofstream(file_name);

			file << "const double lim_min = " << static_cast<T>(lim_min) << ", lim_max = " << static_cast<T>(lim_max) << ", epsilon = " << static_cast<T>(1.0f/epsilon) << ";\n";
			const char* a = typeid(T).name();
            std::string data_type;
            //std::cout << a[0] << std::endl;
			switch(a[0]) {
				case 'f':
					data_type = "float";
					break;
				case 'd':
					data_type = "double";
					break;
				default:
					data_type = "double";
					break;
			}
			file << "const " << data_type << " expo[] = {\n";

			double i_last;
	        for(double i=lim_min;i<lim_max-epsilon;i+=epsilon) {
	            file << "   	" << static_cast<T>(func(static_cast<T>(i))) << ",\n";
	            i_last = i;
	        }
	        file << "   	" << static_cast<T>(func(static_cast<T>(i_last+epsilon))) << "\n};";

	        file.close();
	        std::cout << "done" << std::endl;
		}
		~look_up() {}
};

#endif //LOOK_UP_TABLE_H
